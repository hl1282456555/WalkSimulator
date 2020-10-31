// Fill out your copyright notice in the Description page of Project Settings.


#include "VideoCaptureComponent.h"

#include "EasyFFMPEG.h"
#include "Engine/GameEngine.h"
#include "HAL/FileManager.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/opt.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include "libavutil/error.h"
}

#if WITH_EDITOR
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "IAssetViewport.h"
#endif

// Sets default values for this component's properties
UVideoCaptureComponent::UVideoCaptureComponent()
	: CaptureState(ECaptureState::NotInit)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UVideoCaptureComponent::StartCapture(const FString& InVideoFilename)
{
	if (IsInitialized()) {
		UE_LOG(LogFFmpeg, Warning, TEXT("Please uninitialize capture component before call InitCapture()."));
		return;
	}

	if (!InitFrameGrabber()) {
		UE_LOG(LogFFmpeg, Error, TEXT("InitCapture() failed: Init frame grabber failed."));
		return;
	}

	VideoFilename = InVideoFilename;

	if (!CreateVideoFileWriter()) {
		StopCapture();
		UE_LOG(LogFFmpeg, Error, TEXT(""));
		return;
	}

	Codec = avcodec_find_encoder_by_name("libx264");
	if (Codec == nullptr) {
		UE_LOG(LogFFmpeg, Error, TEXT("InitCapture() failed: Codec 'libx264' not found."));
		StopCapture();
		return;
	}

	CodecCtx = avcodec_alloc_context3(Codec);
	if (Codec == nullptr) {
		UE_LOG(LogFFmpeg, Error, TEXT("InitCapture() failed: Cloud not allocate video codec context."));
		StopCapture();
		return;
	}

	Packet = av_packet_alloc();
	if (Packet == nullptr) {
		UE_LOG(LogFFmpeg, Error, TEXT("InitCapture() failed: Cloud not allocate packet."));
		StopCapture();
		return;
	}

	CodecCtx->bit_rate = CaptureConfigs.BitRate;
	CodecCtx->width = CaptureConfigs.Width;
	CodecCtx->height = CaptureConfigs.Height;
	CodecCtx->time_base = {CaptureConfigs.FrameRate.Y, CaptureConfigs.FrameRate.X};
	CodecCtx->framerate = {CaptureConfigs.FrameRate.X, CaptureConfigs.FrameRate.Y};
	CodecCtx->gop_size = CaptureConfigs.GopSize;
	CodecCtx->max_b_frames = CaptureConfigs.MaxBFrames;
	CodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

	if (Codec->id == AV_CODEC_ID_H264) {
		av_opt_set(CodecCtx->priv_data, "preset", "slow", 0);
	}

	int32 result = avcodec_open2(CodecCtx, Codec, nullptr);
	if (result < 0) {
		UE_LOG(LogFFmpeg, Error, TEXT("InitCapture() failed: Could not open codec."));
		StopCapture();
		return;
	}

	Frame = av_frame_alloc();
	if (Frame == nullptr) {
		UE_LOG(LogFFmpeg, Error, TEXT("InitCapture() failed: Cloud not allocate video frame."));
		StopCapture();
		return;
	}

	Frame->format = CodecCtx->pix_fmt;
	Frame->width = CodecCtx->width;
	Frame->height = CodecCtx->height;

	result = av_frame_get_buffer(Frame, 0);
	if (result < 0) {
		UE_LOG(LogFFmpeg, Error, TEXT("InitCapture() failed: Cloud not allocate the video frame data."));
		StopCapture();
		return;
	}

	CaptureState = ECaptureState::Initialized;
}

bool UVideoCaptureComponent::IsInitialized()
{
	return CaptureState > ECaptureState::Initialized;
}

