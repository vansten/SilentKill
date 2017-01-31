// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "TechDemoCoverActor.h"
#include "TechDemoCharacter.h"

ATechDemoCoverActor::ATechDemoCoverActor() : Super()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Cover trigger"));
	LeftCorner = CreateDefaultSubobject<UBoxComponent>(TEXT("Left corner trigger"));
	RightCorner = CreateDefaultSubobject<UBoxComponent>(TEXT("Right corner trigger"));
	FacingDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("Facing direction"));
	CornerBillboard = CreateDefaultSubobject<UMaterialBillboardComponent>(TEXT("Corner billboard"));
	CoverBillboard = CreateDefaultSubobject<UMaterialBillboardComponent>(TEXT("Cover billboard"));

	RootComponent = Trigger;
	LeftCorner->SetupAttachment(Trigger);
	RightCorner->SetupAttachment(Trigger);
	CornerBillboard->SetupAttachment(Trigger);
	CoverBillboard->SetupAttachment(Trigger);
	FacingDirection->SetupAttachment(Trigger);

	CurrentCoverToTake = nullptr;
	CurrentCorner = nullptr;

	Trigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	LeftCorner->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	RightCorner->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ATechDemoCoverActor::BeginPlay()
{
	Super::BeginPlay();
	
	LeftCorner->OnComponentBeginOverlap.AddDynamic(this, &ATechDemoCoverActor::OnCornerBeginOverlap);
	LeftCorner->OnComponentEndOverlap.AddDynamic(this, &ATechDemoCoverActor::OnCornerEndOverlap);
	
	RightCorner->OnComponentBeginOverlap.AddDynamic(this, &ATechDemoCoverActor::OnCornerBeginOverlap);
	RightCorner->OnComponentEndOverlap.AddDynamic(this, &ATechDemoCoverActor::OnCornerEndOverlap);

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ATechDemoCoverActor::OnTriggerBeginOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ATechDemoCoverActor::OnTriggerEndOverlap);

	CoverBillboard->SetHiddenInGame(true);
	CornerBillboard->SetHiddenInGame(true);

	ATechDemoCharacter::OnInputDeviceChanged.AddDynamic(this, &ATechDemoCoverActor::OnInputDeviceChanged);
	OnInputDeviceChanged(true);
}

void ATechDemoCoverActor::OnCornerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATechDemoCharacter* Player = Cast<ATechDemoCharacter>(OtherActor);
	if(Player != nullptr && Player->IsInCover())
	{
		SetCurrentCorner(OverlappedComponent);
	}
}

void ATechDemoCoverActor::OnCornerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	ATechDemoCharacter* Player = Cast<ATechDemoCharacter>(OtherActor);
	if(Player != nullptr)
	{
		CornerBillboard->SetHiddenInGame(true);
		CurrentCoverToTake = nullptr;
		CurrentCorner = nullptr;
	}
}

void ATechDemoCoverActor::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATechDemoCharacter* Player = Cast<ATechDemoCharacter>(OtherActor);
	if(Player != nullptr && !Player->IsInCover())
	{
		if(!Player->IsMovingToNextCover())
		{
			CoverBillboard->SetHiddenInGame(false);
		}
		Player->TakeCover(this);
	}
}

void ATechDemoCoverActor::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	ATechDemoCharacter* Player = Cast<ATechDemoCharacter>(OtherActor);
	if(Player != nullptr)
	{
		CoverBillboard->SetHiddenInGame(true);
		Player->RemoveCover(this);
	}
}

void ATechDemoCoverActor::SetCurrentCorner(UPrimitiveComponent* Corner)
{
	CornerBillboard->AttachToComponent(Corner, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	CornerBillboard->RelativeLocation = FVector::ZeroVector;
	CurrentCorner = Corner;

	if(Corner == LeftCorner && CoverToLeft != nullptr)
	{
		CornerBillboard->SetHiddenInGame(false);
		CurrentCoverToTake = CoverToLeft;
	}
	else if(Corner == RightCorner && CoverToRight != nullptr)
	{
		CornerBillboard->SetHiddenInGame(false);
		CurrentCoverToTake = CoverToRight;
	}
}

void ATechDemoCoverActor::OnInputDeviceChanged(bool bIsGamepadActive)
{
	if(!CornerMaterialInstance)
	{
		CornerMaterialInstance = UMaterialInstanceDynamic::Create(CornerBillboard->GetMaterial(0), nullptr);
		CornerBillboard->SetMaterial(0, CornerMaterialInstance);
	}

	if(CornerMaterialInstance)
	{
		CornerMaterialInstance->SetTextureParameterValue("ButtonTexture", bIsGamepadActive ? CornerGamepadSprite : CornerKeyboardSprite);
	}

	if(!CoverMaterialInstance)
	{
		CoverMaterialInstance = UMaterialInstanceDynamic::Create(CoverBillboard->GetMaterial(0), nullptr);
		CoverBillboard->SetMaterial(0, CoverMaterialInstance);
	}
	if(CoverMaterialInstance)
	{
		CoverMaterialInstance->SetTextureParameterValue("ButtonTexture", bIsGamepadActive ? CoverGamepadSprite : CoverKeyboardSprite);
	}
}

bool ATechDemoCoverActor::TryUseCorner(ATechDemoCharacter* Player, FVector& NearestLocationInNextCover, ATechDemoCoverActor** NextCover)
{
	if(CurrentCoverToTake != nullptr)
	{
		*NextCover = CurrentCoverToTake;
		FVector CornerLocation = CurrentCorner->GetComponentLocation();
		NearestLocationInNextCover = CurrentCoverToTake->GetActorLocation();
		float MinDistance = (NearestLocationInNextCover - CornerLocation).Size();
		FVector CandidateLocation = CurrentCoverToTake->RightCorner->GetComponentLocation();
		float CandidateDistance = (CandidateLocation - CornerLocation).Size();
		if(CandidateDistance < MinDistance)
		{
			NearestLocationInNextCover = CandidateLocation;
			MinDistance = CandidateDistance;
		}
		CandidateLocation = CurrentCoverToTake->LeftCorner->GetComponentLocation();
		CandidateDistance = (CandidateLocation - CornerLocation).Size();
		if(CandidateDistance < MinDistance)
		{
			MinDistance = CandidateDistance;
			NearestLocationInNextCover = CandidateLocation;
		}

		NearestLocationInNextCover.Z = Player->GetActorLocation().Z;

		CornerBillboard->SetHiddenInGame(true);

		return true;
	}

	return false;
}

void ATechDemoCoverActor::TakeCover(ATechDemoCharacter* Player)
{
	CoverBillboard->SetHiddenInGame(true);
	if(LeftCorner->IsOverlappingActor(Player))
	{
		SetCurrentCorner(LeftCorner);
	}
	else if(RightCorner->IsOverlappingActor(Player))
	{
		SetCurrentCorner(RightCorner);
	}
}

void ATechDemoCoverActor::LeaveCover(ATechDemoCharacter* Player)
{
	CoverBillboard->SetHiddenInGame(!Trigger->IsOverlappingActor(Player) || (Player && Player->IsInState(EPlayerState::InCover)));
	CornerBillboard->SetHiddenInGame(true);
}