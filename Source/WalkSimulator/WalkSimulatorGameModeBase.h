// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WalkSimulatorStructures.h"
#include "WalkSimulatorGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class WALKSIMULATOR_API AWalkSimulatorGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AWalkSimulatorGameModeBase();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void StartSimulate(const FString& DataFile);

	UFUNCTION(BlueprintCallable)
	void StopSimulate();

	UFUNCTION(BlueprintImplementableEvent)
	bool CanSpawnWalker();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<int32, FPathPointList>	WalkPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float StartSimulateTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> SimulateWalkerClass; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InterpolationTime;

private:
	bool IsSimulating;
};
