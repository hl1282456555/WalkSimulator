// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkSimulatorFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

void UWalkSimulatorFunctionLibrary::InitWalkPath(const FString& FilePath, TMap<int32, FPathPointList>& WalkPath)
{
	if (!FPaths::FileExists(FilePath))
	{
		return;
	}

	WalkPath.Empty();

	TArray<FString> stringArray;
	FFileHelper::LoadFileToStringArray(stringArray, *FilePath);
	if (stringArray.Num() <= 0)
	{
		return;
	}

	for (auto currentString : stringArray)
	{
		FString leftString, rightString;
		int32 tempId;
		FPathPoint tempPathPoint;

		currentString.Split(TEXT(","), &leftString, &rightString);

		if (leftString.IsEmpty())
		{
			continue;
		}
		
		tempPathPoint.Time = FCString::Atof(*leftString);
		
		currentString = rightString;
		if (currentString.IsEmpty())
		{
			continue;
		}

		currentString.Split(TEXT(","), &leftString, &rightString);
		if (leftString.IsEmpty())
		{
			continue;
		}
		tempId = FCString::Atoi(*leftString);

		currentString = rightString;
		if (currentString.IsEmpty())
		{
			continue;
		}

		currentString.Split(TEXT(","), &leftString, &rightString);
		if (leftString.IsEmpty())
		{
			continue;
		}
		tempPathPoint.Point.X = FCString::Atof(*leftString);

		if (rightString.IsEmpty())
		{
			continue;
		}

		tempPathPoint.Point.Y = FCString::Atof(*rightString);

		FPathPointList pathList = WalkPath.FindRef(tempId);
		pathList.WalkerId = tempId;
		pathList.PointList.Add(tempPathPoint);
		WalkPath.Add(tempId, pathList);
	}
	
	TArray<int32> walkerIds;
	WalkPath.GetKeys(walkerIds);

	for (auto currentId : walkerIds)
	{
		FPathPointList* pathPointList = WalkPath.Find(currentId);
		TArray<FPathPoint> pathPoint = WalkPath.Find(currentId)->PointList;
		pathPoint.Sort();
		pathPointList->PointList = pathPoint;
	}
}

void UWalkSimulatorFunctionLibrary::WalkPathInterpolation(TMap<int32, FPathPointList>& WalkPath, const float& DeltTime)
{
	TArray<int32> walkers;
	WalkPath.GetKeys(walkers);

	for (int32 walkerIndex = 0; walkerIndex < walkers.Num(); walkerIndex++)
	{
		TArray<FPathPoint> pointList = WalkPath.FindRef(walkers[walkerIndex]).PointList;
		if (pointList.Num() <= 1)
		{
			continue;
		}

		TArray<FPathPoint> tempPointList;
		pointList[0].Rotation = UKismetMathLibrary::FindLookAtRotation(pointList[0].Point, pointList[1].Point).Yaw;
		tempPointList.Add(pointList[0]);

		for (int32 ponitIndex = 1; ponitIndex < pointList.Num() -1; ponitIndex++)
		{
			FVector currentPath = pointList[ponitIndex].Point - pointList[ponitIndex - 1].Point;
			FVector nextPath = pointList[ponitIndex + 1].Point - pointList[ponitIndex].Point;
			int32 pathFrame = static_cast<int32>((pointList[ponitIndex].Time - pointList[ponitIndex - 1].Time) / DeltTime);
			float pathStartTime = pointList[ponitIndex].Time;
			float pathStartRotation = UKismetMathLibrary::FindLookAtRotation(pointList[ponitIndex - 1].Point, pointList[ponitIndex].Point).Yaw;
			float pathEndRotation = UKismetMathLibrary::FindLookAtRotation(pointList[ponitIndex].Point, pointList[ponitIndex + 1].Point).Yaw;
			float deltRotaion = (pathEndRotation - pathStartRotation) / pathFrame;
			float deltLength = currentPath.Size() / pathFrame;
			float pathSpeed = currentPath.Size() / (pointList[ponitIndex + 1].Time - pathStartTime);

			FPathPoint currentPathPoint;
			if (pathFrame > 1)
			{
				for (int32 currentFrame = 1; currentFrame < pathFrame; currentFrame++)
				{
					//TODO:旋转边界值处理
					currentPathPoint.Time = pathStartTime + DeltTime * currentFrame;
					currentPathPoint.Rotation = pathStartRotation + deltRotaion * currentFrame;
					currentPathPoint.Point = pointList[ponitIndex - 1].Point + currentPath.GetSafeNormal() * (deltLength * currentFrame);
					currentPathPoint.Speed = pathSpeed;
					tempPointList.Add(currentPathPoint);
				}
			}
			pointList[ponitIndex].Rotation = pathEndRotation;
			tempPointList.Add(pointList[ponitIndex]);
		}

		//最后一段路线插值
		float lastPathStartTime = tempPointList.Last().Time;
		float lastPathEndTime = pointList.Last().Time;
		int32 lastPathFrame = static_cast<int32>((lastPathEndTime - lastPathStartTime) / DeltTime);
		FVector lastPahtStartPoint = tempPointList.Last().Point;
		FVector lastPath = pointList.Last().Point - tempPointList.Last().Point;
		float lastPathRotation = UKismetMathLibrary::FindLookAtRotation(tempPointList.Last().Point, pointList.Last().Point).Yaw;
		float lastPathDeltLength = lastPath.Size() / lastPathFrame;
		float lastPathSpeed = lastPath.Size() / (lastPathEndTime - lastPathStartTime);

		FPathPoint currentLastPathPoint;
		if (lastPathFrame > 1)
		{
			for (int32 currentLastPathFrame = 1; currentLastPathFrame < lastPathFrame; currentLastPathFrame++)
			{
				currentLastPathPoint.Time = lastPathStartTime + DeltTime * currentLastPathFrame;
				currentLastPathPoint.Rotation = lastPathRotation;
				currentLastPathPoint.Point = lastPahtStartPoint + lastPath.GetSafeNormal() * (lastPathDeltLength * currentLastPathFrame);
				tempPointList.Add(currentLastPathPoint);
			}
		}
		pointList.Last().Rotation = lastPathRotation;
		tempPointList.Add(pointList.Last());

		FPathPointList currentPointList;
		currentPointList.PointList = tempPointList;
		currentPointList.WalkerId = walkerIndex;
		WalkPath.Add(walkers[walkerIndex], currentPointList);
	}
}
