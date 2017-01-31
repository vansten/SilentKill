// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TechDemo.h"
#include "Runtime/Engine/Public/Animation/AnimInstanceProxy.h"
#include "Interactable.h"
#include "TechDemoCharacter.h"
#include "ArrowPlugin.h"
#include "TechDemoArrow.h"
#include "TechDemoCoverActor.h"
#include "TechDemoMeleeWeapon.h"
#include "Runtime/Engine/Classes/Kismet/KismetMaterialLibrary.h"
#include "Perception/AISense_Hearing.h"
#include "TechDemoPlayerController.h"

FPlayerStateChangedDelegate ATechDemoCharacter::OnPlayerStateChanged;
FInputDeviceChangedSignature ATechDemoCharacter::OnInputDeviceChanged;

//////////////////////////////////////////////////////////////////////////
// ATechDemoCharacter

ATechDemoCharacter::ATechDemoCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bDoCollisionTest = true;
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	Bow = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Bow"));
	Bow->SetupAttachment(GetMesh(), EquippedBowSocketName);

	EquippedArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EquippedArrowMesh"));
	EquippedArrowMesh->SetupAttachment(GetMesh(), EquippedArrowSocketName);
	EquippedArrowMesh->SetHiddenInGame(true);

	QuiverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("QuiverMesh"));
	QuiverMesh->SetupAttachment(GetMesh(), QuiverSocketName);
	QuiverMesh->SetCanEverAffectNavigation(false);

	// Create aiming camera transform and attach it to socket
	AimingCameraTransform = CreateDefaultSubobject<USceneComponent>(TEXT("AimingTransform"));
	AimingCameraTransform->SetupAttachment(GetMesh(), FName("AimingCameraSocket"));

	// Default value for aiming camera angle (should be overriden by blueprint loading mechanism or something like that)
	AimingCameraAngle = 90.0f;

	// Create skill tree camera transform and attach it to socket
	SkillTreeCameraTransform = CreateDefaultSubobject<USceneComponent>(TEXT("SkillTreeTransform"));
	SkillTreeCameraTransform->SetupAttachment(GetMesh(), FName("SkillTreeCameraSocket"));

	SkillTreeCharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkillTreeCharacterMesh"));
	SkillTreeCharacterMesh->SetupAttachment(RootComponent);
	SkillTreeCharacterMesh->SetHiddenInGame(true);

	SkillTreeChildActorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("SkillTreeChildActorComponent"));
	SkillTreeChildActorComponent->SetupAttachment(SkillTreeCharacterMesh, FName("SkillTreeSocket"));
	SkillTreeChildActorComponent->SetHiddenInGame(false);

	SkillTreeMoveFromCameraTransform = CreateDefaultSubobject<USceneComponent>(TEXT("SkillTreeCameraFromTransform"));
	SkillTreeMoveFromCameraTransform->SetupAttachment(GetMesh(), FName("SkillTreeCameraFromSocket"));


	// Audio
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->bIsUISound = true;
	AudioComponent->bAllowSpatialization = false;

	// Default value for looking at arm
	//AimingCameraAngle = 55.f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	bLadderUsed = false;
	bIsGamePadActive = false;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATechDemoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	bStart = true;

	// Set up gameplay key bindings
	check(PlayerInputComponent);
	
	// Bind actions
	PlayerInputComponent->BindAction("Use", EInputEvent::IE_Released, this, &ATechDemoCharacter::Use);
	PlayerInputComponent->BindAction("SkillTreeShowHide", EInputEvent::IE_Released, this, &ATechDemoCharacter::SkillTreeShowHide);
	PlayerInputComponent->BindAction("EquipmentShowHide", EInputEvent::IE_Pressed, this, &ATechDemoCharacter::EquipmentShow);
	PlayerInputComponent->BindAction("EquipmentShowHide", EInputEvent::IE_Released, this, &ATechDemoCharacter::EquipmentHide);
	PlayerInputComponent->BindAction("Cancel", EInputEvent::IE_Released, this, &ATechDemoCharacter::Cancel);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &ATechDemoCharacter::SetCrouch);
	PlayerInputComponent->BindAction("TakeCover", EInputEvent::IE_Released, this, &ATechDemoCharacter::TakeCover);
	PlayerInputComponent->BindAction("Shoot", EInputEvent::IE_Released, this, &ATechDemoCharacter::Shoot);
	PlayerInputComponent->BindAction("AnyKey", EInputEvent::IE_Pressed, this, &ATechDemoCharacter::AnyKey);

	// Bind axises
	PlayerInputComponent->BindAxis("MoveForward", this, &ATechDemoCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATechDemoCharacter::MoveRight);

	PlayerInputComponent->BindAxis("CameraLookVertical", this, &ATechDemoCharacter::LookVertical);
	PlayerInputComponent->BindAxis("CameraLookHorizontal", this, &ATechDemoCharacter::LookHorizontal);

	PlayerInputComponent->BindAxis("Aim", this, &ATechDemoCharacter::Aim);

	PlayerInputComponent->BindAxis("SkillTreeMoveHorizontal", this, &ATechDemoCharacter::SkillTreeHorizontal);
	PlayerInputComponent->BindAxis("SkillTreeMoveVertical", this, &ATechDemoCharacter::SkillTreeVertical);

}

void ATechDemoCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentCover = nullptr;

	for(auto& item : WeaponsInfo)
	{
		WeaponsMap.Add(item.WeaponType, item);
	}

	ChangeState(EPlayerState::Normal);
	MaxAimingTime = 1.0f / AimingSpeed;
	OnPlayerStateChanged.BindDynamic(this, &ATechDemoCharacter::HandleChangeState);
	
	APlayerController* pc = Cast<APlayerController>(GetController());
	if(pc != nullptr)
	{
		CameraManager = pc->PlayerCameraManager;
	}

	RightStickValue = FVector2D(0.0f, 1.0f);

	ShotTriggeredProperty = FindField<UBoolProperty>(Bow->GetAnimInstance()->GetClass(), "ShotTriggered");
	bShot = false;

	WasShotProperty = FindField<UBoolProperty>(GetMesh()->GetAnimInstance()->GetClass(), "WasShot");

	MeleeWeapon = GetWorld()->SpawnActor<ATechDemoMeleeWeapon>(WeaponsMap[EWeaponType::Melee].WeaponClass);
	if(MeleeWeapon == nullptr)
	{
		UE_LOG(LogActor, Error, TEXT("Make sure that there is a weapon info for Melee weapon type"));
		FPlatformMisc::DebugBreak();
	}

	CurrentWeaponType = EWeaponType::Bow_Normal;
	EquipWeapon(CurrentWeaponType);
	ArrowReleaseTime = GetWorld()->GetRealTimeSeconds();

	ArrowPluginInstance = FArrowPluginModule::Get();
	ArrowPluginInstance->Initialize(0.5f, GetWorld());

	QuiverMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false), QuiverSocketName);
	EquippedArrowMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false), EquippedArrowSocketName);

	AudioComponent->SetSound(EquipSound);

	BowInitRoll = Bow->RelativeRotation.Roll;

	GetMesh()->SetVisibility(true);
	GetMesh()->SetHiddenInGame(false);
	SkillTreeCharacterMesh->SetVisibility(true);
	SkillTreeCharacterMesh->SetHiddenInGame(true);

	if (SkillTreeChildActorComponent != nullptr && SkillTreeChildActorComponent->GetChildActor() != nullptr)
	{
		SkillTree = Cast<ASkillTree>(SkillTreeChildActorComponent->GetChildActor());
		SkillTreeChildActorComponent->GetChildActor()->SetActorHiddenInGame(true);
	}

	check(ShotTriggeredProperty && "ShotTriggered bool variable not found in BowAnimationBlueprint");
	check(WasShotProperty && "WasShot bool variable not found in player animation blueprint");
	check(CameraManager && "How the fuck is it possible? PlayerCameraManager should be instanced in begin play :<");
}

