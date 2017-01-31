// Copyright by Keyboard Facerollers

#pragma once

#include "TechDemoArrow.h"
#include "TechDemoArrowSplatering.generated.h"

/**
 * 
 */
UCLASS()
class TECHDEMO_API ATechDemoArrowSplatering : public ATechDemoArrow
{
	GENERATED_UCLASS_BODY()

protected:
	bool bWasHit;

public:
	UPROPERTY(EditAnywhere, Category = Subarrows)
	UClass* ArrowsAfterHitClass;

	UPROPERTY(EditAnywhere, Category = Subarrows)
	int32 ArrowsAfterHitNum;
	
protected:
	virtual void OnHit(const FVector& reflectedDirection) override;
	virtual void OnDestroy() override;
};
