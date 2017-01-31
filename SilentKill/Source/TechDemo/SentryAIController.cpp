// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "SentryAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdFollowingComponent.h"


ASentryAIController::ASentryAIController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent"))) {
	BlackboardComponent = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComponent"));
	BehaviorTreeComponent = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorTreeComponent"));

	TargetEnemyName = "TargetEnemy";
	EnemyLocationName = "EnemyLocation";
	BehaviorTypeName = "BehaviorType";
	NeedBreakName = "NeedBreak";
}

void ASentryAIController::Possess(APawn* InPawn) {
	Super::Possess(InPawn);

	auto Sentry = Cast<ASentryCharacter>(InPawn);
	if (Sentry) {
		if (Sentry->BehaviorTree->BlackboardAsset) {
			BlackboardComponent->InitializeBlackboard(*Sentry->BehaviorTree->BlackboardAsset);
			SetBehaviorType(Sentry->BehaviorType);
		}
		BehaviorTreeComponent->StartTree(*Sentry->BehaviorTree);
	}
}

void ASentryAIController::UnPossess() {
	Super::UnPossess();

	BehaviorTreeComponent->StopTree();
}

AActor* ASentryAIController::GetTargetEnemy() const {
	return Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetEnemyName));
}

void ASentryAIController::SetTargetEnemy(AActor* Target) const {
	if(IsValidLowLevelFast() && BlackboardComponent->IsValidLowLevelFast())
	{
		BlackboardComponent->SetValueAsObject(TargetEnemyName, Target);
	}
}

void ASentryAIController::SetTargetLocation(FVector Location) const {
	if(IsValidLowLevelFast() && BlackboardComponent->IsValidLowLevelFast())
	{
		BlackboardComponent->SetValueAsVector(EnemyLocationName, Location);
	}
}

void ASentryAIController::SetBehaviorType(ESentryBehaviorType BehaviorType) const {
	BlackboardComponent->SetValueAsEnum(BehaviorTypeName, uint8(BehaviorType));
	if (BehaviorType == ESentryBehaviorType::Patrolling) {
		SetNeedBreak(false);
	}
}

void ASentryAIController::SetNeedBreak(bool value) const {
	if(IsValidLowLevelFast() && BlackboardComponent->IsValidLowLevelFast())
	{
		BlackboardComponent->SetValueAsBool(NeedBreakName, value);
	}
}
