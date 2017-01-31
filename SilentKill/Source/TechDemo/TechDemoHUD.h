// Copyright by Keyboard Facerollers

#pragma once

#include "GameFramework/HUD.h"
#include "TechDemoHUD.generated.h"

UCLASS()
class TECHDEMO_API ATechDemoHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	class ATechDemoCharacter* Player;
	float AnglePerWeapon;
	float InvAnglePerWeapon;
	int32 WeaponsCount;

public:
	UPROPERTY(EditAnywhere, Category = WeaponsUI)
	UTexture* ArcTopPartImage;

	UPROPERTY(EditAnywhere, Category = WeaponsUI)
	UTexture* ArcBottomPartImage;

	UPROPERTY(EditAnywhere, Category = WeaponsUI)
	float BaseRadius;

	UPROPERTY(EditAnywhere, Category = WeaponsUI)
	float BaseScale;

	UPROPERTY(EditAnywhere, Category = WeaponsUI)
	float SelectedScale;

	UPROPERTY(EditAnywhere, Category = WeaponsUI)
	float BlinkingSpeed;

protected:
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

	void DrawImage(UTexture* Image, FLinearColor TintColor, int32 Index, float scaleMult = 1.0f);
};
