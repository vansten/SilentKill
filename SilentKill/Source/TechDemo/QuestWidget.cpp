// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "QuestWidget.h"


void UQuestWidget::SetInfo(FString QuestTitle, FString QuestDescription) {
	Title = FText::FromString(QuestTitle);
	Description = FText::FromString(QuestDescription);
}
