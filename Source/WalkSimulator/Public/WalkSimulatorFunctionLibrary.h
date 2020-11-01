// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WalkSimulatorStructures.h"
#include "Camera/CameraComponent.h"
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

	UFUNCTION(BlueprintCallable)
	static void GetViewPort(TArray<FVector>& ViewPoints, UCameraComponent* Camera, float Length);

	UFUNCTION(BlueprintCallable)
	static TArray<FString> ReturnOpenFiles();

	UFUNCTION(BlueprintCallable)
	static FString ReturnOpenDir();

	UFUNCTION(BlueprintCallable)
	static bool SaveStringToFile(FString InString, FString FilePath);

	UFUNCTION(BlueprintCallable)
	static FTransform ConvertStringToTransform(FString InString);

private:
	static float CalculateDelatRotation(float& StartRotation, float& EndRotation);

	UFUNCTION(BlueprintPure)
	static float ConvertHFOVToFocalLength(float SensorWidth, float HFOV);

	UFUNCTION(BlueprintPure)
	static float ConvertFocalLengthToHFOV(float SensorWidth, float FocalLength);
};
