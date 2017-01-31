// Copyright by Keyboard Facerollers

#pragma once

#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class TECHDEMO_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Stun)
		UParticleSystem* StunParticles;

	UPROPERTY(EditAnywhere, Category = Stun)
		USceneComponent* StunParticlesTransform;

protected:
	ABaseCharacter(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
		float Health;

	float GetMaxHealth() const;
	virtual void OnStunEnd();
	virtual bool CanDie() const;
	virtual void Die();

	bool bIsDying;
	FTimerHandle StunTimerHandle;
	UParticleSystemComponent* SleepParticlesComponent;

public:
	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		bool IsAlive() const { return Health > 0; }

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void StunCharacter(float Duration);

};
