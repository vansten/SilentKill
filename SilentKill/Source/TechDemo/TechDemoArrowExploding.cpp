// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "TechDemoArrowExploding.h"
#include "SentryCharacter.h"
#include "Perception/AISense_Hearing.h"

ATechDemoArrowExploding::ATechDemoArrowExploding(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WeaponType = EWeaponType::Bow_Exploding;
	ExplosionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Explosion sphere"));
	ExplosionSphere->SetupAttachment(RootComponent);
}

void ATechDemoArrowExploding::OnDestroy()
{
	UWorld* world = GetWorld();
	if(!world)
	{
		return;
	}

	UGameplayStatics::SpawnEmitterAtLocation(world, ExplosionParticles, ExplosionSphere->GetComponentLocation(), FRotator::ZeroRotator, true);

	if(bDrawExplosionSphere)
	{
		DrawDebugSphere(world, ExplosionSphere->GetComponentLocation(), Radius, FMath::Pow(2.0f, (Radius * 0.01f) + 1), FColor::Yellow, true, 10.0f);
	}

	ExplosionSphere->SetSphereRadius(Radius);
	UAISense_Hearing::ReportNoiseEvent(this, ExplosionSphere->GetComponentLocation(), 10.0f, this, Radius * 5.0f);

	//AudioComponent->Stop();
	UGameplayStatics::SpawnSoundAtLocation(this, ExplosionSound, ExplosionSphere->GetComponentLocation());
	//AudioComponent->Play();
	//AudioComponent->SetSound(ExplosionSound);
	
	TArray<AActor*> overlappingActors;
	ExplosionSphere->GetOverlappingActors(overlappingActors);

	int overlappingActorsNum = overlappingActors.Num();
	for(int i = 0; i < overlappingActorsNum; ++i)
	{
		ASentryCharacter* sentry = Cast<ASentryCharacter>(overlappingActors[i]);
		if(sentry)
		{
			EnemyHit(sentry);
		}
	}

	Vanish();
}