// Copyright by Keyboard Facerollers

#pragma once

#include "GameFramework/Actor.h"
#include "Feather.h"
#include "SkillTreeNode.h"
#include "SkillTree.generated.h"

UCLASS(Blueprintable)
class TECHDEMO_API ASkillTree : public AActor
{
	GENERATED_BODY()

	enum EDirection
	{
		Left = 0,
		Up,
		Right,
		Down
	};

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	class USkillTreeNode *Parent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	class USkillTreeNode *CurrentSelected;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	TArray<USkillTreeNode*> AllSkills;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	int SkillPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	UFeather* Feather;

	UAnimInstance* AnimationInstance;
	USkeletalMeshComponent *SkeletalMesh;
	bool VBlock;
	bool HBlock;

public:	
	// Sets default values for this actor's properties
	ASkillTree();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	void OnUse();
	void ChangeSelection(EDirection);
	void OnHorizontalMove(float x);
	void OnVerticalMove(float x);
	void OnShow();


};
