// Copyright by Keyboard Facerollers

#pragma once

#include "Item.h"
#include "StunItem.generated.h"

UCLASS()
class TECHDEMO_API AStunItem : public AItem
{
	GENERATED_BODY()

protected:
	AStunItem(const FObjectInitializer& ObjectInitializer);

	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	UPROPERTY(EditAnywhere)
		float StunDuration;

};
