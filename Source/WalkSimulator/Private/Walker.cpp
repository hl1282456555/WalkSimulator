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

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;

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

void AWalker::CaptureAnimFrame(const float& StartRecordTime)
{
	TArray<FName> boneNames;
	SkeletalMesh->GetBoneNames(boneNames);

	FAnimFrame animFrame;
	UWorld* world = GEngine->GetWorldFromContextObjectChecked(this);
	float time = world ? (world->TimeSeconds - StartRecordTime) : 0;
	animFrame.FrameTime = time;

	for (auto boneName : boneNames)
	{
		animFrame.BoneDatas.Add(boneName, SkeletalMesh->GetSocketTransform(boneName, ERelativeTransformSpace::RTS_World));
	}

	AnimFrames.Add(animFrame.FrameTime, animFrame);
}

void AWalker::InitWalker(USkeletalMesh* Mesh, UClass* AnimClass)
{
	SkeletalMesh->SetSkeletalMesh(Mesh);
	SkeletalMesh->SetAnimClass(AnimClass);
}


