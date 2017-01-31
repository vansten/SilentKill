// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "TechDemoCharacter.h"
#include "TechDemoHUD.h"

void ATechDemoHUD::BeginPlay()
{
	Super::BeginPlay();
	ACharacter* owner = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);;
	if(owner != nullptr)
	{
		Player = Cast<ATechDemoCharacter>(owner);
		if(Player != nullptr)
		{
			WeaponsCount = Player->WeaponsInfo.Num();
			AnglePerWeapon = 360.0f / (float)WeaponsCount;
			InvAnglePerWeapon = 1.0f / AnglePerWeapon;
		}
	}
}

void ATechDemoHUD::DrawHUD()
{
	if(Player != nullptr)
	{
#if 1
		DrawText(Player->GetCurrentStateName(), FLinearColor::Blue, 5.0f, 0.2f);
#endif

		DrawText(FString("Current arrow type: ").Append(FWeaponInfo::GetWeaponTypeName(Player->CurrentWeaponType)), FLinearColor::Blue, 5.0f, 20.0f);
		if(Player->IsInState(EPlayerState::Equipment))
		{
			static FVector2D up = FVector2D(0, 1);
			FVector2D rightStickValue = Player->GetRightStickValue();
			if(rightStickValue.Size() > 0.7f)
			{
				float atan = FMath::Atan2(FVector2D::CrossProduct(rightStickValue, up), FVector2D::DotProduct(rightStickValue, up));
				float angle = FMath::RadiansToDegrees(atan);
				if(angle < 0.0f)
				{
					angle += 360.0f;
				}
				float mult = FMath::RoundToInt(angle * InvAnglePerWeapon);
				float temp = FMath::Abs(angle - mult * AnglePerWeapon);
				if(temp <= 0.35f * AnglePerWeapon)
				{
					int8 selected = ((int8)mult) % WeaponsCount;
					Player->EquipWeapon((EWeaponType)selected);
				}
			}

			float sinTime = (FMath::Sin(GetWorld()->GetRealTimeSeconds() * BlinkingSpeed) + 1.0f) * 0.25f + 0.5f;
			for(auto& weaponsInfo : Player->WeaponsInfo)
			{
				bool selected = Player->CurrentWeaponType == weaponsInfo.WeaponType;
				float scale = selected ? SelectedScale : BaseScale;
				DrawImage(ArcTopPartImage, selected ? weaponsInfo.SelectionColor : FLinearColor::White, (uint8)weaponsInfo.WeaponType, scale);
				DrawImage(ArcBottomPartImage, selected ? FLinearColor::White : weaponsInfo.SelectionColor, (uint8)weaponsInfo.WeaponType, scale);
				DrawImage(weaponsInfo.WeaponImage, selected ? FLinearColor::White : weaponsInfo.SelectionColor, (uint8)weaponsInfo.WeaponType, scale);
			}
		}
	}
}

void ATechDemoHUD::DrawImage(UTexture* Image, FLinearColor TintColor, int32 Index, float scaleMult)
{
	if(Image == nullptr)
	{
		return;
	}

	int32 viewportX;
	int32 viewportY;
	GetOwningPlayerController()->GetViewportSize(viewportX, viewportY);
	const float scale = viewportY / 1080.0f;

	static const FVector RadiusVector = FVector(0, 0, 1);
	const float Radius = BaseRadius * scale;

	FVector RotatedRadiusVector = FRotator(Index * AnglePerWeapon, 0.0f, 0.0f).RotateVector(RadiusVector);
	RotatedRadiusVector.Normalize();

	float width = Image->GetSurfaceWidth() * scale * scaleMult;
	float height = Image->GetSurfaceHeight() * scale * scaleMult;
	float x = viewportX * 0.5f - RotatedRadiusVector.X * Radius - width * 0.5f;
	float y = viewportY * 0.5f - RotatedRadiusVector.Z * Radius - height * 0.5f;

	DrawTexture(
		Image,
		x,
		y,
		width,
		height,
		0.0f, 0.0f,
		1.0f, 1.0f,
		TintColor,
		EBlendMode::BLEND_Translucent,
		1.0f, false,
		Index * AnglePerWeapon, FVector2D(0.5f, 0.5f));
}