void ATechDemoCharacter::Tick(float DeltaSeconds)
{
	if (IsInState(EPlayerState::SkillTree))
	{
		ProcessSkillTreeState(DeltaSeconds);
		return;
	}

	ProcessAimingState(DeltaSeconds);
	ManageInteractables();
	
	if(IsInState(EPlayerState::InCover))
	{
		ProcessCoverState(DeltaSeconds);
	}
}

void ATechDemoCharacter::Die()
{
	Super::Die();
	GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &ATechDemoCharacter::RestartLevel, 3.0f);
}

void ATechDemoCharacter::RestartLevel()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetName()), false);
}

void ATechDemoCharacter::AddInteractableObject(AInteractable* object)
{
	Interactables.Add(object);
}

void ATechDemoCharacter::RemoveInteractableObject(AInteractable* object)
{
	if (Interactables.Contains(object))
	{
		Interactables.Remove(object);
	}
}

void ATechDemoCharacter::Use() 
{
	if(bIsMovingToNextCover)
	{
		return;
	}

	if (IsInState(EPlayerState::SkillTree))
	{
		SkillTree->OnUse();
	}
	else
	{
		if (FrontInteractable != nullptr)
		{
			FrontInteractable->Interact(this);
		}
		else if(CurrentCover != nullptr)
		{
			bIsMovingToNextCover = CurrentCover->TryUseCorner(this, NextCoverLocation, &NextCover);
		}
	}
}

