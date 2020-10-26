// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "UnrealClient.h"
#include "AsyncScreenShotTask.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnCustomScreenshotCaptured, bool, bSuccess, FString, FileName);

/**
 * 
 */
UCLASS()
class WALKSIMULATOR_API UAsyncScreenShotTask : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "ScreenShot")
	static UAsyncScreenShotTask* TakeScreenShot(const FString& InFileName, const FIntPoint& InResolution, const FOnCustomScreenshotCaptured& InCapturedDelegate);

	void StartCapture();

	UFUNCTION()
	void OnScreenshotCaptured(int32 Width, int32 Height, const TArray<FColor>& Colors);

private:
	
	FString FileName;
	
	FIntPoint Resolution;

	FOnCustomScreenshotCaptured CapturedDelegate;
};
