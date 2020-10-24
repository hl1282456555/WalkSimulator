// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WalkSimulatorStructures.h"
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

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent*	SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent*		BoundMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 WalkerId;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FPathPoint> PathPoints;
};