void ATechDemoCharacter::UseLadder(FRotator Rotation)
{
	if (!bLadderUsed) {
		bLadderUsed = true;
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		LadderRotation = Rotation;
	} else {
		bLadderUsed = false;
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void ATechDemoCharacter::SkillTreeShowHide()
{
	if(bIsMovingToNextCover)
	{
		return;
	}

	ATechDemoPlayerController* playerController = Cast<ATechDemoPlayerController>(GetController());
	if(playerController && playerController->IsQuestWidgetShown())
	{
		return;
	}

	if (IsInState(EPlayerState::SkillTree))
	{
		this->GetMesh()->SetHiddenInGame(false);
		SkillTreeChildActorComponent->GetChildActor()->SetActorHiddenInGame(true);
		SkillTreeCharacterMesh->SetHiddenInGame(true);
		CameraBoom->bUsePawnControlRotation = true;
		ChangeState(PreviousPlayerState);
	}
	else
	{
		if(IsInState(EPlayerState::Equipment))
		{
			EquipmentHide();
		}
		SkillTree->OnShow();
		SkillTreeChildActorComponent->GetChildActor()->SetActorHiddenInGame(false);
		SkillTreeCameraMoveTimer = 0.0f;
		SkillTreeCharacterMesh->SetHiddenInGame(false);
		this->GetMesh()->SetHiddenInGame(true);
		ChangeState(EPlayerState::SkillTree);
	}
}

void ATechDemoCharacter::EquipmentShow()
{
	if(bIsMovingToNextCover)
	{
		return;
	}

	ATechDemoPlayerController* playerController = Cast<ATechDemoPlayerController>(GetController());
	if(playerController && playerController->IsQuestWidgetShown())
	{
		return;
	}

	if(IsInState(EPlayerState::Aiming))
	{
		if(CurrentWeapon != nullptr && CurrentWeaponType != EWeaponType::Melee)
		{
			CurrentWeapon->Destroy();
			CurrentWeapon = nullptr;
		}
		ChangeState(EPlayerState::Normal);
	}
	else if(IsInState(EPlayerState::SkillTree))
	{
		return;
	}

	PreviousWeaponType = CurrentWeaponType;
	ChangeState(EPlayerState::Equipment);
}

void ATechDemoCharacter::EquipmentHide()
{
	if(IsInState(EPlayerState::Equipment))
	{
		 //EquipWeapon(CurrentWeaponType);
		 ChangeState(PreviousPlayerState);
	}
}

void ATechDemoCharacter::Cancel()
{
	if(bIsMovingToNextCover)
	{
		return;
	}

	if(IsInState(EPlayerState::SkillTree))
	{
		SkillTreeShowHide();
	}
}

void ATechDemoCharacter::SetCrouch()
{
	if(bIsMovingToNextCover)
	{
		return;
	}

	if(IsInState(EPlayerState::InCover))
	{
		TakeCover();
		return;
	}

	if(IsInState(EPlayerState::Normal))
	{
		ChangeState(EPlayerState::Crouching);
	}
	else if(IsInState(EPlayerState::Crouching))
	{
		ChangeState(EPlayerState::Normal);
	}
}

void ATechDemoCharacter::TakeCover()
{
	if(bIsMovingToNextCover)
	{
		return;
	}

	if(CurrentCover != nullptr)
	{
		if(IsInState(EPlayerState::InCover))
		{
			ChangeState(EPlayerState::Normal);
			CurrentCover->LeaveCover(this);
		}
		else if(!IsInState(EPlayerState::SkillTree))
		{
			ChangeState(EPlayerState::InCover);
			CurrentCover->TakeCover(this);
		}
	}
}

void ATechDemoCharacter::Shoot()
{
	if(bIsMovingToNextCover)
	{
		return;
	}

	if(CanShoot())
	{
		bShot = true;
		ArrowPluginInstance->HideLine();
		ShotTriggeredProperty->SetPropertyValue_InContainer(Bow->GetAnimInstance(), true);
		return;
	}

	if (IsInState(EPlayerState::Normal) && CurrentWeaponType == EWeaponType::Melee)
	{
		SwordAttack = true;
		MeleeWeapon->WeaponCollision->bGenerateOverlapEvents = true;
	}
}

void ATechDemoCharacter::AnyKey(FKey KeyPressed)
{
	if(KeyPressed.IsGamepadKey())
	{
		if(!bIsGamePadActive || bStart)
		{
			bIsGamePadActive = true;
			OnInputDeviceChanged.Broadcast(true);
		}
	}
	else
	{
		if(bIsGamePadActive || bStart)
		{
			bIsGamePadActive = false;
			OnInputDeviceChanged.Broadcast(false);
		}
	}

	bStart = false;
}

void ATechDemoCharacter::ReleaseArrow()
{
	if(CurrentWeaponType == EWeaponType::Melee)
	{
		return;
	}

	ATechDemoArrow* CurrentArrow = Cast<ATechDemoArrow>(CurrentWeapon);

	if(CurrentArrow != nullptr)
	{
		WasShotProperty->SetPropertyValue_InContainer(GetMesh()->GetAnimInstance(), true);
		CurrentWeapon->DetachRootComponentFromParent();
		CurrentArrow->Shoot(CurrentArrow->GetActorForwardVector() * TargetChordStrain * ArrowInitialSpeed);
		CurrentWeapon = nullptr;
		bShot = false;
		ArrowReleaseTime = GetWorld()->GetRealTimeSeconds();
	}
}

void ATechDemoCharacter::PlaceArrowOnChord()
{
	if(CurrentWeaponType == EWeaponType::Melee)// || !ShouldPlaceArrow())
	{
		return;
	}

	EquippedArrowMesh->SetHiddenInGame(true);

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FAttachmentTransformRules attachmentRules(EAttachmentRule::SnapToTarget, true);

	CurrentWeapon = Cast<ATechDemoArrow>(GetWorld()->SpawnActor<ATechDemoArrow>(WeaponsMap[CurrentWeaponType].WeaponClass, FTransform(), params));
	CurrentWeapon->AttachToComponent(Bow, attachmentRules, "ArrowHook");

	ShotTriggeredProperty->SetPropertyValue_InContainer(Bow->GetAnimInstance(), false);
	ArrowPlacedTime = GetWorld()->GetRealTimeSeconds();
}

void ATechDemoCharacter::MoveForward(float Value) 
{
	if(bIsMovingToNextCover)
	{
		return;
	}

	if (Controller != nullptr && Value != 0.0f && !IsInState(EPlayerState::SkillTree) && !SwordAttack) {
		if (IsCrouching() || IsAiming() || IsInCover()) {
			Value *= 0.3f;
		}

		const auto Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		FVector Direction;
		if (bLadderUsed) {
			Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Z);
			SetActorRotation(LadderRotation);
		} else {
			Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		}

		if(IsInState(EPlayerState::InCover) && CurrentCover != nullptr)
		{
			Direction = FollowCamera->GetForwardVector();
			Direction.Z = 0.0f;
			Direction.Normalize();
			FVector CoverFacingVector = CurrentCover->FacingDirection->GetForwardVector();
			FVector PossibleDirection = FVector::CrossProduct(-CoverFacingVector, FVector::UpVector).GetSafeNormal();

			float alongCover = FVector::DotProduct(PossibleDirection, Direction);
			float oppositeCover = FVector::DotProduct(CoverFacingVector, Direction);
			if(FMath::Abs(oppositeCover) > FMath::Abs(alongCover))
			{
				Direction = CoverFacingVector;
				Value *= oppositeCover;
			}
			else
			{
				Value *= alongCover;
				Direction = PossibleDirection;

				if(CurrentCover->IsInCorner())
				{
					FVector CornerForward = CurrentCover->GetCurrentCornerForward();
					float cornerDotMoveDir = FVector::DotProduct(Direction * Value, CornerForward);
					if(cornerDotMoveDir <= 0.0f)
					{
						Value = 0.0f;
					}
				}
			}
		}

		AddMovementInput(Direction, Value);

		if (!IsCrouching() && !IsInCover()) {
			float radius = 1000.0f * Value;
			if(bDrawNoiseSphere)
			{
				DrawDebugSphere(GetWorld(), GetActorLocation(), radius, 128, FColor::Red);
			}
			UAISense_Hearing::ReportNoiseEvent(this, GetActorLocation(), 1.f, this, radius);
		}
	} else if (bLadderUsed && Controller != nullptr && Value == 0.0f && !IsInState(EPlayerState::SkillTree)) {
		GetCharacterMovement()->StopMovementImmediately();
	}
}

void ATechDemoCharacter::MoveRight(float Value)
{
	if(bIsMovingToNextCover)
	{
		return;
	}
	if ( (Controller != NULL) && (Value != 0.0f) && !IsInState(EPlayerState::SkillTree) && !bLadderUsed && !SwordAttack)
	{
		if(IsCrouching() || IsAiming() || IsInCover())
		{
			Value *= 0.3f;
		}

		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		if(IsInState(EPlayerState::InCover) && CurrentCover != nullptr)
		{
			Direction = FollowCamera->GetRightVector();
			Direction.Z = 0.0f;
			Direction.Normalize();
			FVector CoverFacingVector = CurrentCover->FacingDirection->GetForwardVector();
			FVector PossibleDirection = FVector::CrossProduct(-CoverFacingVector, FVector::UpVector).GetSafeNormal();

			float alongCover = FVector::DotProduct(PossibleDirection, Direction);
			float oppositeCover = FVector::DotProduct(CoverFacingVector, Direction);
			if(FMath::Abs(oppositeCover) > FMath::Abs(alongCover))
			{
				Direction = CoverFacingVector;
				Value *= oppositeCover;
			}
			else
			{
				Value *= alongCover;
				Direction = PossibleDirection;

				if(CurrentCover->IsInCorner())
				{
					FVector CornerForward = CurrentCover->GetCurrentCornerForward();
					float cornerDotMoveDir = FVector::DotProduct(Direction * Value, CornerForward);
					if(cornerDotMoveDir <= 0.0f)
					{
						Value = 0.0f;
					}
				}
			}
		}

		// add movement in that direction
		AddMovementInput(Direction, Value);

		if (!IsCrouching() && !IsInCover()) {
			float radius = 1000.0f * Value;
			if(bDrawNoiseSphere)
			{
				DrawDebugSphere(GetWorld(), GetActorLocation(), radius, 128, FColor::Red);
			}
			UAISense_Hearing::ReportNoiseEvent(this, GetActorLocation(), 1.f, this, radius);
		}
	}
}

void ATechDemoCharacter::LookVertical(float Value)
{
	if((Controller != NULL))
	{
		if(IsInState(EPlayerState::Equipment))
		{
			RightStickValue.Y = -Value;
		}
		else if(!IsInState(EPlayerState::SkillTree))
		{
			AddControllerPitchInput(Value);
		}
	}
}

void ATechDemoCharacter::LookHorizontal(float Value)
{
	if((Controller != NULL))
	{
		if(IsInState(EPlayerState::Equipment))
		{
			RightStickValue.X = Value;
		}
		else if(!IsInState(EPlayerState::SkillTree))
		{
			if(IsInState(EPlayerState::Aiming))
			{
				FRotator ActorRotation = GetActorRotation();
				ActorRotation.Yaw += Value;
				SetActorRotation(ActorRotation);
			}
			else
			{
				AddControllerYawInput(Value);
			}
		}
	}
}

void ATechDemoCharacter::Aim(float Value)
{
	if(bIsMovingToNextCover)
	{
		return;
	}

	if(IsInState(EPlayerState::Equipment) || IsInState(EPlayerState::SkillTree) || CurrentWeaponType == EWeaponType::Melee)
	{
		return;
	}

	ATechDemoArrow* CurrentArrow = Cast<ATechDemoArrow>(CurrentWeapon);
	TargetChordStrain = Value;
	//Bow->RelativeRotation.Roll = FMath::Sign(TargetChordStrain) * 90.0f;
	if(TargetChordStrain > 0.0f)
	{
		Bow->RelativeRotation.Roll = BowInitRoll + GetControlRotation().Pitch;
		if(CurrentArrow != nullptr)
		{
			if(WeaponsMap[CurrentWeaponType].WeaponClass != CurrentArrow->GetClass())
			{
				CurrentArrow->Destroy();
				CurrentArrow = nullptr;
			}
		}

		ChangeState(EPlayerState::Aiming);
	}
	else if(IsInState(EPlayerState::Aiming))
	{
		if(CurrentWeapon != nullptr && CurrentWeapon->WeaponType != EWeaponType::Melee)
		{
			CurrentWeapon->Destroy();
			CurrentWeapon = nullptr;
		}

		EquippedArrowMesh->SetHiddenInGame(true);

		ChangeState(PreviousPlayerState);
	}
}

void ATechDemoCharacter::SkillTreeHorizontal(float Value)
{
	if(bIsMovingToNextCover)
	{
		return;
	}

	if(IsInState(EPlayerState::SkillTree))
	{
		SkillTree->OnHorizontalMove(Value);
	}
}

void ATechDemoCharacter::SkillTreeVertical(float Value)
{
	if(bIsMovingToNextCover)
	{
		return;
	}

	if(IsInState(EPlayerState::SkillTree))
	{
		SkillTree->OnVerticalMove(Value);
	}
}

void ATechDemoCharacter::ChangeState(EPlayerState NewState)
{
	if(PlayerState != NewState)
	{
		PreviousPlayerState = PlayerState;
		PlayerState = NewState;
		OnPlayerStateChanged.ExecuteIfBound(PlayerState);
	}
}

FString ATechDemoCharacter::GetStateName(EPlayerState State) const
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPlayerState"), true);
	if(!enumPtr)
	{
		return FString("Invalid");
	}

	return enumPtr->GetEnumName((int32)State);
}

