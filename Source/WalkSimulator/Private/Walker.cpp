// Fill out your copyright notice in the Description page of Project Settings.


#include "Walker.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PoseableMeshComponent.h"

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

	PoseableMesh = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("PosableMesh"));
	PoseableMesh->SetupAttachment(GetRootComponent());

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
		animFrame.BoneDatas.Add(boneName, SkeletalMesh->GetSocketTransform(boneName, ERelativeTransformSpace::RTS_Component));
	}

	AnimFrames.Add(animFrame.FrameTime, animFrame);
}

void AWalker::InitWalker(USkeletalMesh* Mesh, UClass* AnimClass)
{
	SkeletalMesh->SetSkeletalMesh(Mesh);
	SkeletalMesh->SetAnimClass(AnimClass);
}

void AWalker::SetBonePose(const float& Time)
{
	TArray<FName> boneNames;
	FAnimFrame CurrentAnimFrame;
	if (!FindNearestAnimFrame(Time, CurrentAnimFrame))
	{
		return;
	}

	CurrentAnimFrame.BoneDatas.GetKeys(boneNames);
	FTransform boneTransform;
	for (auto boneName : boneNames)
	{
		boneTransform = CurrentAnimFrame.BoneDatas.FindRef(boneName);
		PoseableMesh->SetBoneTransformByName(boneName, boneTransform, EBoneSpaces::ComponentSpace);
	}
}

void AWalker::SetWalkerTransform(const float& Time)
{
	if (PathPoints.Num() == 0)
	{
		return;
	}

	TArray<FPathPoint> tempPathPoints = PathPoints;
	FRotator rot;
	for (int32 pointIndex = 0; pointIndex < PathPoints.Num(); pointIndex++)
	{
		if ((pointIndex + 1) < PathPoints.Num())
		{
			if (Time < PathPoints[pointIndex].Time)
			{
				SetActorLocation(PathPoints[pointIndex].Point);
				rot.Yaw = PathPoints[pointIndex].Rotation;
				SetActorRotation(rot);
				break;
			}
			else if (PathPoints[pointIndex].Time < Time && PathPoints[pointIndex + 1].Time > Time)
			{
				float deltTime = (Time - PathPoints[pointIndex].Time) - (PathPoints[pointIndex + 1].Time - Time);
				SetActorLocation(PathPoints[deltTime > 0 ? pointIndex + 1 : pointIndex].Point);
				rot.Yaw = deltTime > 0 ? PathPoints[pointIndex + 1].Rotation : PathPoints[pointIndex].Rotation;
				SetActorRotation(rot);
				tempPathPoints.RemoveAt(pointIndex);
				break;
			}
			else
			{
				tempPathPoints.RemoveAt(pointIndex);
			}
		}
		else
		{
			SetActorLocation(PathPoints[pointIndex].Point);
			rot.Yaw = PathPoints[pointIndex].Rotation;
			SetActorRotation(rot);
			tempPathPoints.RemoveAt(pointIndex);
		}
	}
	PathPoints = tempPathPoints;
}

bool AWalker::FindNearestAnimFrame(const float& Time, FAnimFrame& CurrentAnimFrame)
{
	TArray<float> frameTimes;
	AnimFrames.GetKeys(frameTimes);
	
	if (frameTimes.Num() == 0)
	{
		return false;
	}

	for (int32 frameIndex = 0; frameIndex < frameTimes.Num(); frameIndex++)
	{
		if ((frameIndex + 1) < frameTimes.Num())
		{
			if (frameTimes[frameIndex ] > Time)
			{
				CurrentAnimFrame = AnimFrames.FindRef(frameTimes[frameIndex]);
				return true;
			}
			else if (frameTimes[frameIndex] < Time && frameTimes[frameIndex + 1] > Time)
			{
				float deltTime = (Time - frameTimes[frameIndex]) - (frameTimes[frameIndex + 1] - Time);
				CurrentAnimFrame = AnimFrames.FindRef(frameTimes[deltTime > 0 ? frameIndex + 1 : frameIndex]);
				AnimFrames.Remove(frameTimes[frameIndex]);
				return true;
			}
			else
			{
				AnimFrames.Remove(frameTimes[frameIndex]);
			}
		}
		else
		{
			CurrentAnimFrame = AnimFrames.FindRef(frameIndex);
			AnimFrames.Remove(frameTimes[frameIndex]);
			return true;
		}
	}
	return false;
}

