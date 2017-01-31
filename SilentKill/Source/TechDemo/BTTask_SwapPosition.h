// Copyright by Keyboard Facerollers

#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Navigation/PathFollowingComponent.h"
#include "SentryCharacter.h"
#include "BTTask_SwapPosition.generated.h"

/**
 *
 */
UCLASS()
class TECHDEMO_API UBTTask_SwapPosition : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

private:
	UBTTask_SwapPosition(const FObjectInitializer& ObjectInitializer);

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemeory) override;
	EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	FString GetStaticDescription() const override;

	UFUNCTION()
		void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	UBehaviorTreeComponent* MyOwnerComp;
	ASentryCharacter* CurrentCharacter;
	FRotator TargetRotation;
};
