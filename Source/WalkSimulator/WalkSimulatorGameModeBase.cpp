// Copyright Epic Games, Inc. All Rights Reserved.


#include "WalkSimulatorGameModeBase.h"
#include "WalkSimulatorFunctionLibrary.h"
#include "Walker.h"

AWalkSimulatorGameModeBase::AWalkSimulatorGameModeBase()
	:InterpolationTime(0.04)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWalkSimulatorGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TArray<int32> walkerIds;
	WalkPath.GetKeys(walkerIds);

	if (walkerIds.Num() == 0)
	{
		return;
	}

	UWorld* world = GEngine->GetWorldFromContextObjectChecked(this);
	float simulateTime = (world ? world->GetTimeSeconds() : 0.f) - StartSimulateTime;

	for (int32 walkerIndex = 0; walkerIndex < walkerIds.Num(); walkerIndex++)
	{
		FPathPointList pathPointList = WalkPath.FindRef(walkerIds[walkerIndex]);
		if (pathPointList.PointList.Num() > 0)
		{
			FPathPoint pathpoint = pathPointList.PointList[0];
			if (simulateTime >= pathpoint.Time)
			{
				pathPointList.MeshName = TEXT("Defender");
				FTransform transform;
				transform.SetLocation(pathpoint.Point);
				FRotator rotation;
				rotation.Yaw = pathpoint.Rotation;
				transform.SetScale3D(FVector::OneVector);

				AWalker* walker = Cast<AWalker>(world->SpawnActor<AActor>(SimulateWalkerClass, transform));
				walker->PathPoints = pathPointList.PointList;
				walker->WalkerId = pathPointList.WalkerId;
				walker->MeshName = pathPointList.MeshName;
				walker->SimulateTime = StartSimulateTime;

				WalkPath.Remove(walkerIds[walkerIndex]);
			}
		}
	}
}

void AWalkSimulatorGameModeBase::StartSimulate(const FString& DataFile)
{
	UWalkSimulatorFunctionLibrary::InitWalkPath(DataFile, WalkPath);
	UWalkSimulatorFunctionLibrary::WalkPathInterpolation(WalkPath, InterpolationTime);

	UWorld* world = GEngine->GetWorldFromContextObjectChecked(this);
	StartSimulateTime = world ? 0.f : world->GetTimeSeconds();
	
	PrimaryActorTick.bCanEverTick = true;
}
