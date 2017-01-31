// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "SkillTree.h"


// Sets default values
ASkillTree::ASkillTree()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASkillTree::BeginPlay()
{
	Super::BeginPlay();
	//skills
	this->GetComponents<USkillTreeNode>(AllSkills);
	Parent = AllSkills[0];
	Parent->Up = AllSkills[1];
	AllSkills[1]->Down = AllSkills[0];
	AllSkills[1]->Left = AllSkills[2];
	AllSkills[1]->Up = AllSkills[3];
	AllSkills[2]->Down = AllSkills[0];
	AllSkills[2]->Right = AllSkills[3];
	AllSkills[2]->Up = AllSkills[4];
	AllSkills[3]->Down = AllSkills[2];
	AllSkills[3]->Left = AllSkills[2];
	AllSkills[3]->Up = AllSkills[4];
	AllSkills[4]->Down = AllSkills[2];
	AllSkills[4]->Up = AllSkills[5];
	AllSkills[5]->Down = AllSkills[4];
	AllSkills[5]->Up = AllSkills[6];
	AllSkills[6]->Down = AllSkills[5];
	//connectors
	TArray<USkillTreeNodeConnector*> connectors;
	this->GetComponents<USkillTreeNodeConnector>(connectors);
	AllSkills[0]->NodeConnectors.Add(connectors[0]);
	AllSkills[0]->NodeConnectors.Add(connectors[1]);
	AllSkills[1]->NodeConnectors.Add(connectors[2]);
	AllSkills[2]->NodeConnectors.Add(connectors[3]);
	AllSkills[2]->NodeConnectors.Add(connectors[4]);
	AllSkills[4]->NodeConnectors.Add(connectors[5]);
	AllSkills[5]->NodeConnectors.Add(connectors[6]);
	//anim
	TArray<USkeletalMeshComponent*> skeletalMesh;
	this->GetComponents<USkeletalMeshComponent>(skeletalMesh);
	SkeletalMesh = skeletalMesh[0];
	//feather
	TArray<UFeather*> feathers;
	this->GetComponents<UFeather>(feathers);
	Feather = feathers[0];
}

void ASkillTree::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ASkillTree::OnUse()
{
	if (CurrentSelected == nullptr) return;
	if( SkillPoints >= CurrentSelected->GetCost() && !CurrentSelected->GetLearned() && (CurrentSelected->Down == nullptr || (CurrentSelected->Down != nullptr && CurrentSelected->Down->GetLearned())))
	{
		CurrentSelected->ChangeLearned();
		SkillPoints -= CurrentSelected->GetCost();
		Feather->ResetAll();
		Feather->ShowAvailable(SkillPoints);
	}
}

void ASkillTree::ChangeSelection(EDirection direction)
{
	USkillTreeNode* tmp = nullptr;
	switch(direction)
	{
		case Left:
			tmp = CurrentSelected->Left;
			break;
		case Up:
			tmp = CurrentSelected->Up;
			break;
		case Right:
			tmp = CurrentSelected->Right;
			break;
		case Down:
			tmp = CurrentSelected->Down;
			break;
	}
	tmp->ChangeSelection(true);
	CurrentSelected->ChangeSelection(false);
	CurrentSelected = tmp;
	Feather->ShowAvailable(SkillPoints);
	if(!CurrentSelected->GetLearned())
	{
		Feather->NotLearnedSelected(SkillPoints, CurrentSelected->GetCost());
	}
	SkeletalMesh->PlayAnimation(CurrentSelected->SkillAnimation, true);
}

void ASkillTree::OnHorizontalMove(float x)
{
	if( x == 0)
		HBlock = false;
	if (CurrentSelected == nullptr || HBlock || VBlock) return;
	if (x > 0)
	{
		if (CurrentSelected->Right != nullptr)
		{
			HBlock = true;
			ChangeSelection(Right);
		}
	}
	else if(x < 0)
	{
		if (CurrentSelected->Left != nullptr)
		{
			HBlock = true;
			ChangeSelection(Left);
		}
	}
}

void ASkillTree::OnVerticalMove(float x)
{
	if (x == 0)
		VBlock = false;
	if (CurrentSelected == nullptr || HBlock || VBlock) return;
	if(x > 0)
	{
		if(CurrentSelected->Up != nullptr)
		{
			VBlock = true;
			ChangeSelection(Up);
		}
	}
	else if (x < 0)
	{
		if (CurrentSelected->Down != nullptr)
		{
			VBlock = true;
			ChangeSelection(Down);
		}
	}
}

void ASkillTree::OnShow()
{
	if(CurrentSelected != nullptr)
		CurrentSelected->ChangeSelection(false);
	CurrentSelected = Parent;
	CurrentSelected->ChangeSelection(true);
	SkeletalMesh->PlayAnimation(CurrentSelected->SkillAnimation, true);
	int size = AllSkills.Num();
	for (int i = 0; i < size; ++i)
	{
		AllSkills[i]->CalculateState();
	}
	Feather->ShowAvailable(SkillPoints);
	if (!CurrentSelected->GetLearned())
	{
		Feather->NotLearnedSelected(SkillPoints, CurrentSelected->GetCost());
	}
}
