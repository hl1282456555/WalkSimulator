// Fill out your copyright notice in the Description page of Project Settings.


#include "VideoCaptureComponent.h"

#include "EasyFFMPEG.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/opt.h"
#include "libavutil/imgutils.h"
}

// Sets default values for this component's properties
UVideoCaptureComponent::UVideoCaptureComponent()
	: CodecName(TEXT("libx264"))
	, CaptureState(ECaptureState::NotInit)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UVideoCaptureComponent::InitCapture(const FString& InVideoFilename)
{
	if (IsInitialized()) {
		UE_LOG(LogFFmpeg, Warning, TEXT("Please uninitialize capture component before call InitCapture()."));
		return;
	}

	VideoFilename = InVideoFilename;

	Codec = avcodec_find_encoder_by_name(TCHAR_TO_UTF8(*CodecName));
	if (Codec == nullptr) {
		UE_LOG(LogFFmpeg, Error, TEXT("InitCapture() failed: Codec '%s' not found."), *CodecName);
		return;
	}

	CodecContext = avcodec_alloc_context3(Codec);
	if (CodecContext == nullptr) {
		UE_LOG(LogFFmpeg, Error, TEXT("InitCapture() failed: Could not allocate video codec context."));
		return;
	}

	CodecContext->bit_rate = CaptureConfigs.BitRate;
	CodecContext->width = CaptureConfigs.Width;
	CodecContext->height = CaptureConfigs.Height;
	CodecContext->time_base = (AVRational){CaptureConfigs.FrameRate.Y, CaptureConfigs.FrameRate.X};
	CodecContext->framerate = (AVRational){ CaptureConfigs.FrameRate.X, CaptureConfigs.FrameRate.Y};
	CodecContext->gop_size = CaptureConfigs.GopSize;
	CodecContext->max_b_frames = CaptureConfigs.MaxBFrames;
	CodecContext->pix_fmt = static_cast<AVPixelFormat>(CaptureConfigs.PixelFormat);

	if (Codec->id == AV_CODEC_ID_H264) {
		av_opt_set(CodecContext->priv_data, "preset", "slow", 0);
	}
}

bool UVideoCaptureComponent::IsInitialized()
{
	return CaptureState > ECaptureState::Initialized;
}

// Called when the game starts
void UVideoCaptureComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UVideoCaptureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

