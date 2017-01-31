// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "BaseCharacter.h"
#include "TechDemoWeapon.h"
#include "SkillTree.h"
#include "TechDemoCharacter.generated.h"

UENUM()
enum class EPlayerState
{
	Normal,		
	Crouching,	
	InCover,		
	Aiming,		
	SkillTree,
	Equipment
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FPlayerStateChangedDelegate, EPlayerState, NewPlayerState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputDeviceChangedSignature, bool, bIsGamepadActive);

class AInteractable;

UCLASS(config=Game)
class ATechDemoCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	/** Arrows info */
	UPROPERTY(EditAnywhere, Category = Weapons)
	TArray<FWeaponInfo> WeaponsInfo;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USceneComponent* AimingCameraTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Aiming)
	class USkeletalMeshComponent* Bow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapons)
	class UStaticMeshComponent* EquippedArrowMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapons)
	class UStaticMeshComponent* QuiverMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio)
	class UAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere, Category = Audio)
	class USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, Category = Weapons)
	FName EquippedBowSocketName;

	UPROPERTY(EditAnywhere, Category = Weapons)
	FName EquippedMeleeSocketName;

	UPROPERTY(EditAnywhere, Category = Weapons)
	FName UnequippedBowSocketName;

	UPROPERTY(EditAnywhere, Category = Weapons)
	FName UnequippedMeleeSocketName;

	UPROPERTY(EditAnywhere, Category = Weapons)
	FName EquippedArrowSocketName;

	UPROPERTY(EditAnywhere, Category = Weapons)
	FName QuiverSocketName;

	UPROPERTY(EditAnywhere, Category = Aiming)
	float AimingSpeed;

	UPROPERTY(EditAnywhere, Category = Aiming)
	float AimingCameraAngle;

	UPROPERTY(EditAnywhere, Category = Weapons)
	float ArrowInitialSpeed;

	UPROPERTY(EditAnywhere, Category = Cover)
	float CornerReachDistance;

	UPROPERTY(EditAnywhere, Category = Debug)
	bool bDrawNoiseSphere;

	/** Equipped weapon type (normal, exploding, splatering, sleepy, melee) */
	EWeaponType CurrentWeaponType;

	/** Delegate for notifying when players state changed. Executed by ChangeState function */
	static FPlayerStateChangedDelegate OnPlayerStateChanged;

	/** Delegate for notifying whetever input device changed */
	static FInputDeviceChangedSignature OnInputDeviceChanged;

	/** Material parameter collection (used for outline drawing) */
	UPROPERTY(EditAnywhere, Category = PostProcess)
	UMaterialParameterCollection* OutlineParameterCollection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USceneComponent* SkillTreeCameraTransform;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USceneComponent* SkillTreeMoveFromCameraTransform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	class UChildActorComponent* SkillTreeChildActorComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	class USkeletalMeshComponent* SkillTreeCharacterMesh;

	class ASkillTree* SkillTree;

protected:
	// Variables
	TArray<AInteractable*> Interactables;
	AInteractable* FrontInteractable;
	UBoolProperty* ShotTriggeredProperty;
	UBoolProperty* WasShotProperty;
	APlayerCameraManager* CameraManager;
	EPlayerState PlayerState;
	EPlayerState PreviousPlayerState;
	float MaxAimingTime;
	float AimingTime;
	float SkillTreeCameraMoveTimer;
	float ChordStrain;
	float TargetChordStrain;
	float ArrowReleaseTime;
	float ArrowPlacedTime;
	float BowInitRoll;
	bool bLadderUsed;
	bool bShot;
	FRotator LadderRotation;
	bool SwordAttack;

	// UI related variables
	FVector2D RightStickValue;

	// Weapons management
	TMap<EWeaponType, FWeaponInfo> WeaponsMap;
	EWeaponType PreviousWeaponType;
	ATechDemoWeapon* CurrentWeapon;
	class FArrowPluginModule* ArrowPluginInstance;
	class ATechDemoMeleeWeapon* MeleeWeapon;
	AActor* LastActorHit;

	// Death management
	FTimerHandle DeathTimerHandle;

	// Cover related
	class ATechDemoCoverActor* CurrentCover;
	class ATechDemoCoverActor* NextCover;
	FVector NextCoverLocation;
	bool bIsMovingToNextCover;

	// Input device management
	bool bIsGamePadActive;
	bool bStart;

