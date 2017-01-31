// Copyright by Keyboard Facerollers

#pragma once

#include "GameFramework/PlayerController.h"
#include "QuestOverlay.h"
#include "QuestWidget.h"
#include "TechDemoPlayerController.generated.h"

/**
 *
 */
UCLASS()
class TECHDEMO_API ATechDemoPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	ATechDemoPlayerController();

	void Possess(APawn* InPawn) override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		TSubclassOf<class UQuestWidget> Quest;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
		TSubclassOf<class UQuestOverlay> Overlay;

	UQuestWidget* QuestRefference;
	UQuestOverlay* OverlayRefference;

	bool bIsQuestWidgetShown;

public:
	void DisplayQuest(FString Title, FString Description);

	FORCEINLINE bool IsQuestWidgetShown() const
	{
		return bIsQuestWidgetShown;
	}

	UFUNCTION(BlueprintCallable, Category = Quests)
	void SetQuestWidgetShown(bool bQuestWidgetShown)
	{
		bIsQuestWidgetShown = bQuestWidgetShown;
	}
};