void ATechDemoCharacter::HandleChangeState(EPlayerState NewState)
{
	bool aiming = NewState == EPlayerState::Aiming;
	bool inCover = NewState == EPlayerState::InCover;
	CameraBoom->bUsePawnControlRotation = !aiming;
	FollowCamera->bUsePawnControlRotation = aiming;
	SetActorRotation(FRotator(0.0f, GetControlRotation().Yaw, 0.0f));
	GetCharacterMovement()->bOrientRotationToMovement = !aiming;// && !inCover;
	if(!aiming)
	{
		ArrowPluginInstance->HideLine();
		FollowCamera->SetRelativeRotation(FRotator::ZeroRotator);
		UpdateOutlines(nullptr);
	}
	else
	{
		Bow->RelativeRotation.Roll = 0.0f;
	}

	if(NewState == EPlayerState::SkillTree)
	{
		CameraBoom->bUsePawnControlRotation = false;
		FollowCamera->bUsePawnControlRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		CameraBoom->SetRelativeRotation(FRotator(-65.0f, -5.0f, 0.0f), true);
		//CameraBoom->SetRelativeRotation(SkillTreeCameraTransform->GetComponentRotation(), true);
	}
}

FString ATechDemoCharacter::GetCurrentStateName() const
{
	return GetStateName(PlayerState);
}

