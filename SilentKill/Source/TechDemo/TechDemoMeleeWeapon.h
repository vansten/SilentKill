// Copyright by Keyboard Facerollers

#pragma once

#include "TechDemoWeapon.h"
#include "TechDemoMeleeWeapon.generated.h"

/**
 * 
 */
UCLASS()
class TECHDEMO_API ATechDemoMeleeWeapon : public ATechDemoWeapon
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Collision)
	UCapsuleComponent* WeaponCollision;
	
protected:
	UFUNCTION()
	void OnMeleeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
