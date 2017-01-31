// Copyright by Keyboard Facerollers

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PerformAttack.generated.h"

/**
 *
 */
UCLASS()
class TECHDEMO_API UBTTask_PerformAttack : public UBTTaskNode
{
	GENERATED_BODY()

private:
	UBTTask_PerformAttack(const FObjectInitializer& ObjectInitializer);

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemeory) override;
	EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	FString GetStaticDescription() const override;
	void OnAnimationTimerDone();
	void CleanUp(UBehaviorTreeComponent& OwnerComp) const;

	/** Animation asset to play. Note that it needs to match the skeleton of pawn this BT is controlling */
	UPROPERTY(Category = Node, EditAnywhere)
		UAnimationAsset* AnimationToPlay;
	UPROPERTY(Category = Node, EditAnywhere)
		bool bLooping;
	UPROPERTY()
		UBehaviorTreeComponent* MyOwnerComp;
	UPROPERTY()
		USkeletalMeshComponent* CachedSkelMesh;

	EAnimationMode::Type PreviousAnimationMode;
	FTimerDelegate TimerDelegate;
	FTimerHandle TimerHandle;
};