void UVideoCaptureComponent::CaptureThisFrame(int32 CurrentFrame)
{
	if (CaptureState == ECaptureState::NotInit || !FrameGrabber.IsValid()) {
		return;
	}

	FrameGrabber->CaptureThisFrame(FFramePayloadPtr());
	TArray<FCapturedFrameData> frames = FrameGrabber->GetCapturedFrames();

	if (!frames.IsValidIndex(0)) {
		return;
	}

	FCapturedFrameData& lastFrame = frames.Last();

	TArray<uint8> rgbColor;
	for (int32 index = 0; index < lastFrame.ColorBuffer.Num(); index++)
	{
		rgbColor.Add(lastFrame.ColorBuffer[index].R);
		rgbColor.Add(lastFrame.ColorBuffer[index].G);
		rgbColor.Add(lastFrame.ColorBuffer[index].B);
	}

	TArray<uint8> yuvColor = RGBToYUV420(rgbColor);

	WriteFrameToFile(yuvColor, CurrentFrame);
}

void UVideoCaptureComponent::StopCapture()
{
	if (IsInitialized() && Writer != nullptr) {
		EncodeVideoFrame(CodecCtx, nullptr, Packet);

		if (Codec->id == AV_CODEC_ID_MPEG1VIDEO || Codec->id == AV_CODEC_ID_MPEG2VIDEO) {
			uint8_t encode[] = { 0, 0, 1, 0xb7 };
			Writer->Serialize(encode, sizeof(encode));
		}
	}

	CaptureState = ECaptureState::NotInit;
	ReleaseFrameGrabber();
	DestroyVideoFileWriter();
	ReleaseContext();
}

// Called when the game starts
void UVideoCaptureComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UVideoCaptureComponent::BeginDestroy()
{
	Super::BeginDestroy();
	StopCapture();
}

bool UVideoCaptureComponent::CreateVideoFileWriter()
{
	if (IFileManager::Get().FileExists(*VideoFilename)) {
		IFileManager::Get().Delete(*VideoFilename);
	}

	Writer = IFileManager::Get().CreateFileWriter(*VideoFilename, EFileWrite::FILEWRITE_Append);
	if (Writer == nullptr) {
		return false;
	}

	return true;
}

void UVideoCaptureComponent::DestroyVideoFileWriter()
{
	if (Writer == nullptr) {
		return;
	}

	Writer->Flush();
	Writer->Close();

	delete Writer;
	Writer = nullptr;
}

bool UVideoCaptureComponent::InitFrameGrabber()
{
	if (FrameGrabber.IsValid()) {
		return true;
	}

	TSharedPtr<FSceneViewport> sceneViewport;

#if WITH_EDITOR
	if (GIsEditor)
	{
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			if (Context.WorldType == EWorldType::PIE)
			{
				FSlatePlayInEditorInfo* SlatePlayInEditorSession = GEditor->SlatePlayInEditorMap.Find(Context.ContextHandle);
				if (SlatePlayInEditorSession)
				{
					if (SlatePlayInEditorSession->DestinationSlateViewport.IsValid())
					{
						TSharedPtr<IAssetViewport> DestinationLevelViewport = SlatePlayInEditorSession->DestinationSlateViewport.Pin();
						sceneViewport = DestinationLevelViewport->GetSharedActiveViewport();
					}
					else if (SlatePlayInEditorSession->SlatePlayInEditorWindowViewport.IsValid())
					{
						sceneViewport = SlatePlayInEditorSession->SlatePlayInEditorWindowViewport;
					}
				}
			}
		}
	}
	else
#endif
	{
		UGameEngine* gameEngine = Cast<UGameEngine>(GEngine);
		if (gameEngine == nullptr || !gameEngine->SceneViewport.IsValid()) {
			return false;
		}

		sceneViewport = gameEngine->SceneViewport;
	}


	FrameGrabber = MakeShareable(new FFrameGrabber(sceneViewport.ToSharedRef(), FIntPoint(CaptureConfigs.Width, CaptureConfigs.Height)));
	FrameGrabber->StartCapturingFrames();

	return true;
}

void UVideoCaptureComponent::ReleaseFrameGrabber()
{
	if (CaptureState == ECaptureState::NotInit) {
		return;
	}

	if (FrameGrabber.IsValid()){
		FrameGrabber->StopCapturingFrames();
		FrameGrabber->Shutdown();
		FrameGrabber.Reset();
	}
}


