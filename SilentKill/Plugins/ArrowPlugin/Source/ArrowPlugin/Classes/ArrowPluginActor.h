#pragma once

#include "ArrowPluginActor.generated.h"

USTRUCT()
struct FLineVertexData
{
	GENERATED_USTRUCT_BODY()

protected:
	TArray<FVector> Vertices;
	TArray<int32> Indices;
	TArray<FVector2D> UVs;

public:
	FLineVertexData();
	FLineVertexData(FVector start, FVector end, float width, bool first = false);

	FORCEINLINE const TArray<FVector>& GetVertices() const
	{
		return Vertices;
	}

	FORCEINLINE const TArray<int32>& GetIndices() const
	{
		return Indices;
	}

	FORCEINLINE const TArray<FVector2D>& GetUVs() const
	{
		return UVs;
	}
};

UCLASS()
class ARROWPLUGIN_API AArrowPluginActor : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Components)
	class USceneComponent* RootSceneComponent;
	UPROPERTY(EditAnywhere, Category = Line)
	class UMaterialInstance* LineMaterial;
	UPROPERTY(EditAnywhere, Category = Line)
	class UProceduralMeshComponent* LineProceduralMesh;
	UPROPERTY(EditAnywhere, Category = Line)
	float LineWidth;

public:
	virtual void PostInitializeComponents() override;

	void SetInfo(const FPath& Path, FLinearColor& Color);
};