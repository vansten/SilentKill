// Copyright by Keyboard Facerollers

#pragma once

#include "Blueprint/UserWidget.h"
#include "QuestOverlay.generated.h"

/**
 *
 */
UCLASS()
class TECHDEMO_API UQuestOverlay : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
		void AddQuestToUI();

	UPROPERTY(BlueprintReadOnly)
		FText QuestToAdd;

public:
	void AddQuestToUI(FText QuestTitle);

};