void ATechDemoCharacter::EquipWeapon(EWeaponType WeaponType)
{
	if(PreviousWeaponType != WeaponType)
	{
		PreviousWeaponType = CurrentWeaponType;
		AudioComponent->Play();
		bool melee = WeaponType == EWeaponType::Melee;
		CurrentWeaponType = WeaponType;
		static FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, true);
		if(melee)
		{
			Bow->AttachToComponent(GetMesh(), rules, UnequippedBowSocketName);
			MeleeWeapon->AttachToComponent(GetMesh(), rules, EquippedMeleeSocketName);
		}
		else
		{
			Bow->AttachToComponent(GetMesh(), rules, EquippedBowSocketName);
			MeleeWeapon->AttachToComponent(GetMesh(), rules, UnequippedMeleeSocketName);
		}
	}
}

void ATechDemoCharacter::UpdateOutlines(AActor* newActor)
{
	if(LastActorHit != newActor)
	{
		if(LastActorHit != nullptr)
		{
			TArray<UMeshComponent*> meshes;
			LastActorHit->GetComponents<UMeshComponent>(meshes);
			int size = meshes.Num();
			for(int i = 0; i < size; ++i)
			{
				meshes[i]->SetRenderCustomDepth(false);
			}
		}

		if(newActor != nullptr)
		{
			TArray<UMeshComponent*> meshes;
			newActor->GetComponents<UMeshComponent>(meshes);
			int size = meshes.Num();
			for(int i = 0; i < size; ++i)
			{
				meshes[i]->SetRenderCustomDepth(true);
				meshes[i]->CustomDepthStencilValue = 254;
			}
		}

		LastActorHit = newActor;
	}
}

