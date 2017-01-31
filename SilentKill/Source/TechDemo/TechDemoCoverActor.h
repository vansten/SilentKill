// Copyright by Keyboard Facerollers

#pragma once

#include "GameFramework/Actor.h"
#include "TechDemoCoverActor.generated.h"

UCLASS()
class TECHDEMO_API ATechDemoCoverActor : public AActor
{
	GENERATED_BODY()
	
protected:
	ATechDemoCoverActor* CurrentCoverToTake;
	UPrimitiveComponent* CurrentCorner;

	UMaterialInstanceDynamic* CornerMaterialInstance;
	UMaterialInstanceDynamic* CoverMaterialInstance;

public:
	UPROPERTY(EditAnywhere, Category = Cover)
	UBoxComponent* Trigger;

	UPROPERTY(EditAnywhere, Category = Cover)
	UBoxComponent* LeftCorner;

	UPROPERTY(EditAnywhere, Category = Cover)
	UBoxComponent* RightCorner;

	UPROPERTY(EditAnywhere, Category = Cover)
	UArrowComponent* FacingDirection;

	UPROPERTY(EditAnywhere, Category = Visual)
	UMaterialBillboardComponent* CoverBillboard;

	UPROPERTY(EditAnywhere, Category = Visual)
	UMaterialBillboardComponent* CornerBillboard;

	UPROPERTY(EditAnywhere, Category = Cover)
	ATechDemoCoverActor* CoverToLeft;

	UPROPERTY(EditAnywhere, Category = Cover)
	ATechDemoCoverActor* CoverToRight;

	UPROPERTY(EditAnywhere, Category = Visual)
	UTexture2D* CornerGamepadSprite;

	UPROPERTY(EditAnywhere, Category = Visual)
	UTexture2D* CornerKeyboardSprite;

	UPROPERTY(EditAnywhere, Category = Visual)
	UTexture2D* CoverGamepadSprite;

	UPROPERTY(EditAnywhere, Category = Visual)
	UTexture2D* CoverKeyboardSprite;

protected:
	UFUNCTION()
	void OnCornerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCornerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	void SetCurrentCorner(UPrimitiveComponent* Corner);

	UFUNCTION()
	void OnInputDeviceChanged(bool bIsGamepadActive);

public:	
	// Sets default values for this actor's properties
	ATechDemoCoverActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool TryUseCorner(class ATechDemoCharacter* Player, FVector& NearestLocationInNextCover, ATechDemoCoverActor** NextCover);
	void TakeCover(class ATechDemoCharacter* Player);
	void LeaveCover(class ATechDemoCharacter* Player);

	FORCEINLINE bool IsInCorner() const
	{
		return CurrentCorner != nullptr;
	}

	FORCEINLINE FVector GetCurrentCornerForward() const
	{
		if(CurrentCorner == nullptr)
		{
			return FVector::ZeroVector;
		}
		FVector Forward = GetActorRotation().RotateVector(-CurrentCorner->RelativeLocation);
		return Forward.GetSafeNormal();
	}
};
