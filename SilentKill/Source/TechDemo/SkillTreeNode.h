// Copyright by Keyboard Facerollers

#pragma once

#include "Components/DecalComponent.h"
#include "SkillTreeNodeConnector.h"
#include "SkillTreeNode.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class TECHDEMO_API USkillTreeNode : public UDecalComponent
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	bool Learned;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	bool Selected;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	FLinearColor NodeStateVector;
	class USkillTreeNode *Left;
	class USkillTreeNode *Right;
	class USkillTreeNode *Up;
	class USkillTreeNode *Down;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	int Cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	UAnimationAsset *SkillAnimation;
	TArray<USkillTreeNodeConnector*> NodeConnectors;
	/* 1,0,0,0 selected = false/learned = false
	* 0,1,0,0 selected = false/learned = true
	* 0,0,1,0 selected = true/learned = false
	* 0,0,0,1 selected = true/learned = true */
	UMaterialInstanceDynamic *SkillMaterial;
public:
	USkillTreeNode();
	virtual void BeginPlay() override;
	void CalculateState();
	void ChangeSelection(bool selected);
	void ChangeLearned(bool learned = true);
	int GetCost() const;
	bool GetLearned() const;
	void CalculateConnectors();
};
