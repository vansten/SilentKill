// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "TechDemoArrowSplatering.h"

ATechDemoArrowSplatering::ATechDemoArrowSplatering(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WeaponType = EWeaponType::Bow_Splatering;
	bWasHit = false;
}

void ATechDemoArrowSplatering::OnHit(const FVector& reflectedDirection)
{
	FVector arrowVelo = reflectedDirection;

	UWorld* world = GetWorld();
	FActorSpawnParameters spawnParameters;
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FRotator rotateArrowVelo = FRotator();
	for(int i = 0; i < ArrowsAfterHitNum; ++i)
	{
		rotateArrowVelo.Yaw = FMath::RandRange(-15.0f, 15.0f);
		rotateArrowVelo.Roll = FMath::RandRange(-15.0f, 15.0f);
		arrowVelo = rotateArrowVelo.RotateVector(reflectedDirection);
		ATechDemoArrow* arrow = world->SpawnActor<ATechDemoArrow>(ArrowsAfterHitClass, GetActorLocation(), arrowVelo.Rotation(), spawnParameters);
		if(arrow)
		{
			arrow->Shoot(arrowVelo);
		}
	}

	bWasHit = true;
}

void ATechDemoArrowSplatering::OnDestroy()
{
	if(bWasHit)
	{
		AudioComponent->Play();
		Vanish();
	}
	else
	{
		Super::OnDestroy();
	}
}