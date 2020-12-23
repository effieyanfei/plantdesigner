// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"
#include "Math/RandomStream.h"
#include "Components/StaticMeshComponent.h"

void UMyBlueprintFunctionLibrary::randomPoints(FVector minXYZ, FVector maxXYZ, TArray<FVector>& outLocations, int num) {
	int stepX = (maxXYZ.X - minXYZ.X) / num;
	int stepY = (maxXYZ.Y - minXYZ.Y) / num;
	
	for (int i = 0; i < num; i++) {
		for (int j = 0; j < num; j++) {
			FVector location = FVector(minXYZ.X + i * stepX, minXYZ.Y + j * stepY, 0);
			outLocations.Add(location);
		}
	}
}

void UMyBlueprintFunctionLibrary::plantInitLocations(int numInitial, TArray<FVector> inLocations, TArray<FVector>& outInitLocations) {
	FRandomStream RandomNumberGenerator;
	RandomNumberGenerator.Initialize(1234);

	for (int i = 0; i < numInitial; i++) {
		int index = RandomNumberGenerator.FRandRange(0, inLocations.Num() - 1);
		outInitLocations.Add(inLocations[index]);
	}
}


TSet<FVector> UMyBlueprintFunctionLibrary::generateAttractionPoints(FVector crownLocation, float numPoints, float range, float randomGen) {
	//generate an array of random points
	FRandomStream RandomNumberGenerator;
	RandomNumberGenerator.Initialize(randomGen);
	TSet<FVector> attractionPoints;
	for (int i = 0; i < numPoints; i++) {
		float x = RandomNumberGenerator.FRandRange(-1.0f * range, range) + crownLocation.X;
		float y = RandomNumberGenerator.FRandRange(-1.0f * range, range) + crownLocation.Y;
		float z = RandomNumberGenerator.FRandRange(-1.0f * range * 0.5, range * 0.5) + crownLocation.Z + range;
		FVector newPoint = FVector(x, y, z);
		attractionPoints.Add(newPoint);
	}
	return attractionPoints;
}

