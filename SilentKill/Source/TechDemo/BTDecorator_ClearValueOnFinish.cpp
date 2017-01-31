// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "BTDecorator_ClearValueOnFinish.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTDecorator_ClearValueOnFinish::UBTDecorator_ClearValueOnFinish(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {
	bNotifyDeactivation = true;

	NodeName = "Clear Value";
}

void UBTDecorator_ClearValueOnFinish::OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult) {
	auto Blackboard = SearchData.OwnerComp.GetBlackboardComponent();
	Blackboard->ClearValue(BlackboardKey.SelectedKeyName);
}

FString UBTDecorator_ClearValueOnFinish::GetStaticDescription() const {
	return FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), *BlackboardKey.SelectedKeyName.ToString());
}
