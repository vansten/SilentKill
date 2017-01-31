#pragma once

#include "ArrowPluginMeshActor.generated.h"

UCLASS()
class ARROWPLUGIN_API AArrowPluginMeshActor : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Visual)
	UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, Category = PhysicalProperties)
	float Reflection;
};