// Copyright by Keyboard Facerollers

#pragma once

#include "Components/DecalComponent.h"
#include "SkillTreeNodeConnector.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class TECHDEMO_API USkillTreeNodeConnector : public UDecalComponent
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	FLinearColor NodeStateVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	bool Learned;
	UMaterialInstanceDynamic *SkillMaterial;
public:
	virtual void BeginPlay() override;
	void CalculateState();
	void ChangeLearned(bool learned = true);
	bool GetLearned() const;
};
