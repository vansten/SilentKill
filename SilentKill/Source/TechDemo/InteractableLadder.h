// Copyright by Keyboard Facerollers

#pragma once

#include "Interactable.h"
#include "InteractableLadder.generated.h"

/**
 *
 */
UCLASS()
class TECHDEMO_API AInteractableLadder : public AInteractable
{
	GENERATED_BODY()

private:
	AInteractableLadder(const FObjectInitializer& ObjectInitializer);

	void Interact(ATechDemoCharacter* Player) override;
	void OnExit(ATechDemoCharacter* Player) override;

	UPROPERTY(EditDefaultsOnly)
		UStaticMeshComponent* Mesh;

	bool bInteracted;
};
