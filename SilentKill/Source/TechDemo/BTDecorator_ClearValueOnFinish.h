// Copyright by Keyboard Facerollers

#pragma once

#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_ClearValueOnFinish.generated.h"

/**
 *
 */
UCLASS()
class TECHDEMO_API UBTDecorator_ClearValueOnFinish : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

private:
	UBTDecorator_ClearValueOnFinish(const FObjectInitializer& ObjectInitializer);

	void OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult) override;
	FString GetStaticDescription() const override;
};
