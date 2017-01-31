// Copyright by Keyboard Facerollers

#include "TechDemo.h"
#include "EnvQueryGenerator_GetAllSentries.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"


static UEnum* TextOpEnum = nullptr;
UEnvQueryGenerator_GetAllSentries::UEnvQueryGenerator_GetAllSentries(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {
	ItemType = UEnvQueryItemType_Actor::StaticClass();
	SearchedActorClass = ASentryCharacter::StaticClass();
	SearchCenter = UEnvQueryContext_Querier::StaticClass();
	SearchRadius.DefaultValue = 2500.f;
	BehaviorType = ESentryBehaviorType::Observation;
	TextOpEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("ESentryBehaviorType"));
}

void UEnvQueryGenerator_GetAllSentries::GenerateItems(FEnvQueryInstance& QueryInstance) const {
	auto QueryOwner = QueryInstance.Owner.Get();
	if (QueryOwner == nullptr) {
		return;
	}

	auto World = GEngine->GetWorldFromContextObject(QueryOwner);
	if (World == nullptr || SearchedActorClass == nullptr) {
		return;
	}

	SearchRadius.BindData(QueryOwner, QueryInstance.QueryID);
	const auto RadiusValue = SearchRadius.GetValue(),
		RadiusSq = FMath::Square(RadiusValue);

	TArray<FVector> ContextLocations;
	QueryInstance.PrepareContext(SearchCenter, ContextLocations);

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(World, SearchedActorClass, Actors);
	for (auto Actor : Actors) {
		auto Sentry = Cast<ASentryCharacter>(Actor);
		if (Sentry->IsAlive() && Sentry->BehaviorType == BehaviorType) {
			for (int32 ContextIndex = 0; ContextIndex < ContextLocations.Num(); ++ContextIndex) {
				if (FVector::DistSquared(ContextLocations[ContextIndex], Sentry->GetActorLocation()) < RadiusSq) {
					QueryInstance.AddItemData<UEnvQueryItemType_Actor>(Actor);
					break;
				}
			}
		}
	}
}

FText UEnvQueryGenerator_GetAllSentries::GetDescriptionTitle() const {
	FFormatNamedArguments Args;
	Args.Add(TEXT("DescriptionTitle"), Super::GetDescriptionTitle());
	Args.Add(TEXT("ActorsClass"), FText::FromString(GetNameSafe(SearchedActorClass)));
	Args.Add(TEXT("DescribeContext"), UEnvQueryTypes::DescribeContext(SearchCenter));

	return FText::Format(FTextFormat::FromString("{DescriptionTitle}: generate set of actors of {ActorsClass} around {DescribeContext}"), Args);
}

FText UEnvQueryGenerator_GetAllSentries::GetDescriptionDetails() const {
	FFormatNamedArguments Args;
	Args.Add(TEXT("Radius"), FText::FromString(SearchRadius.ToString()));
	Args.Add(TEXT("Behavior"), FText::FromString(TextOpEnum->GetEnumName(int32(BehaviorType))));

	auto Desc = FText::Format(FTextFormat::FromString("radius: {Radius}, behavior: {Behavior}"), Args);

	return Desc;
}
