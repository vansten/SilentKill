// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "InteractableTrapdoor.h"


AInteractableTrapdoor::AInteractableTrapdoor(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {
	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	Mesh->SetupAttachment(TriggerBox);

	bOpen = false;
	OpenTime = 1.f;
}

void AInteractableTrapdoor::BeginPlay() {
	CloseRotation = Mesh->RelativeRotation;
	OpenRotation = Mesh->RelativeRotation + FRotator(0, 0, 90);
}

void AInteractableTrapdoor::Interact(ATechDemoCharacter* Player) {
	bOpen = !bOpen;
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	auto TargetRelativeRotation = bOpen ? OpenRotation : CloseRotation;

	UKismetSystemLibrary::MoveComponentTo(Mesh, Mesh->RelativeLocation, TargetRelativeRotation, false, false, OpenTime, true, EMoveComponentAction::Move, LatentInfo);
}
