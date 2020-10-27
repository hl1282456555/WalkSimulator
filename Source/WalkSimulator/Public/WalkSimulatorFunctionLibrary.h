// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WalkSimulatorStructures.h"
#include "WalkSimulatorFunctionLibrary.generated.h"

UCLASS()
class WALKSIMULATOR_API UWalkSimulatorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void InitWalkPath(const FString& FilePath, TMap<int32, FPathPointList>& WalkPath);

	UFUNCTION(BlueprintCallable)
	static void WalkPathInterpolation(UPARAM(Ref) TMap<int32, FPathPointList>& WalkPath, const float& DeltTime);

	UFUNCTION(BlueprintPure)
		static float ConvertHFOVToFocalLength(float SensorWidth, float HFOV);

	UFUNCTION(BlueprintPure)
		static float ConvertFocalLengthToHFOV(float SensorWidth, float FocalLength);
};
