// Copyright by Keyboard Facerollers

#pragma once

#include "Components/StaticMeshComponent.h"
#include "Feather.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class TECHDEMO_API UFeather : public UStaticMeshComponent
{
	GENERATED_BODY()
	
protected:
	TArray<UMaterialInstanceDynamic *>SkillPoints;
	
public:
	// Sets default values for this actor's properties
	//UFeather();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void ShowAvailable(int skillPoints);
	void NotLearnedSelected(int skillPoints, int cost);
	void ResetAll();
};
