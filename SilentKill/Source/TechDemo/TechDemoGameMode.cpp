// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TechDemo.h"
#include "TechDemoGameMode.h"
#include "TechDemoCharacter.h"
#include "TechDemoPlayerController.h"

ATechDemoGameMode::ATechDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Hero/Player_BP"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<AHUD> HUDBPClass(TEXT("/Game/UI/Blueprints/HUD_BP"));
	if(HUDBPClass.Class != nullptr)
	{
		HUDClass = HUDBPClass.Class;
	}

	PlayerControllerClass = ATechDemoPlayerController::StaticClass();
}

void ATechDemoGameMode::BeginPlay()
{
	Super::BeginPlay();

	if(GEngine)
	{
		UGameUserSettings* MyGameSettings = GEngine->GetGameUserSettings();
		MyGameSettings->SetScreenResolution(FIntPoint(1920, 1080));
		MyGameSettings->SetFullscreenMode(EWindowMode::Fullscreen);
		MyGameSettings->SetVSyncEnabled(true);
		MyGameSettings->ApplySettings(false);
	}
}