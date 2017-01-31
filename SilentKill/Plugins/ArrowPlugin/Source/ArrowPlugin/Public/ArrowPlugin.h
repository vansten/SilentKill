// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleManager.h"

void LogOnScreen(FString msg);

enum class EArrowPathType : uint8
{
	Line,
	Parabola
};

struct FPath
{
protected:
	TArray<FVector> Points;

public:
	class AActor* HitActor;

public:
	void AddPoint(const FVector& Point)
	{
		Points.Add(Point);
	}

	FORCEINLINE const FVector& GetFirst() const
	{
		check(Points.Num() > 0);
		return Points[0];
	}

	FORCEINLINE const FVector& GetLast() const
	{
		check(Points.Num() > 0);
		return Points.Last();
	}

	FORCEINLINE const TArray<FVector>& GetPoints() const
	{
		return Points;
	}
};

class ARROWPLUGIN_API FArrowPluginModule : public IModuleInterface
{
protected:
	class AArrowPluginActor* ArrowPluginActor;
	UWorld* CurrentWorld;
	USceneComponent* Owner;
	FName Socket;
	float SamplesPerUnit;
	bool bWasInitialized;

protected:
	FPath DrawLinePath(const FVector& StartPosition, const FVector& Direction, int32 MaxDepth);
	FPath DrawParabolaPath(const FVector& StartPosition, const FVector& Direction, int32 MaxDepth);

public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void Initialize(float SamplesPerUnit, UWorld* world);
	void SetOwner(USceneComponent* OwnerComponent, FName SocketName = NAME_None);
	FPath DrawPath(const FVector& StartPosition, const FVector& Direction, FLinearColor& ArrowLineColor, int32 MaxDepth = 2, EArrowPathType PathType = EArrowPathType::Line);
	void HideLine();

	inline static FArrowPluginModule* Get()
	{
		static FArrowPluginModule* instance = FModuleManager::LoadModulePtr<FArrowPluginModule>("ArrowPlugin");
		check(instance && "Cannot load ArrowPlugin module.");
		return instance;
	}

	FORCEINLINE bool IsInitialized() const
	{
		return bWasInitialized;
	}
};