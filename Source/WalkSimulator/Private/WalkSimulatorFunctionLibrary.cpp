// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkSimulatorFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"

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
		tempPathPoint.Point.X = FCString::Atof(*leftString) * 100.f;

		if (rightString.IsEmpty())
		{
			continue;
		}

		tempPathPoint.Point.Y = FCString::Atof(*rightString) * 100.f;

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
			float pathStartTime = pointList[ponitIndex - 1].Time;
			float pathStartRotation = UKismetMathLibrary::FindLookAtRotation(pointList[ponitIndex - 1].Point, pointList[ponitIndex].Point).Yaw;
			float pathEndRotation = UKismetMathLibrary::FindLookAtRotation(pointList[ponitIndex].Point, pointList[ponitIndex + 1].Point).Yaw;
			float deltRotaion = CalculateDelatRotation(pathStartRotation, pathEndRotation) / pathFrame;//(pathEndRotation - pathStartRotation) / pathFrame;
			float deltLength = currentPath.Size() / pathFrame;
			float pathSpeed = currentPath.Size() / (pointList[ponitIndex].Time - pathStartTime);

			FPathPoint currentPathPoint;
			if (pathFrame > 1)
			{
				for (int32 currentFrame = 1; currentFrame < pathFrame; currentFrame++)
				{
					//TODO:旋转边界值处理
					currentPathPoint.Time = pathStartTime + DeltTime * currentFrame;
					currentPathPoint.Rotation = pathStartRotation +deltRotaion * currentFrame;
					currentPathPoint.Point = pointList[ponitIndex - 1].Point + currentPath.GetSafeNormal() * (deltLength * currentFrame);
					currentPathPoint.Speed = pathSpeed;
					tempPointList.Add(currentPathPoint);
				}
			}
			pointList[ponitIndex].Rotation = pathEndRotation;
			pointList[ponitIndex].Speed = pathSpeed;
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

void UWalkSimulatorFunctionLibrary::GetViewPort(TArray<FVector>& ViewPoints, UCameraComponent* Camera, float Length)
{
	FVector cameraLocation = Camera->GetComponentLocation();
	FVector forwardVector = Camera->GetForwardVector();
	FVector rightVector = Camera->GetRightVector();
	FVector upVector = Camera->GetUpVector();

	float  fov = Camera->FieldOfView;

	FVector center = forwardVector * Length + cameraLocation;
	float width = Length * FMath::Tan(PI / (180.f) * fov / 2.f);
	float hight = width * 9.f / 16.f;

	FVector tempPoint = center - rightVector * width + upVector * hight;
	ViewPoints.Add(tempPoint);
	tempPoint = center + rightVector * width + upVector * hight;
	ViewPoints.Add(tempPoint);
	tempPoint = center + rightVector * width - upVector * hight;
	ViewPoints.Add(tempPoint);
	tempPoint = center - rightVector * width - upVector * hight;
	ViewPoints.Add(tempPoint);
}

TArray<FString> UWalkSimulatorFunctionLibrary::ReturnOpenFiles()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	TArray<FString> OpenFileNames;

	FString ExtensionStr;

	ExtensionStr = TEXT("JSON files | *.json");
	DesktopPlatform->OpenFileDialog(nullptr, TEXT("Load Camera Config"), FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()), TEXT(""), *ExtensionStr, EFileDialogFlags::None, OpenFileNames);
	if (OpenFileNames.Num() > 0)
	{
		FString FullPath = FPaths::ConvertRelativePathToFull(OpenFileNames[0]);
		OpenFileNames.Empty();
		OpenFileNames.Add(FullPath);
	}
	return OpenFileNames;
}

FString UWalkSimulatorFunctionLibrary::ReturnOpenDir()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	FString SaveFilePath;
	DesktopPlatform->OpenDirectoryDialog(nullptr, TEXT("Save Camera Config"), FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()), SaveFilePath);

	return SaveFilePath;
}

bool UWalkSimulatorFunctionLibrary::SaveStringToFile(FString InString, FString FilePath)
{
	return FFileHelper::SaveStringToFile(InString, *FilePath);
}

FTransform UWalkSimulatorFunctionLibrary::ConvertStringToTransform(FString InString)
{
	FString Left, Right;
	FVector Location;
	FRotator Rotation;
	FVector Scale;
	InString.Split(TEXT("Translation:"), &Left, &Right, ESearchCase::IgnoreCase, ESearchDir::FromStart);
	Right.Split(TEXT("Rotation:"), &Left, &Right, ESearchCase::IgnoreCase, ESearchDir::FromStart);
	Location.InitFromString(Left);

	Left.Empty();
	Right.Split(TEXT("Scale"), &Left, &Right, ESearchCase::IgnoreCase, ESearchDir::FromStart);
	Scale.InitFromString(Right);

	Rotation.InitFromString(Left);

	FTransform transform;

	transform.SetTranslation(Location);
	transform.SetRotation(Rotation.Quaternion());
	transform.SetScale3D(Scale);

	return transform;
}

float UWalkSimulatorFunctionLibrary::CalculateDelatRotation(float& StartRotation, float& EndRotation)
{
	if (FMath::Abs(StartRotation - EndRotation) < 180.f)
	{
		return EndRotation - StartRotation;
	}

	if (StartRotation < 0)
	{
		StartRotation += 360.f;
	}
	else
	{
		EndRotation += 360.f;
	}
	return EndRotation - StartRotation;
}

float UWalkSimulatorFunctionLibrary::ConvertHFOVToFocalLength(float SensorWidth, float HFOV)
{
	float focalLength = (SensorWidth / 2.f) / FMath::Tan(FMath::DegreesToRadians(HFOV / 2.f));
	return focalLength;
}

float UWalkSimulatorFunctionLibrary::ConvertFocalLengthToHFOV(float SensorWidth, float FocalLength)
{
	if (FocalLength <= 0) {
		return 0;
	}

	float HFOV = FMath::RadiansToDegrees(2.f * FMath::Atan(SensorWidth / (2.f * FocalLength)));
	return HFOV;
}
