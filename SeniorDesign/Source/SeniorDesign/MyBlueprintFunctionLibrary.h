// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBlueprintFunctionLibrary.generated.h"

USTRUCT(BlueprintType)
struct FPlantStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "plantProperty")
		FVector2D location;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "plantProperty")
		float seedPerStep;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "plantProperty")
		float growthRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "plantProperty")
		float seedDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "plantProperty")
		float matureSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "plantProperty")
		float killDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "plantProperty")
		float numPoints;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "plantProperty")
		float range;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "plantProperty")
		float radiusInfluence;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "plantProperty")
		float radiusBranch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "plantProperty")
		float trunkHeight;

};

UCLASS()
class SENIORDESIGN_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "plant")
	static TArray<FVector> generateTrunk(FVector plantLocation, float trunkHeight, float trunkradius);

	UFUNCTION(BlueprintCallable, Category = "plant")
		static void spaceColonization(FVector plantLocation, float trunkHeight, float killDistance, float numPoints, float range, float radiusInfluence, float radiusBranch, float randomGen, TArray<FTransform>& outBranches, TArray<FVector>& outLeafLocations, TArray<FTransform>& outTrunklocation);

	UFUNCTION(Category = "plant")
	static TSet<FVector> generateAttractionPoints(FVector crownLocation, float numPoints, float range, float randomGen);

	UFUNCTION(BlueprintCallable, Category = "plant")
	static void simulation(TArray<FVector> inLocations, float randomGen, float initialNumPlants, FVector minXYZ, FVector maxXYZ, int numStep, FPlantStruct prototype, TArray<FPlantStruct>& outPlants);

	UFUNCTION(BlueprintCallable, Category = "plant")
	static void randomPoints(FVector minXYZ, FVector maxXYZ, TArray<FVector>& outLocations, int num);

	UFUNCTION(BlueprintCallable, Category = "plant")
	static void plantInitLocations(int numInitial, TArray<FVector> inLocations, TArray<FVector>& outInitLocations);


	UFUNCTION()
	static TArray<FPlantStruct> recursiveSimulation(FVector minXYZ, FVector maxXYZ, FPlantStruct prototype, FRandomStream RandomNumberGenerator, int step, TArray<FPlantStruct> currentPlants, TArray<FPlantStruct> outPlants);
};
