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
	TArray<AActor*> RefreshViewMesh(int32 VSliceNum, int32 HSliceNum);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterial* ViewMeshMaterial;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneCaptureComponent2D* Camera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* MonitorMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCapsuleComponent* CollisionCapsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProceduralMeshComponent* ViewMesh;
};
