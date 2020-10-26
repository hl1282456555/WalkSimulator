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

USTRUCT(BlueprintType)
struct FPathPoint
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		float Time;

	UPROPERTY(BlueprintReadWrite)
		FVector Point;

	UPROPERTY(BlueprintReadWrite)
		float Rotation;

	UPROPERTY(BlueprintReadWrite)
		float Speed;

public:
	friend bool operator< (const FPathPoint& LHP, const FPathPoint& RHP)
	{
		return LHP.Time < RHP.Time;
	}
};

USTRUCT(BlueprintType)
struct FPathPointList
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString MeshName;

	UPROPERTY(BlueprintReadWrite)
	int32 WalkerId;

	UPROPERTY(BlueprintReadWrite)
	TArray<FPathPoint> PointList;
};

USTRUCT(BlueprintType)
struct FAnimFrame
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	float	FrameTime;

	UPROPERTY(BlueprintReadWrite)
	TMap<FName, FTransform>	BoneDatas;
};