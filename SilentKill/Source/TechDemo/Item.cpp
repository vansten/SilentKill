// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "Item.h"
#include "Perception/AISense_Hearing.h"


AItem::AItem(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {
	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	Mesh->SetCollisionProfileName(TEXT("Item"));
	Mesh->SetSimulatePhysics(true);
	Mesh->SetNotifyRigidBodyCollision(true);
	Mesh->WakeRigidBody();
	RootComponent = Mesh;

	bEnableNoise = true;
	NoiseDistance = 2000.f;
}

void AItem::BeginPlay() {
	Super::BeginPlay();

	UAIPerceptionSystem::RegisterPerceptionStimuliSource(this, UAISense_Hearing::StaticClass(), this);
	if (Mesh) {
		Mesh->OnComponentHit.AddDynamic(this, &AItem::OnComponentHit);
	}
}

void AItem::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	if (bEnableNoise && !bNoiseDone) {
		bNoiseDone = true;
		UAISense_Hearing::ReportNoiseEvent(this, GetActorLocation(), 1.f, this, NoiseDistance);
	}
}
