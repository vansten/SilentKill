// Copyright by Keyboard Facerollers

#pragma once

#include "TechDemoWeapon.h"
#include "TechDemoArrow.generated.h"

UCLASS()
class TECHDEMO_API ATechDemoArrow : public ATechDemoWeapon
{
	GENERATED_UCLASS_BODY()

protected:
	FVector CurrentVelocity;
	FTimerHandle DestroyTimerHandle;
	bool bWasShot;

public:
	static FName ArrowheadSocketName;

	UPROPERTY(EditAnywhere, Category = Collision)
	USphereComponent* ArrowheadCollision;
	UPROPERTY(EditAnywhere, Category = Audio)
	UAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere, Category = Audio)
	USoundCue* HitSound;
	UPROPERTY(EditAnywhere, Category = Collision)
	int32 MaxHits;

protected:
	UFUNCTION()
	void OnArrowheadBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void EnemyHit(class ASentryCharacter* Enemy);
	virtual void OnHit(const FVector& reflectedDirection);
	virtual void OnDestroy();
	void Vanish();

public:	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	//void Shoot(float ChordStrain, float InitialSpeed);
	FORCEINLINE void Shoot(const FVector& newVelocity)
	{
		CurrentVelocity = newVelocity;
		bWasShot = true;
	}
};
