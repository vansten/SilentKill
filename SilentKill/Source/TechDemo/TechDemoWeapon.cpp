// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "TechDemoWeapon.h"

FString FWeaponInfo::GetWeaponTypeName(EWeaponType WeaponType)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EWeaponType"), true);
	if(!enumPtr)
	{
		return FString("Invalid");
	}

	return enumPtr->GetEnumName((int32)WeaponType);
}

ATechDemoWeapon::ATechDemoWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	RootScene = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root Component"));
	RootComponent = RootScene;
	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	Mesh->SetupAttachment(RootScene);
}
