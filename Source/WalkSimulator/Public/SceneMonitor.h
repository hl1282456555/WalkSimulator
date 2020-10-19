// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SceneMonitor.generated.h"

UCLASS()
class WALKSIMULATOR_API ASceneMonitor : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASceneMonitor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Monitor")
	void ToggleCapture(bool bCapture);

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monitor")
	class USceneCaptureComponent2D*	Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monitor")
	class UTextureRenderTarget2D* MonitorCanvas;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monitor")
	FVector2D CanvasSize;
};
