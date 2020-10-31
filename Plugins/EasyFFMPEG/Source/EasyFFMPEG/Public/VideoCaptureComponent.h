// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrameGrabber.h"
#include "Components/SceneComponent.h"
#include "VideoCaptureComponent.generated.h"

UENUM(BlueprintType)
enum class ECaptureState : uint8
{
	NotInit = 0,
	Initialized,
	Capturing,
};

USTRUCT(BlueprintType)
struct FCaptureConfigs
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Capture Configs")
	int32	BitRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Capture Configs")
	int32	Width;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Capture Configs")
	int32	Height;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Capture Configs")
	FIntPoint	FrameRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Capture Configs")
	int32	GopSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Capture Configs")
	int32	MaxBFrames;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Capture Configs")
	int32	PixelFormat;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EASYFFMPEG_API UVideoCaptureComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVideoCaptureComponent();

	UFUNCTION(BlueprintCallable, Category = "Video Capture")
	void InitCapture(const FString& InVideoFilename);

	UFUNCTION(BlueprintPure, Category = "Video Capture")
	bool IsInitialized();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Video Capture")
	FString CodecName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Video Capture")
	FString	VideoFilename;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Video Capture")
	FCaptureConfigs	CaptureConfigs;

	ECaptureState CaptureState;

private:

	struct AVCodecContext* CodecContext;
	struct AVCodec* Codec;

	TSharedPtr<FFrameGrabber>	FrameGrabber;
};
