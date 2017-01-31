// Copyright by Keyboard Facerollers

#pragma once

#include "TechDemoArrow.h"
#include "TechDemoArrowExploding.generated.h"

/**
 * 
 */
UCLASS()
class TECHDEMO_API ATechDemoArrowExploding : public ATechDemoArrow
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Particles)
	UParticleSystem* ExplosionParticles;

	UPROPERTY(EditAnywhere, Category = Explosion)
	USphereComponent* ExplosionSphere;

	UPROPERTY(EditAnywhere, Category = Audio)
	USoundCue* ExplosionSound;

	UPROPERTY(EditAnywhere, Category = Explosion)
	float Radius;

	UPROPERTY(EditAnywhere, Category = Debug)
	bool bDrawExplosionSphere;

protected:
	virtual void OnDestroy() override;
};
