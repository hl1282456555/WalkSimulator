// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncScreenShotTask.h"

#include "Modules/ModuleManager.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Misc/FileHelper.h"
#include "HighResScreenshot.h"

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
	GEngine->GameViewport->OnScreenshotCaptured().AddUObject(this, &UAsyncScreenShotTask::OnScreenshotCaptured);

	GetHighResScreenshotConfig().SetResolution(Resolution.X, Resolution.Y);
	GetHighResScreenshotConfig().SetHDRCapture(false);
	GetHighResScreenshotConfig().SetFilename(FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Temp/TempScreenshot.png")));
	FScreenshotRequest::RequestScreenshot(false);
}

void UAsyncScreenShotTask::OnScreenshotCaptured(int32 Width, int32 Height, const TArray<FColor>& Colors)
{
	GEngine->GameViewport->OnScreenshotCaptured().Clear();

	IImageWrapperModule& imageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	if (!imageWrapper.IsValid() || !imageWrapper->SetRaw(Colors.GetData(), Colors.Num() * sizeof(FColor), Width, Height, ERGBFormat::BGRA, 8)) {
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
