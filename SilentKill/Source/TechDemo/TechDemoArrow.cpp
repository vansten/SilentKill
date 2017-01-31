// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "TechDemoArrow.h"
#include "ArrowPluginMeshActor.h"
#include "SentryCharacter.h"
#include "Perception/AISense_Hearing.h"

FName ATechDemoArrow::ArrowheadSocketName = FName("Arrowhead");

ATechDemoArrow::ATechDemoArrow(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Mesh->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f);
	ArrowheadCollision = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("ArrowheadCollision"));
	ArrowheadCollision->SetupAttachment(Mesh, ArrowheadSocketName);

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ArrowheadCollision->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	ArrowheadCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	ArrowheadCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ArrowheadCollision->bGenerateOverlapEvents = true;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	AudioComponent->SetupAttachment(Mesh);

	WeaponType = EWeaponType::Bow_Normal;

	PrimaryActorTick.bCanEverTick = true;
}

void ATechDemoArrow::OnArrowheadBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!bWasShot)
	{
		return;
	}

	AArrowPluginMeshActor* meshActor = Cast<AArrowPluginMeshActor>(OtherActor);
	if(meshActor)
	{
		--MaxHits;
		if(meshActor->Reflection > 0.0f)
		{
			FCollisionQueryParams params = FCollisionQueryParams::DefaultQueryParam;
			params.AddIgnoredActor(this);
			params.AddIgnoredComponent(Mesh);
			params.AddIgnoredComponent(ArrowheadCollision);
			FHitResult hitInfo;
			if(GetWorld()->LineTraceSingleByChannel(hitInfo, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 1000.0f, ECollisionChannel::ECC_MAX, params))
			{
				SetActorLocation(hitInfo.Location);
				FVector reflectionVector = FMath::GetReflectionVector(CurrentVelocity.GetSafeNormal(), hitInfo.ImpactNormal).GetSafeNormal();
				if(FVector::DotProduct(reflectionVector, hitInfo.ImpactNormal) > 0.99f)
				{
					MaxHits = 0;
				}
				CurrentVelocity = reflectionVector.GetSafeNormal() * CurrentVelocity.Size() * meshActor->Reflection;
				OnHit(CurrentVelocity);
			}
		}

		if(MaxHits > 0)
		{
			return;
		}
	}

	bWasShot = false;
	FVector loc = GetActorLocation();
	loc += GetActorForwardVector() * 20.0f;
	SetActorLocation(loc);
	CurrentVelocity = FVector::ZeroVector;
	ArrowheadCollision->bGenerateOverlapEvents = false;
	OnDestroy();

	ASentryCharacter* Sentry = Cast<ASentryCharacter>(OtherActor);
	if(Sentry)
	{
		EnemyHit(Sentry);
	}
}

void ATechDemoArrow::EnemyHit(ASentryCharacter* Enemy)
{
	Enemy->TakeDamage(100.0f, FDamageEvent(), nullptr, this);
}

void ATechDemoArrow::OnHit(const FVector& reflectedDirection)
{
	AudioComponent->Play();
	UAISense_Hearing::ReportNoiseEvent(this, GetActorLocation(), 1.f, this, 1500.0f);
}

void ATechDemoArrow::OnDestroy()
{
	AudioComponent->Play();
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &ATechDemoArrow::Vanish, 1.5f);
}

void ATechDemoArrow::Vanish()
{
	Destroy();
}

void ATechDemoArrow::BeginPlay()
{
	Super::BeginPlay();
	bWasShot = false;
	ArrowheadCollision->OnComponentBeginOverlap.AddDynamic(this, &ATechDemoArrow::OnArrowheadBeginOverlap);
	CurrentVelocity = FVector::ZeroVector;
	AudioComponent->SetSound(HitSound);
}

void ATechDemoArrow::Tick(float DeltaSeconds)
{
	if(bWasShot)
	{
		SetActorRotation(CurrentVelocity.GetSafeNormal().Rotation());
		FVector actorLoc = GetActorLocation();
		actorLoc += CurrentVelocity * DeltaSeconds;
		SetActorLocation(actorLoc, true);

		CurrentVelocity += FVector(0, 0, GetWorld()->GetGravityZ()) * DeltaSeconds * DeltaSeconds;
	}
}