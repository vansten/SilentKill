// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "TechDemoGameMode.generated.h"

UCLASS(minimalapi)
class ATechDemoGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATechDemoGameMode();

	virtual void BeginPlay() override;
};
