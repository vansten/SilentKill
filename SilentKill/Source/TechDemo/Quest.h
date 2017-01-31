// Copyright by Keyboard Facerollers

#pragma once

#include "GameFramework/Actor.h"
#include "Quest.generated.h"

UCLASS()
class TECHDEMO_API AQuest : public AActor
{
	GENERATED_BODY()

protected:
	AQuest();

	UPROPERTY(EditDefaultsOnly)
		USceneComponent* SceneComponent;
	UPROPERTY(EditDefaultsOnly)
		UMaterialBillboardComponent* MaterialBillboardComponent;
	UPROPERTY(EditDefaultsOnly)
		UBoxComponent* TriggerBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Title;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Description;

	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};
