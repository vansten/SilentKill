// Copyright by Keyboard Facerollers

#pragma once

#include "Interactable.h"
#include "InteractableDoors.generated.h"

/**
 * 
 */
UCLASS()
class TECHDEMO_API AInteractableDoors : public AInteractable
{
	GENERATED_BODY()
	protected:
		AActor* CurrentPlayer;
		bool Opened;
		UPROPERTY(EditAnywhere, Category = "Doors")
		bool Closable;
		bool Use;
		float Time;
		int SideDirection;
		int OpenDirection;

protected:
		UFUNCTION()
		void CalculateDirection(ATechDemoCharacter * Actor);

		UFUNCTION()
			void OnCloseOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
		UFUNCTION()
			void OnCloseOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	public:
		UPROPERTY(VisibleAnywhere, Category = "Doors")
		UStaticMeshComponent *DoorsMesh;
		UPROPERTY(VisibleAnywhere, Category = "Doors")
		USceneComponent *RotationRoot;
		UPROPERTY(EditAnywhere, Category = "Doors")
		bool Left;
		UPROPERTY(EditAnywhere, Category = "Doors")
		class AInteractableDoors *SecondWing;
		UPROPERTY(EditAnywhere, Category = "Doors")
		class UCurveFloat* Curve;
		UPROPERTY(VisibleAnywhere, Category = "Doors")
		class UBoxComponent *CloseTriggerBox;

		AInteractableDoors(const FObjectInitializer& ObjectInitializer);

		virtual void BeginPlay() override;

		virtual void Tick(float DeltaSeconds) override;

		UFUNCTION()
		virtual void OnEnter(ATechDemoCharacter * Actor) override;

		UFUNCTION()
		virtual void OnExit(ATechDemoCharacter * Actor) override;

		UFUNCTION()
		virtual void Interact(ATechDemoCharacter* Player) override;
	
		virtual void ShowBillboard() override;
};
