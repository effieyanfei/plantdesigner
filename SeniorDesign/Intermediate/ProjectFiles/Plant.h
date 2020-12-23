#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

UCLASS()
class SENIORDESIGN_API Plant : public AActor
{
	GENERATED_BODY()

public:
	Plant();

protected:
	int numAttractions;
	int killDistance;

public:
	UFUNCTION(BlueprintCallable)
	void DoSomething();
};