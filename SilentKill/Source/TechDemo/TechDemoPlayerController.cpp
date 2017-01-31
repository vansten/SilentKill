// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "TechDemoPlayerController.h"


ATechDemoPlayerController::ATechDemoPlayerController() {
	static ConstructorHelpers::FClassFinder<UQuestWidget> WidgetClass(TEXT("/Game/UI/Quest/MainQuest"));
	if (WidgetClass.Class) {
		Quest = WidgetClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UQuestOverlay> OverlayClass(TEXT("/Game/UI/Quest/QuestOverlay"));
	if (OverlayClass.Class) {
		Overlay = OverlayClass.Class;
	}
}

void ATechDemoPlayerController::Possess(APawn* InPawn) {
	Super::Possess(InPawn);

	if (Overlay) {
		OverlayRefference = CreateWidget<UQuestOverlay>(this, Overlay);
		OverlayRefference->AddToViewport();
	}
}

void ATechDemoPlayerController::DisplayQuest(FString Title, FString Description) {
	if (Quest) {
		GetCharacter()->GetCharacterMovement()->DisableMovement();

		if (!QuestRefference) {
			QuestRefference = CreateWidget<UQuestWidget>(this, Quest);
		}

		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(QuestRefference->GetCachedWidget());
		SetInputMode(InputMode);

		QuestRefference->SetInfo(Title, Description);
		QuestRefference->AddToViewport();
		//bShowMouseCursor = true;
		bIsQuestWidgetShown = true;
		OverlayRefference->AddQuestToUI(FText::FromString(Title));
	}
}
