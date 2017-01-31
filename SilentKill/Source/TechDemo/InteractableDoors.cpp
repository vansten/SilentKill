// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "InteractableDoors.h"

void AInteractableDoors::CalculateDirection(ATechDemoCharacter* Actor)
{
	FVector dir = this->GetActorLocation() - Actor->GetActorLocation();
	float x = FVector::DotProduct(dir, this->GetActorRightVector());
	if (x > 0)
	{
		OpenDirection = 1;
	}
	else if (x < 0)
	{
		OpenDirection = -1;
	}
	//CloseTriggerBox->SetRelativeLocation(this->GetActorRightVector() * 120.f * SideDirection *OpenDirection, true);
	if (SecondWing != nullptr)
	{
		//SecondWing->CloseTriggerBox->SetRelativeLocation(this->GetActorRightVector() * 120.f * SideDirection * OpenDirection, true);
		SecondWing->OpenDirection = OpenDirection;
	}
}

void AInteractableDoors::OnCloseOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(Use) return;
	Closable = false;
	if(Opened)
	{
		HideBillboard();
	}
	if (SecondWing != nullptr)
		SecondWing->Closable = false;
}

void AInteractableDoors::OnCloseOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Use) return;
	Closable = true;
	ShowBillboard();
	if (SecondWing != nullptr)
		SecondWing->Closable = true;
}

AInteractableDoors::AInteractableDoors(const FObjectInitializer& ObjectInitializer) : AInteractable(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	RotationRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RotationRoot"));
	RotationRoot->SetupAttachment(CloseTriggerBox);
	DoorsMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	DoorsMesh->SetupAttachment(RotationRoot);

	CloseTriggerBox = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("CloseTrigger"));
	CloseTriggerBox->bGenerateOverlapEvents = true;
	CloseTriggerBox->SetupAttachment(TriggerBox);
	CloseTriggerBox->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	CloseTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AInteractableDoors::OnCloseOverlapBegin);
	CloseTriggerBox->OnComponentEndOverlap.AddDynamic(this, &AInteractableDoors::OnCloseOverlapEnd);
}

void AInteractableDoors::BeginPlay()
{
	AInteractable::BeginPlay();
	Opened = false;
	Closable = true;
	SideDirection = Left ? -1 : 1;
}

void AInteractableDoors::Tick(float DeltaSeconds)
{
	AInteractable::Tick(DeltaSeconds);
	if(Use)
	{
		HideBillboard();
		if (Opened && Closable)
		{
			RotationRoot->SetRelativeRotation(FRotator(0.f, SideDirection * OpenDirection * Curve->GetFloatValue(Time), 0.f));
			Time -= DeltaSeconds;
			if (Time <= 0)
			{
				ShowBillboard();
				Use = false;
				Opened = false;
			}
		}
		else if (!Opened)
		{
			RotationRoot->SetRelativeRotation(FRotator(0.f, SideDirection * OpenDirection * Curve->GetFloatValue(Time), 0.f));
			Time += DeltaSeconds;
			if (Time >= 1)
			{
				ShowBillboard();
				Use = false;
				Opened = true;
			}
		}
		else
			Use = false;
	}
}

void AInteractableDoors::OnEnter(ATechDemoCharacter* Actor)
{
	CurrentPlayer = Actor;
}

void AInteractableDoors::OnExit(ATechDemoCharacter* Actor)
{
	CurrentPlayer = nullptr;
	HideBillboard();
}

void AInteractableDoors::Interact(ATechDemoCharacter* Player)
{
	if (!Use)
	{
		Use = true;
		if(Opened)
		{
			Time = 1.0f;
		}
		else
		{
			Time = 0.0f;
			CalculateDirection(Player);
		}
		if (SecondWing != nullptr)
		{
			SecondWing->Interact(Player);
		}

		HideBillboard();
	}
}

void AInteractableDoors::ShowBillboard()
{
	if(CurrentPlayer != nullptr)
	{
		if(Opened && CloseTriggerBox->IsOverlappingActor(CurrentPlayer))
		{
			Billboard->SetHiddenInGame(true);
		}
		else
		{
			ATechDemoCharacter* Player = Cast<ATechDemoCharacter>(CurrentPlayer);
			if(Opened && Player)
			{
				FVector cameraToInteractable = GetActorLocation() - Player->FollowCamera->GetComponentLocation();
				cameraToInteractable.Z = 0.0f;
				cameraToInteractable.Normalize();
				FVector meshToInteractable = GetActorLocation() - Player->GetMesh()->GetComponentLocation();
				meshToInteractable.Z = 0.0f;
				meshToInteractable.Normalize();
				float dot2 = FVector::DotProduct(cameraToInteractable, meshToInteractable);
				Billboard->SetHiddenInGame(dot2 < 0);
			}
			else
			{
				Billboard->SetHiddenInGame(false);
			}
		}
	}
}