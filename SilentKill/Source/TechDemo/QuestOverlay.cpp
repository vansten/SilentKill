// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "QuestOverlay.h"


void UQuestOverlay::AddQuestToUI(FText QuestTitle) {
	QuestToAdd = QuestTitle;
	AddQuestToUI();
}
