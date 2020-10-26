// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncScreenShotTask.h"

#include "Modules/ModuleManager.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Misc/FileHelper.h"

UAsyncScreenShotTask::UAsyncScreenShotTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AddToRoot();
}

UAsyncScreenShotTask* UAsyncScreenShotTask::TakeScreenShot(const FString& InFileName, const FIntPoint& InResolution, const FOnCustomScreenshotCaptured& InCapturedDelegate)
{
	UAsyncScreenShotTask* task = NewObject<UAsyncScreenShotTask>();
	task->FileName = InFileName;
	task->Resolution = InResolution;
	task->CapturedDelegate = InCapturedDelegate;

	task->StartCapture();

	return task;
}

void UAsyncScreenShotTask::StartCapture()
{
	FScreenshotRequest::OnScreenshotCaptured().AddUFunction(this, TEXT("OnScreenshotCaptured"));
	FScreenshotRequest::RequestScreenshot(false);
}

void UAsyncScreenShotTask::OnScreenshotCaptured(int32 Width, int32 Height, const TArray<FColor>& Colors)
{
	IImageWrapperModule& imageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	IImageWrapperPtr imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	if (!imageWrapper.IsValid() || !imageWrapper->SetRaw(Colors.GetData(), Colors.Num(), Resolution.X, Resolution.Y, ERGBFormat::RGBA, 8)) {
		CapturedDelegate.ExecuteIfBound(false, TEXT(""));
		RemoveFromRoot();
		return;
	}

	TArray64<uint8> compressedColors = imageWrapper->GetCompressed();
	if (!FFileHelper::SaveArrayToFile(compressedColors, *FileName)) {
		CapturedDelegate.ExecuteIfBound(false, TEXT(""));
		RemoveFromRoot();
		return;
	}

	CapturedDelegate.ExecuteIfBound(true, FileName);
	RemoveFromRoot();
	return;
}
