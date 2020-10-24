// Fill out your copyright notice in the Description page of Project Settings.


#include "Walker.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AWalker::AWalker(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	BoundMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoundMesh"));
	BoundMesh->SetupAttachment(GetRootComponent());

	WalkerId = -1;
}

// Called when the game starts or when spawned
void AWalker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWalker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

