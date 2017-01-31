// Copyright by Keyboard Facerollers

#pragma once

#include "Interactable.h"
#include "InteractableTrapdoor.generated.h"

/**
 *
 */
UCLASS()
class TECHDEMO_API AInteractableTrapdoor : public AInteractable
{
	GENERATED_BODY()

private:
	AInteractableTrapdoor(const FObjectInitializer& ObjectInitializer);

	void BeginPlay() override;
	void Interact(ATechDemoCharacter* Player) override;

	UPROPERTY(EditDefaultsOnly)
		UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere)
		float OpenTime;

	bool bOpen;
	FRotator OpenRotation, CloseRotation;
};
