// Copyright by Keyboard Facerollers

#pragma once

#include "GameFramework/Actor.h"
#include "TechDemoCharacter.h"
#include "Interactable.generated.h"

UCLASS()
class TECHDEMO_API AInteractable : public AActor
{
	GENERATED_BODY()

protected:
	UMaterialInstanceDynamic* BillboardMaterialInstance;
	
public:
	UPROPERTY(EditAnywhere, Category = "Interactable")
	class UBoxComponent *TriggerBox;
	UPROPERTY(EditAnywhere, Category = Visual)
	class UMaterialBillboardComponent *Billboard;
	UPROPERTY(EditAnywhere, Category = Visual)
	UTexture2D* InteractableGamepadSprite;
	UPROPERTY(EditAnywhere, Category = Visual)
	UTexture2D* InteractableKeyboardSprite;


	// Sets default values for this actor's properties
	AInteractable(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void OnEnter(ATechDemoCharacter *Actor);
	
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void OnExit(ATechDemoCharacter *Actor);

	UFUNCTION()
	virtual void Interact(ATechDemoCharacter* Player);

	UFUNCTION()
	void OnInputDeviceChanged(bool bIsGamepadActive);

	virtual void ShowBillboard();
	virtual void HideBillboard();
};
