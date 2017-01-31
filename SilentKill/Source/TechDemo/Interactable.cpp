// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "Interactable.h"


// Sets default values
AInteractable::AInteractable(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("Trigger"));
	TriggerBox->bGenerateOverlapEvents = true;
	RootComponent = TriggerBox;
	TriggerBox->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AInteractable::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AInteractable::OnOverlapEnd);

	Billboard = CreateDefaultSubobject<UMaterialBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();

	Billboard->SetHiddenInGame(true);
	ATechDemoCharacter::OnInputDeviceChanged.AddDynamic(this, &AInteractable::OnInputDeviceChanged);
	OnInputDeviceChanged(true);
}

// Called every frame
void AInteractable::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AInteractable::Interact(ATechDemoCharacter* Player)
{
}

void AInteractable::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ATechDemoCharacter* actor = Cast<ATechDemoCharacter>(OtherActor);
	if (actor != nullptr)
	{
		actor->AddInteractableObject(this);
		OnEnter(actor);
	}
}

void AInteractable::OnEnter(ATechDemoCharacter * Actor)
{
}

void AInteractable::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ATechDemoCharacter* actor = Cast<ATechDemoCharacter>(OtherActor);
	if (actor != nullptr)
	{
		actor->RemoveInteractableObject(this);
		OnExit(actor);
	}
}

void AInteractable::OnExit(ATechDemoCharacter * Actor)
{
	//LogOnScreen(-1, 2.f, FColor::Yellow, FString("Base exit"));
}

void AInteractable::OnInputDeviceChanged(bool bIsGamepadActive)
{
	if (!BillboardMaterialInstance)
	{
		BillboardMaterialInstance = UMaterialInstanceDynamic::Create(Billboard->GetMaterial(0), nullptr);
		Billboard->SetMaterial(0, BillboardMaterialInstance);
	}

	if (BillboardMaterialInstance && BillboardMaterialInstance->IsValidLowLevelFast())
	{
		BillboardMaterialInstance->SetTextureParameterValue("ButtonTexture", bIsGamepadActive ? InteractableGamepadSprite : InteractableKeyboardSprite);
	}
}

void AInteractable::ShowBillboard()
{
	Billboard->SetHiddenInGame(false);
}

void AInteractable::HideBillboard()
{
	Billboard->SetHiddenInGame(true);
}