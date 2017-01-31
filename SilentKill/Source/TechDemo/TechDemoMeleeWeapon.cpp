// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "SentryCharacter.h"
#include "TechDemoMeleeWeapon.h"

ATechDemoMeleeWeapon::ATechDemoMeleeWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WeaponCollision = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("Capsule collision"));
	WeaponCollision->SetupAttachment(Mesh);

	WeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	WeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponCollision->bGenerateOverlapEvents = true;
}

void ATechDemoMeleeWeapon::BeginPlay()
{
	WeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &ATechDemoMeleeWeapon::OnMeleeBeginOverlap);
}

void ATechDemoMeleeWeapon::OnMeleeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASentryCharacter* sentry = Cast<ASentryCharacter>(OtherActor);
	if(sentry != nullptr)
	{
		sentry->TakeDamage(100.0f, FDamageEvent(), nullptr, this);
	}
}