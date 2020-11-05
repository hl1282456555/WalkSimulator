// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonitorActor.generated.h"

UCLASS()
class WALKSIMULATOR_API AMonitorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMonitorActor();

	UFUNCTION(BlueprintPure)
	float GetMonitorFOV();

	UFUNCTION(BlueprintCallable)
	void SetMonitorFOV(float NewFOV);

	UFUNCTION(BlueprintCallable)
	void RefreshViewMesh(int32 SliceNum);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<FVector> CalculateViewPoints();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ViewLength;

public:

	class USceneCaptureComponent2D* Camera;
	class UStaticMeshComponent* MonitorMesh;
	class USphereComponent* CollisionSphere;
	class UProceduralMeshComponent* ViewMesh;
};
