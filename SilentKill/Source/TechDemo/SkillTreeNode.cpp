// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "SkillTreeNode.h"


USkillTreeNode::USkillTreeNode()
{
	Left = nullptr;
	Right = nullptr;
	Up = nullptr;
	Down = nullptr;
}

void USkillTreeNode::BeginPlay()
{
	Learned = false;
	Selected = false;
	SkillMaterial = UMaterialInstanceDynamic::Create(this->GetMaterial(0), this);
	SetMaterial(0, SkillMaterial);
}

void USkillTreeNode::CalculateState()
{
	if(Selected)
	{
		if(Learned)
		{
			NodeStateVector = FLinearColor(0.f, 0.f, 0.f, 1.f);
		}
		else
		{
			NodeStateVector = FLinearColor(0.f, 0.f, 1.f, 0.f);
		}
	}
	else
	{
		if (Learned)
		{
			NodeStateVector = FLinearColor(0.f, 1.f, 0.f, 0.f);
		}
		else
		{
			NodeStateVector = FLinearColor(1.f, 0.f, 0.f, 0.f);
		}
	}
	SkillMaterial->SetVectorParameterValue(TEXT("State"), NodeStateVector);
}


void USkillTreeNode::ChangeSelection(bool selected)
{
	Selected = selected;
	CalculateState();
}

void USkillTreeNode::ChangeLearned(bool learned)
{
	Learned = learned;
	CalculateState();
	CalculateConnectors();
}

int USkillTreeNode::GetCost() const
{
	return Cost;
}

bool USkillTreeNode::GetLearned() const
{
	return Learned;
}

void USkillTreeNode::CalculateConnectors()
{
	int size = NodeConnectors.Num();
	for (int i = 0; i < size; ++i)
	{
		if(!NodeConnectors[i]->GetLearned())
		{
			NodeConnectors[i]->ChangeLearned(Learned);
		}
	}
}
