// Copyright by Keyboard Facerollers

#pragma once

#include "EnvironmentQuery/EnvQueryGenerator.h"
#include "SentryCharacter.h"
#include "EnvQueryGenerator_GetAllSentries.generated.h"

/**
 *
 */
UCLASS()
class TECHDEMO_API UEnvQueryGenerator_GetAllSentries : public UEnvQueryGenerator
{
	GENERATED_BODY()

private:
	UEnvQueryGenerator_GetAllSentries(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly)
		FAIDataProviderFloatValue SearchRadius;
	UPROPERTY(EditDefaultsOnly)
		ESentryBehaviorType BehaviorType;

	void GenerateItems(FEnvQueryInstance& QueryInstance) const override;
	FText GetDescriptionTitle() const override;
	FText GetDescriptionDetails() const override;

	TSubclassOf<UEnvQueryContext> SearchCenter;
	TSubclassOf<AActor> SearchedActorClass;
};
