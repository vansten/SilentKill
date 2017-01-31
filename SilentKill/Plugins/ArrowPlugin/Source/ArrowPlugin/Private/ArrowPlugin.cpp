// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ArrowPluginPrivatePCH.h"
#include "ArrowPluginActor.h"
#include "ArrowPluginMeshActor.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectGlobals.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"

#define LOCTEXT_NAMESPACE "FArrowPluginModule"

void LogOnScreen(FString msg)
{
#if !UE_BUILD_SHIPPING
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(12345, 2.0f, FColor::Blue, FString("ArrowPlugin: ").Append(msg));
	}
#endif
}

FPath FArrowPluginModule::DrawLinePath(const FVector& StartPosition, const FVector& Direction, int32 MaxDepth)
{
	static const float maxLineLength = 10000.0f;

	FPath path;
	path.AddPoint(StartPosition);

	FVector dir = Direction;
	dir.Normalize(); 
	//CurrentWorld->DebugDrawTraceTag = FName("ArrowTrace");
	FVector startPosition = StartPosition;
	FVector endPosition = StartPosition + dir * maxLineLength;
	FCollisionQueryParams queryParams = FCollisionQueryParams::DefaultQueryParam;
	queryParams.TraceTag = FName("ArrowTrace");
	queryParams.AddIgnoredActor(Owner->GetOwner());
	FString actorsHit = "Arrowhead";
	while(MaxDepth > 0)
	{
		path.HitActor = nullptr;
		if(CurrentWorld)
		{
			FHitResult hitInfo(ForceInit);
			bool block = CurrentWorld->LineTraceSingleByChannel(hitInfo, startPosition, endPosition, ECollisionChannel::ECC_MAX, queryParams);
			if(block)
			{
				path.HitActor = hitInfo.GetActor();
				AArrowPluginMeshActor* meshActor = Cast<AArrowPluginMeshActor>(hitInfo.GetActor());
				path.AddPoint(hitInfo.ImpactPoint);
				if(meshActor != nullptr && meshActor->Reflection > 0.0f)
				{
					actorsHit.Append("--->");
					actorsHit.Append(meshActor->GetName());
					queryParams.AddIgnoredActor(meshActor);
					startPosition = hitInfo.ImpactPoint;
					dir = FMath::GetReflectionVector(dir, hitInfo.ImpactNormal).GetSafeNormal();
					endPosition = startPosition + dir * maxLineLength * meshActor->Reflection;
				}
				else
				{
					MaxDepth = 0;
				}
			}
			else
			{
				path.AddPoint(startPosition + dir * maxLineLength * 0.2f);
				break;
			}
		}
		--MaxDepth;
	}

	LogOnScreen(actorsHit);
	
	return path;
}

FPath FArrowPluginModule::DrawParabolaPath(const FVector& StartPosition, const FVector& Direction, int32 MaxDepth)
{
	FPath path;

	return path;
}

void FArrowPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	bWasInitialized = false;
}

void FArrowPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	bWasInitialized = false;
}

void FArrowPluginModule::Initialize(float SamplesPU, UWorld* world)
{
	CurrentWorld = world;
	SamplesPerUnit = SamplesPU; 
	ArrowPluginActor = nullptr;
	for(TActorIterator<AArrowPluginActor> ActorItr(CurrentWorld); ActorItr; ++ActorItr)
	{
		if(ArrowPluginActor == nullptr)
		{
			ArrowPluginActor = (*ActorItr);
		}
		else
		{
			(*ActorItr)->Destroy();
		}
	}

	if(ArrowPluginActor == nullptr)
	{
		ArrowPluginActor = (AArrowPluginActor*)world->SpawnActor(AArrowPluginActor::StaticClass());
	}

	bWasInitialized = ArrowPluginActor != nullptr;
	if(bWasInitialized)
	{
		ArrowPluginActor->SetActorLocation(FVector::ZeroVector);
		ArrowPluginActor->SetActorRotation(FRotator::ZeroRotator);
		ArrowPluginActor->SetActorHiddenInGame(true);
	}
	else
	{
		UE_LOG(LogActor, Error, TEXT("ArrowPluginActor instance was not found"));
	}
}

void FArrowPluginModule::SetOwner(USceneComponent* OwnerComponent, FName SocketName)
{
	Owner = OwnerComponent;
	Socket = SocketName;
}

FPath FArrowPluginModule::DrawPath(const FVector& StartPosition, const FVector& Direction, FLinearColor& ArrowLineColor, int32 MaxDepth, EArrowPathType PathType)
{
	if(!bWasInitialized)
	{
		return FPath();
	}
	ArrowPluginActor->SetActorHiddenInGame(false);
	FPath path;
	if(PathType == EArrowPathType::Line)
	{
		path = DrawLinePath(StartPosition, Direction, MaxDepth);
	}
	else
	{
		path = DrawParabolaPath(StartPosition, Direction, MaxDepth);
	}
	ArrowPluginActor->SetInfo(path, ArrowLineColor);

	return path;
}

void FArrowPluginModule::HideLine()
{
	if(bWasInitialized)
	{
		ArrowPluginActor->SetActorHiddenInGame(true);
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FArrowPluginModule, ArrowPlugin)