void UVideoCaptureComponent::ReleaseContext()
{
	if (CodecCtx != nullptr) {
		avcodec_free_context(&CodecCtx);
		CodecCtx = nullptr;
	}

	if (Frame != nullptr) {
		av_frame_free(&Frame);
		Frame = nullptr;
	}

	if (Packet != nullptr) {
		av_packet_free(&Packet);
		Packet = nullptr;
	}
}

void UVideoCaptureComponent::WriteFrameToFile(const TArray<uint8>& ColorBuffer, int32 CurrentFrame)
{
	int32 result = av_frame_make_writable(Frame);
	if (result < 0) {
		UE_LOG(LogFFmpeg, Error, TEXT("Cant make the frame writable."));
		return;
	}

	Frame->data[0] = (uint8_t*)ColorBuffer.GetData();
	Frame->data[1] = (uint8_t*)&ColorBuffer[CaptureConfigs.Width * CaptureConfigs.Height];
	Frame->data[2] = (uint8_t*)&ColorBuffer[(CaptureConfigs.Width * CaptureConfigs.Height) + (CaptureConfigs.Width * CaptureConfigs.Height * 1 / 4)];

	Frame->pts = CurrentFrame;

	EncodeVideoFrame(CodecCtx, Frame, Packet);
}

TArray<uint8> UVideoCaptureComponent::RGBToYUV420(const TArray<uint8>& RGBColor)
{
	TArray<uint8> yuvColor;
	yuvColor.AddZeroed(CaptureConfigs.Width * CaptureConfigs.Height * 3 / 2);

	int32 indexY = 0;
	int32 indexU = CaptureConfigs.Width * CaptureConfigs.Height;
	int32 indexV = indexU + (CaptureConfigs.Width * CaptureConfigs.Height * 1 / 4);

	for (int32 indexH = 0; indexH < CaptureConfigs.Height; indexH++)
	{
		for (int32 indexW = 0; indexW < CaptureConfigs.Width; indexW++)
		{
			uint8 r, g, b;
			r = RGBColor[CaptureConfigs.Width * indexH * 3];
			g = RGBColor[CaptureConfigs.Width * indexH * 3 + 1];
			b = RGBColor[CaptureConfigs.Width * indexH * 3 + 2];

			uint8 y, u, v;
			y = (uint8)((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
			u = (uint8)((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
			v = (uint8)((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;

			yuvColor[indexY++] = FMath::Clamp<uint8>(y, 0, 255);
			if (indexH % 2 == 0 && indexW % 2 == 0) {
				yuvColor[indexU++] = FMath::Clamp<uint8>(u, 0, 255);
			}
			else {
				if (indexW % 2 == 0) {
					yuvColor[indexV++] = FMath::Clamp<uint8>(v, 0, 255);
				}
			}
		}
	}

	return yuvColor;
}

void UVideoCaptureComponent::EncodeVideoFrame(struct AVCodecContext* InCodecCtx, struct AVFrame* InFrame, struct AVPacket* InPacket)
{
	if (InFrame != nullptr) {
		UE_LOG(LogFFmpeg, Log, TEXT("Send frame %3lld"), InFrame->pts);
	}

	int32 result = avcodec_send_frame(InCodecCtx, InFrame);
	if (result < 0) {
		UE_LOG(LogFFmpeg, Error, TEXT("Error sending a frame for encoding."));
		return;
	}

	while(result >= 0)
	{
		result = avcodec_receive_packet(InCodecCtx, InPacket);
		if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
			return;
		}
		else if (result < 0) {
			UE_LOG(LogFFmpeg, Error, TEXT("Error during encoding."));
			return;
		}

		UE_LOG(LogFFmpeg, Log, TEXT("Write packet %3lld(size=%5d)"), InPacket->pts, InPacket->size);
		Writer->Serialize(InPacket->data, InPacket->size);
		av_packet_unref(InPacket);
	}
}

// Called every frame
void UVideoCaptureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
