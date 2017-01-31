// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TechDemo.h"


IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, TechDemo, "TechDemo" );
 
void LogOnScreen(int32 key, float duration, const FColor& color, const FString& msg)
{
#if !UE_BUILD_SHIPPING
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(key, duration, color, msg);
	}
#endif
}