TArray<FPlantStruct> UMyBlueprintFunctionLibrary::recursiveSimulation(FVector minXYZ, FVector maxXYZ, FPlantStruct prototype, FRandomStream RandomNumberGenerator, int step, TArray<FPlantStruct> currentPlants, TArray<FPlantStruct> outPlants)
{
	if (step <= 0) { return outPlants; } //basecase
	TArray<FPlantStruct> nextPlants;
	float ratio = prototype.growthRate * 0.01 * step;
	ratio = FMath::Min(1.f, ratio);
	float newNumAttracts = prototype.numPoints * ratio;
	float newRange = prototype.range * ratio;
	float newTrunkHeight = prototype.trunkHeight * ratio;
	for (FPlantStruct p : currentPlants) {
		if(ratio * 100 >= prototype.matureSize) {
			for (int i = 0; i < prototype.seedPerStep; i++) {
				FPlantStruct thisPlant = prototype;
				float angle = FMath::Rand() * 360;
				float x = cos(angle) * prototype.seedDistance;
				float y = sin(angle) * prototype.seedDistance;
				FVector2D newPoint = FVector2D(x, y) + p.location;
				if (newPoint.X > minXYZ.X && newPoint.Y > minXYZ.Y && newPoint.X < maxXYZ.X && newPoint.Y < maxXYZ.Y) {
					bool collision = false;
					for (FPlantStruct existP : outPlants) {
						if (FVector2D::Distance(existP.location,newPoint) < prototype.range) {
							collision = true;
						}
						if (collision) {
							break;
						}
					}
					if (!collision) {
						thisPlant.location = newPoint;
						thisPlant.range = newRange;
						thisPlant.numPoints = newNumAttracts;
						thisPlant.trunkHeight = newTrunkHeight;
						nextPlants.Add(thisPlant);
						outPlants.Add(thisPlant);
					}
				}
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("ratio not qualify for reproduction"));
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("num of plants spawning: %f"), (float)nextPlants.Num());
	return recursiveSimulation(minXYZ, maxXYZ, prototype, RandomNumberGenerator, step - 1, nextPlants, outPlants);
}

void UMyBlueprintFunctionLibrary::simulation(TArray<FVector> inLocations, float randomGEn, float initialNumPlants, FVector minXYZ, FVector maxXYZ, int numStep, FPlantStruct prototype, TArray<FPlantStruct>& outPlants)
{

	FRandomStream RandomNumberGenerator;
	RandomNumberGenerator.Initialize(1234);
	TArray<FPlantStruct> initialPlants;
	float ratio = prototype.growthRate * 0.01 * numStep;
	ratio = FMath::Min(1.f, ratio);
	float newNumAttracts = prototype.numPoints * ratio;
	float newRange = prototype.range * ratio;
	float newTrunkHeight = prototype.trunkHeight * ratio;

	for (int i = 0; i < initialNumPlants; i++) {
		FVector2D newLocation(inLocations[i].X, inLocations[i].Y);
		prototype.location = newLocation;
		prototype.numPoints = newNumAttracts;
		prototype.range = newRange;
		prototype.trunkHeight = newTrunkHeight;
		initialPlants.Add(prototype);
	}
	
	//outPlants = initialPlants;
	outPlants = recursiveSimulation(minXYZ, maxXYZ, prototype, RandomNumberGenerator, numStep - 1, initialPlants, initialPlants);
}

TArray<FVector> UMyBlueprintFunctionLibrary::generateTrunk(FVector plantLocation, float trunkHeight, float trunkradius)
{
	TArray<FVector> trunkNodes;
	float num = trunkHeight / trunkradius;
	for (int i = 0; i < num; i++) {
		trunkNodes.Add(plantLocation + trunkradius * FVector(0.f, 0.f, i));
	}
	return trunkNodes;
}

void UMyBlueprintFunctionLibrary::spaceColonization(FVector plantLocation, float trunkHeight, float killDistance, float numPoints, float range, float radiusInfluence, float radiusBranch, float randomGen, TArray<FTransform>& outBranches, TArray<FVector>& outLeafLocations, TArray<FTransform>& outTrunklocation)
{
	FVector crownLocation = plantLocation + FVector(0, 0, trunkHeight - radiusBranch * (int) (trunkHeight / radiusBranch));
	TSet<FVector> attractionPoints = generateAttractionPoints(crownLocation, numPoints, range, randomGen);
	TArray<bool> isLeaf;
	TArray<FVector> branchNodes;
	TArray<FTransform> branchTransforms;
	
	branchNodes.Add(crownLocation);
	isLeaf.Add(true);
	bool stopIteration = false;
	int step = 0;
	int influencingAttractions = 1;
	FVector scale = FVector(1.f, 1.f, 1.f);
	TArray<FVector> trunk = generateTrunk(plantLocation, trunkHeight, radiusBranch);
	for (int i = 0; i < trunk.Num() - 1; i++) {
		FVector t = trunk[i];
		FTransform trunkT = FTransform(FQuat::Identity, t, scale);
		scale = FVector(scale.X * 0.8, scale.Y * 0.8, 1.f);
		outTrunklocation.Add(trunkT);
	}
	


	while (attractionPoints.Num() > 0 && influencingAttractions > 0 && step < 50) {
		influencingAttractions = 0;
		TSet<FVector> toAdd;
		TSet<FVector> toRemove;
		TArray<FVector> influence;
		influence.Init(FVector(0.f, 0.f, 0.f), branchNodes.Num());

		for (FVector attraction : attractionPoints) {
			FVector closest = branchNodes[0];
			int closestIndex = 0;

			for (int i = 0; i < branchNodes.Num(); i++) {
				//find attraction within kill distance
				if (FVector::Distance(branchNodes[i], attraction) < killDistance) {
					toRemove.Add(attraction);
				}
				//find closest branch
				if (FVector::Distance(branchNodes[i], attraction) < FVector::Distance(attraction, closest) &&
					FVector::Distance(branchNodes[i], attraction) > killDistance) {
					closest = branchNodes[i];
					closestIndex = i;
				}
			}
			//accumulate influence for each branch
			if (FVector::Distance(attraction, closest) < radiusInfluence && FVector::Distance(attraction, closest) > killDistance) {
				influencingAttractions++;
				influence[closestIndex] = influence[closestIndex] + FVector(attraction - closest);
			}
		}
		//find all new branches
		for (int i = 0; i < branchNodes.Num(); i++) {
			if (influence[i].Size() != 0) {
				
				FVector branchDir = influence[i] / influence[i].Size();
				FVector newBranch = branchNodes[i] + radiusBranch * branchDir;
				isLeaf[i] = false;
				toAdd.Add(newBranch);

				//calculate rotation
				FTransform originT = FTransform(FQuat::Identity, newBranch, FVector(1, 1, 1));
				FVector origin = originT.TransformFVector4(FVector4(0, 0, 1, 0));
				origin = origin / origin.Size(); //normalize
				FQuat quat = FQuat::FindBetween(origin, -1 * branchDir);
				FTransform branchT = FTransform(quat, newBranch, scale);
				branchTransforms.Add(branchT);
			}
		}

		scale = FVector(scale.X * 0.9, scale.Y * 0.9, 1.f);

		//update branchNodes
		for (FVector newBranch : toAdd) {
			branchNodes.Add(newBranch);
			isLeaf.Add(true);
		}

		//update attraction points
		for (FVector invalid : toRemove) {
			attractionPoints.Remove(invalid);
		}
		
		step++;
	}

	TArray<FVector> leaves;
	for (int i = 0; i < branchNodes.Num(); i++) {
		if (isLeaf[i]) {
			leaves.Add(branchNodes[i]);
		}
	}

	

	outBranches = branchTransforms;
	outLeafLocations = leaves;
}
