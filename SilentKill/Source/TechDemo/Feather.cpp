// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "Feather.h"
#include <algorithm>


void UFeather::BeginPlay()
{
	UMaterialInstanceDynamic* tmp = UMaterialInstanceDynamic::Create(this->GetMaterial(1), this);
	SkillPoints.Add(tmp);
	SetMaterial(1, tmp);

	tmp = UMaterialInstanceDynamic::Create(this->GetMaterial(5), this);
	SkillPoints.Add(tmp);
	SetMaterial(5, tmp);

	tmp = UMaterialInstanceDynamic::Create(this->GetMaterial(4), this);
	SkillPoints.Add(tmp);
	SetMaterial(4, tmp);

	tmp = UMaterialInstanceDynamic::Create(this->GetMaterial(2), this);
	SkillPoints.Add(tmp);
	SetMaterial(2, tmp);

	tmp = UMaterialInstanceDynamic::Create(this->GetMaterial(9), this);
	SkillPoints.Add(tmp);
	SetMaterial(9, tmp);

	tmp = UMaterialInstanceDynamic::Create(this->GetMaterial(8), this);
	SkillPoints.Add(tmp);
	SetMaterial(8, tmp);

	tmp = UMaterialInstanceDynamic::Create(this->GetMaterial(6), this);
	SkillPoints.Add(tmp);
	SetMaterial(6, tmp);

	tmp = UMaterialInstanceDynamic::Create(this->GetMaterial(7), this);
	SkillPoints.Add(tmp);
	SetMaterial(7, tmp);

	tmp = UMaterialInstanceDynamic::Create(this->GetMaterial(3), this);
	SkillPoints.Add(tmp);
	SetMaterial(3, tmp);

	tmp = UMaterialInstanceDynamic::Create(this->GetMaterial(11), this);
	SkillPoints.Add(tmp);
	SetMaterial(11, tmp);

	tmp = UMaterialInstanceDynamic::Create(this->GetMaterial(0), this);
	SkillPoints.Add(tmp);
	SetMaterial(0, tmp);
}

void UFeather::ShowAvailable(int skillPoints)
{
	if (skillPoints > 0)
	{
		skillPoints = std::min(11, skillPoints);
		for (int i = 0; i < skillPoints; ++i)
		{
			SkillPoints[i]->SetVectorParameterValue(TEXT("State"), FLinearColor(0.f, 1.f, 0.f, 1.f));
		}
	}
	else
	{		
		ResetAll();
	}
}

void UFeather::NotLearnedSelected(int skillPoints, int cost)
{
	int diff = skillPoints - cost;
	if(diff >= 0)
	{
		skillPoints = std::min(9, skillPoints - 1);
		diff = skillPoints - cost;
		for (int i = skillPoints; i > diff; --i)
		{
			SkillPoints[i]->SetVectorParameterValue(TEXT("State"), FLinearColor(0.f, 1.f, 1.f, 1.f));
		}
	}
}

void UFeather::ResetAll()
{
	int size = SkillPoints.Num();
	for(int i = 0; i < size; ++i)
	{
		SkillPoints[i]->SetVectorParameterValue(TEXT("State"), FLinearColor(1.f, 0.f, 0.f, 1.f));
	}
}