void ATechDemoCharacter::ProcessSkillTreeState(float DeltaSeconds)
{
	if(IsInState(EPlayerState::SkillTree))
	{
		SkillTreeCameraMoveTimer += DeltaSeconds;
		FVector newLocation = FMath::VInterpTo(SkillTreeMoveFromCameraTransform->GetComponentLocation(), SkillTreeCameraTransform->GetComponentLocation(), SkillTreeCameraMoveTimer, 1.f);
		FollowCamera->SetWorldLocation(newLocation);
	}
}

void ATechDemoCharacter::ProcessAimingState(float DeltaSeconds)
{
	// Update chord strain
	ChordStrain = FMath::FInterpTo(ChordStrain, TargetChordStrain, DeltaSeconds, AimingSpeed);

	// Calculate min and max camera yaw and aiming time based on current state
	float minYaw = 0.0f;
	float maxYaw = 359.999f;
	if(IsInState(EPlayerState::Aiming))
	{
		AimingTime += DeltaSeconds;
		FRotator actorRotation = GetActorRotation();
		float angle = GetActorForwardVector().CosineAngle2D(GetMesh()->GetForwardVector());
		minYaw = actorRotation.Yaw + angle;
		maxYaw = actorRotation.Yaw + angle;

		ATechDemoArrow* CurrentArrow = Cast<ATechDemoArrow>(CurrentWeapon);

		if(!bShot && CurrentArrow != nullptr && CurrentArrow->Mesh->DoesSocketExist(ATechDemoArrow::ArrowheadSocketName))
		{
			ArrowPluginInstance->SetOwner(CurrentArrow->Mesh, ATechDemoArrow::ArrowheadSocketName);
			FLinearColor color = WeaponsMap[CurrentWeaponType].SelectionColor;
			FPath path = ArrowPluginInstance->DrawPath(CurrentArrow->Mesh->GetSocketTransform(ATechDemoArrow::ArrowheadSocketName).GetLocation(), CurrentArrow->GetActorForwardVector(), color);
			UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), OutlineParameterCollection, FName("OutlineColor"), color);

			UpdateOutlines(path.HitActor);
		}
	}
	else
	{
		AimingTime -= DeltaSeconds;
	}

	// Set camera position
	AimingTime = FMath::Clamp(AimingTime, 0.0f, MaxAimingTime);
	FVector newLocation = FMath::VInterpTo(CameraBoom->GetSocketLocation(CameraBoom->SocketName), AimingCameraTransform->GetComponentLocation(), AimingTime, AimingSpeed);
	FollowCamera->SetWorldLocation(newLocation);

	// And min and max camera yaw
	CameraManager->ViewYawMin = minYaw;
	CameraManager->ViewYawMax = maxYaw;
}

