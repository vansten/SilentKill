// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "BTTask_SwapPosition.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SentryAIController.h"


UBTTask_SwapPosition::UBTTask_SwapPosition(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {
	NodeName = "Swap Position";

	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_SwapPosition, BlackboardKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_SwapPosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemeory) {
	auto Result = EBTNodeResult::Failed;
	MyOwnerComp = &OwnerComp;
	const auto MyBlackboard = OwnerComp.GetBlackboardComponent();
	const auto MyController = Cast<ASentryAIController>(OwnerComp.GetAIOwner());

	if (MyBlackboard && MyController) {
		CurrentCharacter = Cast<ASentryCharacter>(MyController->GetPawn());
		if (CurrentCharacter && BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass()) {
			auto KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
			auto TargetActor = Cast<ASentryCharacter>(KeyValue);
			if (TargetActor) {
				FVector TargetLocation;
				TargetActor->GetActorEyesViewPoint(TargetLocation, TargetRotation);

				TargetActor->SetBehaviorType(ESentryBehaviorType::Patrolling);
				CurrentCharacter->SetBehaviorType(ESentryBehaviorType::Observation);

				MyController->MoveToLocation(TargetLocation);
				if(!MyController->ReceiveMoveCompleted.Contains(this, "OnMoveCompleted"))
				{
					MyController->ReceiveMoveCompleted.AddDynamic(this, &UBTTask_SwapPosition::OnMoveCompleted);
				}

				Result = EBTNodeResult::InProgress;
			}
		}
	}

	return Result;
}

EBTNodeResult::Type UBTTask_SwapPosition::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	const auto MyController = OwnerComp.GetAIOwner();
	if (MyController && MyController->GetPathFollowingComponent()) {
		MyController->GetPathFollowingComponent()->AbortMove(*this, FPathFollowingResultFlags::OwnerFinished);
	}

	return EBTNodeResult::Aborted;
}

FString UBTTask_SwapPosition::GetStaticDescription() const {
	return FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), *BlackboardKey.SelectedKeyName.ToString());
}

void UBTTask_SwapPosition::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result) {
	if (MyOwnerComp && MyOwnerComp->GetAIOwner()) {
		FinishLatentTask(*MyOwnerComp, EBTNodeResult::Succeeded);
		CurrentCharacter->SetActorRotation(TargetRotation);
	} else {
		FinishLatentTask(*MyOwnerComp, EBTNodeResult::Failed);
	}
}
