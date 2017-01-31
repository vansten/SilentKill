// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "TechDemoArrowSleepy.h"
#include "SentryCharacter.h"

ATechDemoArrowSleepy::ATechDemoArrowSleepy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WeaponType = EWeaponType::Bow_Sleepy;
}

void ATechDemoArrowSleepy::EnemyHit(ASentryCharacter* Enemy)
{
	Enemy->StunCharacter(SleepDuration);
}