// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "BaseCharacter.h"
#include "Perception/AISense_Hearing.h"

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {
	Health = 100;
	bIsDying = false;

	GetMesh()->SetRelativeLocation(FVector(0, 0, -97));
	GetMesh()->RelativeRotation.Yaw = -90.f;
	GetMesh()->bGenerateOverlapEvents = true;
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));

	StunParticlesTransform = CreateDefaultSubobject<USceneComponent>(TEXT("Stun particles transform"));
	StunParticlesTransform->SetupAttachment(RootComponent);
	StunParticlesTransform->RelativeLocation = FVector(0.0f, 90.0f, 0.0f);
}

float ABaseCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	if (Health <= 0.f) {
		return 0.f;
	}

	const auto ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f) {
		Health -= ActualDamage;
		if (Health <= 0.f) {
			LogOnScreen(-1, 5.0f, FColor::Red, GetName().Append(": I'm dead :( :< :( :<"));
			Die();
		}
	}
	return ActualDamage;
}

float ABaseCharacter::GetHealth() const {
	return Health / GetMaxHealth();
}

void ABaseCharacter::StunCharacter(float Duration) {
	LogOnScreen(-1, 3.0f, FColor::Red, FString("Stun start"));

	UWorld* world = GetWorld();
	if(world)
	{
		if(!SleepParticlesComponent)
		{
			SleepParticlesComponent = UGameplayStatics::SpawnEmitterAtLocation(world, StunParticles, StunParticlesTransform->GetComponentLocation(), StunParticlesTransform->GetComponentRotation(), true);
		}
		else if(!SleepParticlesComponent->IsActive())
		{
			SleepParticlesComponent->Activate(true);
		}
	}

	GetCharacterMovement()->DisableMovement();
	GetWorldTimerManager().SetTimer(StunTimerHandle, this, &ABaseCharacter::OnStunEnd, Duration);
}

void ABaseCharacter::OnStunEnd() {
	if(SleepParticlesComponent && SleepParticlesComponent->IsActive())
	{
		SleepParticlesComponent->Deactivate();
	}
	GetCharacterMovement()->MovementMode = MOVE_Walking;
	LogOnScreen(-1, 3.0f, FColor::Red, FString("Stun end"));
}

bool ABaseCharacter::CanDie() const {
	return !bIsDying && !IsPendingKill();
}

void ABaseCharacter::Die() {
	if (!CanDie()) {
		return;
	}

	bIsDying = true;
	Health = FMath::Min(0.f, Health);

	DetachFromControllerPendingDestroy();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	if (GetMesh()) {
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	}
	auto MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (MovementComponent) {
		MovementComponent->DisableMovement();
		MovementComponent->StopMovementImmediately();
		MovementComponent->SetComponentTickEnabled(false);
	}

	UAISense_Hearing::ReportNoiseEvent(this, GetActorLocation(), 1.5f, this, 2000.0f);
}

float ABaseCharacter::GetMaxHealth() const {
	return GetClass()->GetDefaultObject<ABaseCharacter>()->Health;
}
