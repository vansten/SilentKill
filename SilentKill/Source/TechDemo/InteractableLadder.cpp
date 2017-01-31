// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "InteractableLadder.h"
#include "TechDemoCharacter.h"


AInteractableLadder::AInteractableLadder(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {
	bInteracted = false;

	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	Mesh->SetupAttachment(TriggerBox);
}

void AInteractableLadder::Interact(ATechDemoCharacter* Player) {
	bInteracted = !bInteracted;
	auto Rotation = FRotator(0, GetActorRotation().Yaw + 180.f, 0);
	Player->UseLadder(Rotation);
	Player->SetActorRotation(Rotation);
}

void AInteractableLadder::OnExit(ATechDemoCharacter* Player) {
	if (bInteracted) {
		bInteracted = false;
		Player->UseLadder(FRotator());
	}
}
