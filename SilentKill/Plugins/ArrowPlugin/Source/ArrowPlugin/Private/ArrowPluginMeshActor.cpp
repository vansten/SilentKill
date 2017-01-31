#include "ArrowPluginPrivatePCH.h"
#include "ArrowPluginMeshActor.h"

AArrowPluginMeshActor::AArrowPluginMeshActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	RootComponent = Mesh;

	Reflection = 0.5f;
}