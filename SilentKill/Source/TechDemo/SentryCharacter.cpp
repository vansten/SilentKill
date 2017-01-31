// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "SentryCharacter.h"
#include "SentryAIController.h"
#include "TechDemoCharacter.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Item.h"
#include "TechDemoArrow.h"

ASentryCharacter::ASentryCharacter(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {
	AIControllerClass = ASentryAIController::StaticClass();
	BehaviorType = ESentryBehaviorType::Patrolling;
	TimeForBreak = 12.f;
	SenseTimeOut = 3.f;
	bCanDealDamage = false;

	FAISenseAffiliationFilter SenseAffiliationFilter;
	SenseAffiliationFilter.bDetectEnemies = true;
	SenseAffiliationFilter.bDetectFriendlies = true;
	SenseAffiliationFilter.bDetectNeutrals = true;

	PerceptionComponent = ObjectInitializer.CreateDefaultSubobject<UAIPerceptionComponent>(this, TEXT("PerceptionComponent"));

	SightConfig = ObjectInitializer.CreateDefaultSubobject<UAISenseConfig_Sight>(this, TEXT("SightConfig"));
	SightConfig->SightRadius = 2500.f;
	SightConfig->LoseSightRadius = 2600.f;
	SightConfig->PeripheralVisionAngleDegrees = 65.f;
	SightConfig->SetMaxAge(SenseTimeOut);
	SightConfig->DetectionByAffiliation = SenseAffiliationFilter;
	PerceptionComponent->ConfigureSense(*SightConfig);

	HearingConfig = ObjectInitializer.CreateDefaultSubobject<UAISenseConfig_Hearing>(this, TEXT("HearingConfig"));
	HearingConfig->HearingRange = 3000;
	HearingConfig->LoSHearingRange = 3500;
	HearingConfig->DetectionByAffiliation = SenseAffiliationFilter;
	HearingConfig->SetMaxAge(SenseTimeOut);
	PerceptionComponent->ConfigureSense(*HearingConfig);

	GetCharacterMovement()->MaxWalkSpeed = 250.f;

	Weapon = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("WeaponMesh"));
	Weapon->SetCollisionProfileName(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), FName("WeaponSocket"));
	Weapon->SetCanEverAffectNavigation(false);

	Shield = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("ShieldMesh"));
	Shield->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
	Shield->SetupAttachment(GetMesh(), FName("ShieldSocket"));
	Shield->SetCanEverAffectNavigation(false);
}

void ASentryCharacter::BeginPlay() {
	Super::BeginPlay();

	if (PerceptionComponent) {
		PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ASentryCharacter::OnPerceptionUpdated);
	}
	if (Weapon) {
		Weapon->OnComponentBeginOverlap.AddDynamic(this, &ASentryCharacter::WeaponOnComponentBeginOverlap);
	}
	if (BehaviorType == ESentryBehaviorType::Patrolling) {
		GetWorldTimerManager().SetTimer(BreakTimerHandle, this, &ASentryCharacter::NeedBreak, TimeForBreak);
	}
	AIController = Cast<ASentryAIController>(GetController());
}

void ASentryCharacter::Die() {
	Super::Die();

	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Shield->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASentryCharacter::NeedBreak() const {
	AIController->SetNeedBreak(true);
}

void ASentryCharacter::OnPerceptionUpdated(TArray<AActor*> UpdatedActors) {
	if (!AIController || !AIController->IsValidLowLevel()) {
		return;
	}

	auto bHasAnyTarget = false;
	for (auto Actor : UpdatedActors) {
		auto Pawn = Cast<ATechDemoCharacter>(Actor);
		if(Pawn && Pawn->IsAlive())
		{
			auto PerceptionInfo = PerceptionComponent->GetActorInfo(*Pawn);
			if(PerceptionInfo->HasAnyCurrentStimulus())
			{
				AIController->SetTargetEnemy(Pawn);
				AIController->SetTargetLocation(PerceptionInfo->GetLastStimulusLocation());
				bHasAnyTarget = true;
				break;
			}
		}

		auto Arrow = Cast<ATechDemoArrow>(Actor);
		if (Arrow) {
			AIController->SetTargetEnemy(Arrow);
			auto PerceptionInfo = PerceptionComponent->GetActorInfo(*Arrow);
			if (PerceptionInfo->HasAnyCurrentStimulus()) {
				AIController->SetTargetLocation(PerceptionInfo->GetLastStimulusLocation());
				bHasAnyTarget = true;
			}
		}

		
	}
	if (!bHasAnyTarget) {
		AIController->SetTargetEnemy(nullptr);
	}
	bSensedTarget = bHasAnyTarget;
}

void ASentryCharacter::WeaponOnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (bCanDealDamage) {
		auto Player = Cast<ATechDemoCharacter>(OtherActor);
		if (Player && Player->IsAlive()) {
			FPointDamageEvent PointDamage;
			PointDamage.Damage = 200.f;
			OtherActor->TakeDamage(PointDamage.Damage, PointDamage, GetController(), this);
		}
	}
}

void ASentryCharacter::SetBehaviorType(ESentryBehaviorType Type) {
	BehaviorType = Type;
	if (BehaviorType == ESentryBehaviorType::Patrolling) {
		BreakTimerHandle.Invalidate();
		GetWorldTimerManager().SetTimer(BreakTimerHandle, this, &ASentryCharacter::NeedBreak, TimeForBreak);
	}

	if (AIController) {
		AIController->SetBehaviorType(Type);
	}
}

void ASentryCharacter::SetCanDealDamage(bool value) {
	bCanDealDamage = value;
}
