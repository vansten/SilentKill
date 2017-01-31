// Copyright by Keyboard Facerollers

#pragma once

#include "Blueprint/UserWidget.h"
#include "QuestWidget.generated.h"

/**
 *
 */
UCLASS()
class TECHDEMO_API UQuestWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
		FText Title;
	UPROPERTY(BlueprintReadOnly)
		FText Description;

public:
	void SetInfo(FString QuestTitle, FString QuestDescription);
};
