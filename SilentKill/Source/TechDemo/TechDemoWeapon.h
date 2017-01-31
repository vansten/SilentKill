// Copyright by Keyboard Facerollers

#pragma once

#include "GameFramework/Actor.h"
#include "TechDemoWeapon.generated.h"

UENUM()
enum class EWeaponType : uint8
{
	Melee,
	Bow_Exploding,
	Bow_Splatering,
	Bow_Sleepy,
	Bow_Normal
};

USTRUCT()
struct FWeaponInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Weapons)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = Weapons)
	TSubclassOf<class ATechDemoWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = WeaponsUI)
	FLinearColor SelectionColor;

	UPROPERTY(EditAnywhere, Category = WeaponsUI)
	UTexture* WeaponImage;

	UPROPERTY(EditAnywhere, Category = Weapons)
	class UStaticMesh* WeaponMesh;

	static FString GetWeaponTypeName(EWeaponType WeaponType);
};

UCLASS()
class TECHDEMO_API ATechDemoWeapon : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Weapons)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = Components)
	USceneComponent* RootScene;

	UPROPERTY(EditAnywhere, Category = Visual)
	UStaticMeshComponent* Mesh;
};
