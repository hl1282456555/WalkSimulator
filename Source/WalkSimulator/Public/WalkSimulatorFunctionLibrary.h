// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WalkSimulatorFunctionLibrary.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct WALKSIMULATOR_API FPathPoint {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	float Time;

	UPROPERTY(BlueprintReadWrite)
	FVector Point;

public:
	friend bool operator< (const FPathPoint& LHP, const FPathPoint& RHP)
	{
		return LHP.Time < RHP.Time;
	}
};

USTRUCT(BlueprintType)
struct WALKSIMULATOR_API FPathPointList {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 WalkerId;

	UPROPERTY(BlueprintReadWrite)
	TArray<FPathPoint> PointList;
};

UCLASS()
class WALKSIMULATOR_API UWalkSimulatorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void InitWalkPath(const FString& FilePath, TMap<int32, FPathPointList>& WalkPath);
};
