// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WalkSimulatorStructures.h"
#include "VaRestJsonObject.h"
#include "Walker.generated.h"

UCLASS()
class WALKSIMULATOR_API AWalker : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWalker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void CaptureAnimFrame(const float& StartRecordTime);

	UFUNCTION(BlueprintCallable)
	void InitWalker(USkeletalMesh* Mesh, UClass* AnimClass);

	UFUNCTION(BlueprintCallable)
	void SetBonePose(const float& Time);

	UFUNCTION(BlueprintCallable)
	void SetWalkerTransform(const float& Time);

	UFUNCTION(BlueprintCallable)
	void GetWireFrame(TArray<FVector2D>& WireFrame);

	UFUNCTION(BlueprintCallable)
	bool IsWalkerInViewport();

	UFUNCTION(BlueprintCallable)
	void CaptureWalkerFrameData(const int32& Frame);

	UFUNCTION(BlueprintCallable)
	void RefreshVisibility(float FrameTime);

	UFUNCTION(BlueprintCallable)
	UVaRestJsonObject* ExportWalkerFrameData();

private:
	bool FindNearestAnimFrame(const float& Time, FAnimFrame& CurrentAnimFrame);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* Scene;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent*	SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UPoseableMeshComponent* PoseableMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent*		BoundMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 WalkerId;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString MeshName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> BoneNames;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float SimulateTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float SpawnTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FPathPoint CurrentPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FPathPoint> PathPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<float, FAnimFrame>	AnimFrames;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32	CheckStartIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 AnimStartIndex;

	UPROPERTY(Transient, BlueprintReadOnly)
	TArray<FVector2D> WireFramePoints;

	UPROPERTY(Transient, BlueprintReadOnly)
	TMap<int32, FWalkerCaptureData> WalkerFrameData;

	UPROPERTY(Transient, BlueprintReadOnly)
	UVaRestJsonObject* WalkerCaptureJosn;

private:
	TArray<UVaRestJsonObject*> WalkerFrameJosn;
};
