// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "SkillTreeNodeConnector.h"


void USkillTreeNodeConnector::BeginPlay()
{
	Learned = false;
	SkillMaterial = UMaterialInstanceDynamic::Create(this->GetMaterial(0), this);
	SetMaterial(0, SkillMaterial);
}

void USkillTreeNodeConnector::CalculateState()
{
	if (Learned)
	{
		NodeStateVector = FLinearColor(0.f, 1.f, 0.f, 0.f);
	}
	else
	{
		NodeStateVector = FLinearColor(1.f, 0.f, 0.f, 0.f);
	}

	SkillMaterial->SetVectorParameterValue(TEXT("State"), NodeStateVector);
}

void USkillTreeNodeConnector::ChangeLearned(bool learned)
{
	Learned = learned;
	CalculateState();
}

bool USkillTreeNodeConnector::GetLearned() const
{
	return Learned;
}
