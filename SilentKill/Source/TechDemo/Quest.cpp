// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "Quest.h"
#include "TechDemoCharacter.h"
#include "TechDemoPlayerController.h"


AQuest::AQuest() {
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	MaterialBillboardComponent = CreateDefaultSubobject<UMaterialBillboardComponent>(TEXT("Material"));
	MaterialBillboardComponent->SetupAttachment(RootComponent);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetBoxExtent(FVector(75, 75, 75));
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AQuest::OnBeginOverlap);

	Title = "Quest title";
	Description = "Quest description";
}

void AQuest::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	auto Player = Cast<ATechDemoCharacter>(OtherActor);
	if (Player) {
		auto PlayerController = Cast<ATechDemoPlayerController>(Player->Controller);
		if (PlayerController) {
			PlayerController->DisplayQuest(Title, Description);
			Destroy();
		}
	}
}

