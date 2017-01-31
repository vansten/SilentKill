// Copyright by Keyboard Facerollers

#pragma once

#include "TechDemoArrow.h"
#include "TechDemoArrowSleepy.generated.h"

/**
 * 
 */
UCLASS()
class TECHDEMO_API ATechDemoArrowSleepy : public ATechDemoArrow
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Sleep)
	float SleepDuration;

protected:
	virtual void EnemyHit(class ASentryCharacter* Enemy) override;
};
