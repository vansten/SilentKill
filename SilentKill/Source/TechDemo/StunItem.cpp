// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "StunItem.h"
#include "BaseCharacter.h"


AStunItem::AStunItem(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {
	StunDuration = 3.f;
	NoiseDistance = 1000.f;
}

void AStunItem::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	Super::OnComponentHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

	auto PhysicalMaterial = Hit.PhysMaterial.Get();
	if (PhysicalMaterial && PhysicalMaterial->SurfaceType == SURFACE_HEAD) {
		auto Character = Cast<ABaseCharacter>(OtherActor);
		Character->StunCharacter(StunDuration);
	}
}