void ATechDemoCharacter::ProcessCoverState(float DeltaSeconds)
{
	if(bIsMovingToNextCover)
	{
		FVector Direction = NextCoverLocation - GetActorLocation();
		//DrawDebugLine(GetWorld(), GetActorLocation(), NextCoverLocation, FColor::Yellow, true, 10.0f);
		Direction.Z = 0.0f;
		float DirectionLength = Direction.Size();
		Direction.Normalize();

		FHitResult HitInfo;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		if(GetWorld()->LineTraceSingleByChannel(HitInfo, GetActorLocation(), NextCoverLocation, ECollisionChannel::ECC_WorldStatic, Params))
		{
			FVector Normal = HitInfo.Normal;
			Normal.Z = 0.0f;
			Normal.Normalize();
			FVector PossibleDirection = FVector::CrossProduct(FVector::UpVector, Normal).GetSafeNormal();
			Direction = PossibleDirection * FMath::Sign(FVector::DotProduct(Direction, PossibleDirection));
		}

		AddMovementInput(Direction, 0.3f);
		
		if(DirectionLength <= FMath::Max(CornerReachDistance, GetCapsuleComponent()->GetScaledCapsuleRadius()))
		{
			if(CurrentCover != nullptr)
			{
				CurrentCover->LeaveCover(this);
			}
			if(NextCover != nullptr)
			{
				NextCover->TakeCover(this);
			}
			TakeCover(NextCover);
			NextCover = nullptr;
			bIsMovingToNextCover = false;
		}

		return;
	}

	if(CurrentCover == nullptr)
	{
		ChangeState(EPlayerState::Normal);
		return;
	}
}

void ATechDemoCharacter::ManageInteractables()
{
	int32 num = 0;
	if((num = Interactables.Num()) > 0)
	{
		AInteractable* prevFrontInteractable = FrontInteractable;
		FrontInteractable = nullptr;
		FVector cameraForward = FollowCamera->GetForwardVector();
		cameraForward.Z = 0.0f;
		cameraForward.Normalize();
		float maxDot = 0.9f;
		for(int i = 0; i < num; ++i)
		{
			FVector cameraToInteractable = Interactables[i]->GetActorLocation() - FollowCamera->GetComponentLocation();
			cameraToInteractable.Z = 0.0f;
			cameraToInteractable.Normalize();
			float dot = FVector::DotProduct(cameraForward, cameraToInteractable);
			if(dot > maxDot)
			{
				FrontInteractable = Interactables[i];
				maxDot = dot;
			}
		}

		if(prevFrontInteractable != FrontInteractable)
		{
			if(prevFrontInteractable != nullptr)
			{
				prevFrontInteractable->HideBillboard();
			}
			if(FrontInteractable != nullptr)
			{
				FrontInteractable->ShowBillboard();
			}
		}
	}
	else
	{
		if(FrontInteractable != nullptr)
		{
			FrontInteractable->HideBillboard();
		}
		FrontInteractable = nullptr;
	}
}

void ATechDemoCharacter::TakeCover(ATechDemoCoverActor* Cover)
{
	CurrentCover = Cover;
}

void ATechDemoCharacter::RemoveCover(ATechDemoCoverActor* Cover)
{
	if(CurrentCover == Cover)
	{
		CurrentCover = nullptr;
	}
}

void ATechDemoCharacter::SetSwordAttack(bool attack)
{
	SwordAttack = attack;
	if(!attack)
	{
		MeleeWeapon->WeaponCollision->bGenerateOverlapEvents = false;
	}
}