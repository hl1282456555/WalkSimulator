// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneMonitor.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

// Sets default values
ASceneMonitor::ASceneMonitor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Camera"));
	Camera->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ASceneMonitor::BeginPlay()
{
	Super::BeginPlay();

	MonitorCanvas = NewObject<UTextureRenderTarget2D>(this);
	MonitorCanvas->InitCustomFormat(CanvasSize.X, CanvasSize.Y, PF_A8R8G8B8, false);
	MonitorCanvas->TargetGamma = 2.2f;
	
	Camera->TextureTarget = MonitorCanvas;
}

// Called every frame
void ASceneMonitor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASceneMonitor::ToggleCapture(bool bCapture)
{
	if (Camera == nullptr) {
		return;
	}

	Camera->SetActive(bCapture, true);
}

