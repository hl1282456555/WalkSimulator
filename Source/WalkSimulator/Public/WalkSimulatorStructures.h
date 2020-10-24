// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WalkSimulatorStructures.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ECaptureState : uint8
{
	None = 0,
	PendingCaptureData,
	DataCaptureCompeleted,
	SynthesisVideo,
};

/**
 *
 */
USTRUCT(BlueprintType)
struct FPathPoint {
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
struct FPathPointList {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int32 WalkerId;

	UPROPERTY(BlueprintReadWrite)
		TArray<FPathPoint> PointList;
};