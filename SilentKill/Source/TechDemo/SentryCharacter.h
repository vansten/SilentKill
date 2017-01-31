// Copyright by Keyboard Facerollers

#pragma once

#include "BaseCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "SentryCharacter.generated.h"

UENUM()
enum class ESentryBehaviorType :uint8
{
	Observation,
	Patrolling
};

UCLASS()
class TECHDEMO_API ASentryCharacter : public ABaseCharacter
{
	GENERATED_BODY()

private:
	ASentryCharacter(const FObjectInitializer& ObjectInitializer);

	void BeginPlay() override;
	void Die() override;
	void NeedBreak() const;

	UPROPERTY(EditAnywhere, Category = "AI")
		float TimeForBreak;
	UPROPERTY(VisibleAnywhere, Category = "AI")
		UAIPerceptionComponent* PerceptionComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		UStaticMeshComponent* Weapon;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		UStaticMeshComponent* Shield;

	UFUNCTION()
		void OnPerceptionUpdated(TArray<AActor*> UpdatedActors);
	UFUNCTION()
		void WeaponOnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	class UAISenseConfig_Sight* SightConfig;
	class UAISenseConfig_Hearing* HearingConfig;
	class ASentryAIController* AIController;
	bool bCanDealDamage, bSensedTarget;
	float SenseTimeOut;
	FTimerHandle BreakTimerHandle;

public:
	UPROPERTY(EditAnywhere, Category = "AI")
		ESentryBehaviorType BehaviorType;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		UBehaviorTree* BehaviorTree;

	UFUNCTION(BlueprintCallable, Category = "Attacking")
		bool HasTarget() const { return bSensedTarget; }

	void SetBehaviorType(ESentryBehaviorType Type);
	void SetCanDealDamage(bool value);
	FORCEINLINE ASentryAIController* GetAIController() const { return AIController; }

};
