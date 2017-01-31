// Copyright by Keyboard Facerollers

#pragma once

#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class TECHDEMO_API AItem : public AActor
{
	GENERATED_BODY()

protected:
	AItem(const FObjectInitializer& ObjectInitializer);

	void BeginPlay() override;

	UFUNCTION()
		virtual void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly)
		UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere)
		bool bEnableNoise;
	UPROPERTY(EditAnywhere)
		float NoiseDistance;

private:
	bool bNoiseDone = false;
};