protected:

	// Action mappings
	void Use();	
	void SkillTreeShowHide();
	void EquipmentShow();
	void EquipmentHide();
	void Cancel();
	void SetCrouch();
	void TakeCover();
	void Shoot();

	void AnyKey(FKey KeyPressed);

	// Axis mappings

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** Called for right stick/mouse vertical input */
	void LookVertical(float Value);
	
	/** Called for right stick/mouse horizontal input */
	void LookHorizontal(float Value);

	/** Called for aiming */
	void Aim(float Value);

	/** Called for skill tree moving (it should be erased from this cpp unit and moved to more proper place) */
	void SkillTreeHorizontal(float Value);

	/** Called for skill tree moving (it should be erased from this cpp unit and moved to more proper place) */
	void SkillTreeVertical(float Value);

	// State management

	/** Called when state should change */
	void ChangeState(EPlayerState NewState);
	/** Returns player state name */
	FString GetStateName(EPlayerState State) const;
	/** Handles state change */
	UFUNCTION()
	void HandleChangeState(EPlayerState NewState);

	/** Handles outline drawing */
	void UpdateOutlines(AActor* newActor);

	/** Tick helper functions */
	void ProcessSkillTreeState(float DeltaSeconds);
	void ProcessAimingState(float DeltaSeconds);
	void ProcessCoverState(float DeltaSeconds);
	void ManageInteractables();

	// Death management
	virtual void Die() override;
	void RestartLevel();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	ATechDemoCharacter(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	virtual void AddInteractableObject(AInteractable *object);
	UFUNCTION()
	virtual void RemoveInteractableObject(AInteractable *object);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	void UseLadder(FRotator Rotation);

	UFUNCTION(BlueprintCallable, Category = Arrows)
	void PlaceArrowOnChord();
	UFUNCTION(BlueprintCallable, Category = Arrows)
	void ReleaseArrow();

	UFUNCTION(BlueprintCallable, Category = PlayerStates)
	FString GetCurrentStateName() const;

	// Weapon management
	void EquipWeapon(EWeaponType WeaponType);

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Is player in state? */
	UFUNCTION(BlueprintCallable, Category = PlayerStates)
	FORCEINLINE bool IsInState(EPlayerState State) const { return (PlayerState == State); }
	/** Was player in state? */
	FORCEINLINE bool WasInState(EPlayerState State) const { return (PreviousPlayerState == State); }

	// Blueprint callable functions

	// Animation related
	/** Is player crouching? */
	UFUNCTION(BlueprintCallable, Category = Movement)
	FORCEINLINE bool IsCrouching() const
	{
		return (PlayerState == EPlayerState::Crouching) || (PlayerState == EPlayerState::Equipment && PreviousPlayerState == EPlayerState::Crouching);
	}
	/** Is player aiming? */
	UFUNCTION(BlueprintCallable, Category = Attack)
	FORCEINLINE bool IsAiming() const
	{
		return (PlayerState == EPlayerState::Aiming);
	}
	/** Is player in cover? */
	UFUNCTION(BlueprintCallable, Category = Movement)
	FORCEINLINE bool IsInCover() const
	{
		return (PlayerState == EPlayerState::InCover) || (PlayerState == EPlayerState::Equipment && PreviousPlayerState == EPlayerState::InCover);
	}

	UFUNCTION(BlueprintCallable, Category = Aiming)
	FORCEINLINE float GetChordStrain() const
	{
		return ChordStrain;
	}

	FORCEINLINE FVector2D GetRightStickValue() const
	{
		return RightStickValue;
	}

	UFUNCTION(BlueprintCallable, Category = Weapons)
	FORCEINLINE bool ShouldPlaceArrow() const
	{
		return IsInState(EPlayerState::Aiming) && CurrentWeapon == nullptr && CurrentWeaponType != EWeaponType::Melee && ((GetWorld()->GetRealTimeSeconds() - ArrowReleaseTime) > 0.5f);
	}

	UFUNCTION(BlueprintCallable, Category = Weapons)
	void GrabArrow()
	{
		EquippedArrowMesh->SetStaticMesh(WeaponsMap[CurrentWeaponType].WeaponMesh);
		EquippedArrowMesh->SetHiddenInGame(false);
	}

	UFUNCTION(BlueprintCallable, Category = Weapons)
	FORCEINLINE bool CanShoot() const
	{
		return IsInState(EPlayerState::Aiming) && CurrentWeapon != nullptr && CurrentWeaponType != EWeaponType::Melee && ((GetWorld()->GetRealTimeSeconds() - ArrowPlacedTime) > 0.2f);
	}

	UFUNCTION(BlueprintCallable, Category = Attack)
	FORCEINLINE bool GetSwordAttack() const
	{
		return SwordAttack;
	}

	UFUNCTION(BlueprintCallable, Category = Attack)
	void SetSwordAttack(bool attack);

	// Cover related
	void TakeCover(class ATechDemoCoverActor* Cover);
	void RemoveCover(class ATechDemoCoverActor* Cover);

	FORCEINLINE bool IsMovingToNextCover() const
	{
		return bIsMovingToNextCover;
	}
};

