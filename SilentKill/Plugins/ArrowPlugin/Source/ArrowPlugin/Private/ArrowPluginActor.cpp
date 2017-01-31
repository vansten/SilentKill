#include "ArrowPluginPrivatePCH.h"
#include "ArrowPluginActor.h"
#include "ArrowPlugin.h"
#include "ProceduralMeshComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

FLineVertexData::FLineVertexData()
{

}

FLineVertexData::FLineVertexData(FVector start, FVector end, float width, bool first)
{
	FVector forward = (end - start);
	float forwardLength = forward.Size();
	forward.Normalize();
	FVector right = FVector::CrossProduct(forward, FVector::UpVector);
	right.Normalize();
	FVector up = FVector::CrossProduct(right, forward);
	up.Normalize();
	const float halfWidth = width * 0.5f;

	int32 indexOffset = 0;
	if(first)
	{
		const float offset = FMath::Min(50.0f, forwardLength * 0.5f);
		indexOffset = 5;
		
		FVector beginStart = start;
		FVector beginEndTop = beginStart + forward * offset + up * halfWidth;
		FVector beginEndTopLeft = beginEndTop - right * halfWidth;
		FVector beginEndTopRight = beginEndTop + right * halfWidth;
		FVector beginEndBottom = beginStart + forward * offset - up * halfWidth;
		FVector beginEndBottomLeft = beginEndBottom - right * halfWidth;
		FVector beginEndBottomRight = beginEndBottom + right * halfWidth;

		Vertices.Add(beginStart);
		Vertices.Add(beginEndTopLeft);
		Vertices.Add(beginEndTopRight);
		Vertices.Add(beginEndBottomLeft);
		Vertices.Add(beginEndBottomRight);

		// Top face
		Indices.Add(0);
		Indices.Add(1);
		Indices.Add(2);

		// Bottom face
		Indices.Add(0);
		Indices.Add(3);
		Indices.Add(4);

		// Left face
		Indices.Add(0);
		Indices.Add(1);
		Indices.Add(3);
		
		// Right face
		Indices.Add(0);
		Indices.Add(2);
		Indices.Add(4);
		
		UVs.Add(FVector2D(0.5f, 0));
		UVs.Add(FVector2D(0, 1));
		UVs.Add(FVector2D(1, 1));
		UVs.Add(FVector2D(0, 1));
		UVs.Add(FVector2D(1, 1));

		start += forward * offset;
	}

	FVector startTop = start + up * halfWidth;
	FVector startBottom = start - up * halfWidth;
	FVector endTop = end + up * halfWidth;
	FVector endBottom = end - up * halfWidth;

	FVector startTopLeft = startTop - right * halfWidth;
	FVector startTopRight = startTop + right * halfWidth;
	FVector endTopLeft = endTop - right * halfWidth;
	FVector endTopRight = endTop + right * halfWidth;
	FVector startBottomLeft = startBottom - right * halfWidth;
	FVector startBottomRight = startBottom + right * halfWidth;
	FVector endBottomLeft = endBottom - right * halfWidth;
	FVector endBottomRight = endBottom + right * halfWidth;

	Vertices.Add(startTopLeft);
	Vertices.Add(startTopRight);
	Vertices.Add(endTopLeft);
	Vertices.Add(endTopRight);
	Vertices.Add(startBottomLeft);
	Vertices.Add(startBottomRight);
	Vertices.Add(endBottomLeft);
	Vertices.Add(endBottomRight);

	// Top face
	Indices.Add(0 + indexOffset);
	Indices.Add(1 + indexOffset);
	Indices.Add(2 + indexOffset);
	Indices.Add(1 + indexOffset);
	Indices.Add(2 + indexOffset);
	Indices.Add(3 + indexOffset);

	// Bottom face
	Indices.Add(4 + indexOffset);
	Indices.Add(5 + indexOffset);
	Indices.Add(6 + indexOffset);
	Indices.Add(5 + indexOffset);
	Indices.Add(6 + indexOffset);
	Indices.Add(7 + indexOffset);

	// Right face
	Indices.Add(1 + indexOffset);
	Indices.Add(3 + indexOffset);
	Indices.Add(5 + indexOffset);
	Indices.Add(3 + indexOffset);
	Indices.Add(5 + indexOffset);
	Indices.Add(7 + indexOffset);

	// Left face
	Indices.Add(0 + indexOffset);
	Indices.Add(2 + indexOffset);
	Indices.Add(4 + indexOffset);
	Indices.Add(2 + indexOffset);
	Indices.Add(4 + indexOffset);
	Indices.Add(6 + indexOffset);

	UVs.Add(FVector2D(0, 0));
	UVs.Add(FVector2D(1, 0));
	UVs.Add(FVector2D(0, 1));
	UVs.Add(FVector2D(1, 1));
	UVs.Add(FVector2D(0, 0));
	UVs.Add(FVector2D(1, 0));
	UVs.Add(FVector2D(0, 1));
	UVs.Add(FVector2D(1, 1));
}

AArrowPluginActor::AArrowPluginActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RootSceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	RootComponent = RootSceneComponent;

	LineProceduralMesh = ObjectInitializer.CreateDefaultSubobject<UProceduralMeshComponent>(this, TEXT("Line mesh 1"));
	LineProceduralMesh->SetupAttachment(RootComponent);
	LineProceduralMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	LineWidth = 10.0f;

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> LineMaterialInstanceFinder(TEXT("MaterialInstanceConstant'/ArrowPlugin/Line_MaterialInstance.Line_MaterialInstance'"));
	if(LineMaterialInstanceFinder.Succeeded())
	{
		LineMaterial = LineMaterialInstanceFinder.Object;
	}
}

void AArrowPluginActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UMaterialInstanceDynamic* temp = UMaterialInstanceDynamic::Create(LineMaterial, nullptr);
	LineProceduralMesh->SetMaterial(0, temp);
}

void AArrowPluginActor::SetInfo(const FPath& Path, FLinearColor& Color)
{
	UMaterialInstanceDynamic* tempMaterial = Cast<UMaterialInstanceDynamic>(LineProceduralMesh->GetMaterial(0));
	if(tempMaterial != nullptr)
	{
		tempMaterial->SetVectorParameterValue("Color", Color);
	}

	TArray<FVector> points = Path.GetPoints();
	int numEl = points.Num();
	if(numEl > 1)
	{
		LineProceduralMesh->ClearAllMeshSections();
		LineProceduralMesh->SetWorldLocation(FVector::ZeroVector);
		for(int i = 0; i < numEl - 1; ++i)
		{
			FLineVertexData data(points[i], points[i + 1], LineWidth, i == 0);
			LineProceduralMesh->CreateMeshSection(i, data.GetVertices(), data.GetIndices(), TArray<FVector>(), data.GetUVs(), TArray<FColor>(), TArray<FProcMeshTangent>(), false);
			LineProceduralMesh->SetMaterial(i, tempMaterial);
		}
	}
}