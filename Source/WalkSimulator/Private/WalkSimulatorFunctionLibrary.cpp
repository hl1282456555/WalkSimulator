// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkSimulatorFunctionLibrary.h"

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