// Copyright by Keyboard Facerollers

#pragma once

#include "AIController.h"
#include "SentryCharacter.h"
#include "SentryAIController.generated.h"

/**
 *
 */
UCLASS()
class TECHDEMO_API ASentryAIController : public AAIController
{
	GENERATED_BODY()

private:
	ASentryAIController(const FObjectInitializer& ObjectInitializer);

	void Possess(APawn* InPawn) override;
	void UnPossess() override;

	UBehaviorTreeComponent* BehaviorTreeComponent;
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		FName TargetEnemyName;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		FName EnemyLocationName;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		FName BehaviorTypeName;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		FName NeedBreakName;

public:
	AActor* GetTargetEnemy() const;
	void SetTargetEnemy(AActor* Target) const;
	void SetTargetLocation(FVector Location) const;
	void SetBehaviorType(ESentryBehaviorType BehaviorType) const;
	void SetNeedBreak(bool value) const;
};
