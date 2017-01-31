// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "BTTask_PerformAttack.h"
#include "SentryAIController.h"


UBTTask_PerformAttack::UBTTask_PerformAttack(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {
	NodeName = "Perform Attack";
	bCreateNodeInstance = true;
	bLooping = false;

	TimerDelegate = FTimerDelegate::CreateUObject(this, &UBTTask_PerformAttack::OnAnimationTimerDone);
	PreviousAnimationMode = EAnimationMode::AnimationBlueprint;
}

EBTNodeResult::Type UBTTask_PerformAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemeory) {
	const auto MyController = Cast<ASentryAIController>(OwnerComp.GetAIOwner());
	auto Result = EBTNodeResult::Failed;
	TimerHandle.Invalidate();
	MyOwnerComp = &OwnerComp;

	if (AnimationToPlay && MyController && MyController->GetPawn()) {
		auto Target = Cast<ABaseCharacter>(MyController->GetTargetEnemy());
		if (Target && Target->IsAlive()) {
			USkeletalMeshComponent* SkelMesh = nullptr;
			const auto MyCharacter = Cast<ASentryCharacter>(MyController->GetPawn());
			if (MyCharacter) {
				SkelMesh = MyCharacter->GetMesh();
			} else {
				SkelMesh = MyController->GetPawn()->FindComponentByClass<USkeletalMeshComponent>();
			}

			if (SkelMesh != nullptr) {
				PreviousAnimationMode = SkelMesh->GetAnimationMode();
				CachedSkelMesh = SkelMesh;

				MyCharacter->SetCanDealDamage(true);
				SkelMesh->PlayAnimation(AnimationToPlay, bLooping);
				const auto FinishDelay = AnimationToPlay->GetMaxCurrentTime();

				if (FinishDelay > 0) {
					if (bLooping == false) {
						MyController->GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, FinishDelay, false);
					}
					Result = EBTNodeResult::InProgress;
				} else {
					MyCharacter->SetCanDealDamage(false);
					Result = EBTNodeResult::Succeeded;
				}
			}
		}
	}

	return Result;
}

EBTNodeResult::Type UBTTask_PerformAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	const auto MyController = OwnerComp.GetAIOwner();

	if (AnimationToPlay && MyController && TimerHandle.IsValid()) {
		MyController->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}

	TimerHandle.Invalidate();
	CleanUp(OwnerComp);

	return EBTNodeResult::Aborted;
}

FString UBTTask_PerformAttack::GetStaticDescription() const {
	return FString::Printf(TEXT("%s: '%s'"), *Super::GetStaticDescription(), *GetNameSafe(AnimationToPlay)
		, bLooping ? TEXT(", looping") : TEXT(""));
}

void UBTTask_PerformAttack::OnAnimationTimerDone() {
	if (MyOwnerComp) {
		auto Pawn = Cast<ASentryCharacter>(MyOwnerComp->GetAIOwner()->GetPawn());
		Pawn->SetCanDealDamage(false);
		CleanUp(*MyOwnerComp);
		FinishLatentTask(*MyOwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTTask_PerformAttack::CleanUp(UBehaviorTreeComponent& OwnerComp) const {
	if (CachedSkelMesh != nullptr && PreviousAnimationMode == EAnimationMode::AnimationBlueprint) {
		CachedSkelMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}
}
