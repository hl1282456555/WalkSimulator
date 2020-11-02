// Fill out your copyright notice in the Description page of Project Settings.


#include "Walker.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UMG/Public/Blueprint/WidgetLayoutLibrary.h"

// Sets default values
AWalker::AWalker(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	CheckStartIndex(0),
	AnimStartIndex(0)
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
	CheckStartIndex = 0;
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

	GetWireFrame(WireFramePoints);
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
	
	for (int32 pointIndex = CheckStartIndex; pointIndex < PathPoints.Num(); pointIndex++)
	{
		FRotator rot(FRotator::ZeroRotator);
		if ((pointIndex + 1) < PathPoints.Num())
		{
			if (Time < PathPoints[pointIndex].Time)
			{
				SetActorLocation(PathPoints[pointIndex].Point);
				rot.Yaw = PathPoints[pointIndex].Rotation;
				SetActorRotation(rot);
				CheckStartIndex = pointIndex;
				break;
			}
			else if (PathPoints[pointIndex].Time < Time && PathPoints[pointIndex + 1].Time > Time)
			{
				float deltTime = (Time - PathPoints[pointIndex].Time) - (PathPoints[pointIndex + 1].Time - Time);
				SetActorLocation(PathPoints[deltTime > 0 ? pointIndex + 1 : pointIndex].Point);
				rot.Yaw = deltTime > 0 ? PathPoints[pointIndex + 1].Rotation : PathPoints[pointIndex].Rotation;
				SetActorRotation(rot);
				CurrentPoint = PathPoints[pointIndex];
				CheckStartIndex = pointIndex;
				break;
			}
			else
			{
				CurrentPoint = PathPoints[pointIndex];
				CheckStartIndex = pointIndex;
			}
		}
		else
		{
			SetActorLocation(PathPoints[pointIndex].Point);
			rot.Yaw = PathPoints[pointIndex].Rotation;
			SetActorRotation(rot);
			CurrentPoint = PathPoints[pointIndex];
			CheckStartIndex = pointIndex;
		}
	}
}

void AWalker::GetWireFrame(TArray<FVector2D>& WireFrame)
{
	FVector origin = SkeletalMesh->Bounds.Origin;
	FVector extent = SkeletalMesh->Bounds.BoxExtent;

	TArray<FVector> box;

	box.Add(origin + FVector(extent.X, extent.Y, extent.Z));
	box.Add(origin + FVector(extent.X, -extent.Y, extent.Z));
	box.Add(origin + FVector(-extent.X, -extent.Y, extent.Z));
	box.Add(origin + FVector(-extent.X, extent.Y, extent.Z));

	box.Add(origin + FVector(extent.X, extent.Y, -extent.Z));
	box.Add(origin + FVector(extent.X, -extent.Y, -extent.Z));
	box.Add(origin + FVector(-extent.X, -extent.Y, -extent.Z));
	box.Add(origin + FVector(-extent.X, extent.Y, -extent.Z));

	FVector2D Min;
	FVector2D Max;
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	if (playerController == nullptr)
	{
		return;
	}

	for (int32 index = 0; index < box.Num(); index++)
	{
		FVector2D screenLocation;
		playerController->ProjectWorldLocationToScreen(box[index], screenLocation, true);
		if (index == 0)
		{
			Min = screenLocation;
			Max = screenLocation;
		}
		else
		{
			if (screenLocation.X > Max.X)
			{
				Max.X = screenLocation.X;
			}
			if (screenLocation.X < Min.X)
			{
				Min.X = screenLocation.X;
			}
			if (screenLocation.Y > Max.Y)
			{
				Max.Y = screenLocation.Y;
			}
			if (screenLocation.Y < Min.Y)
			{
				Min.Y = screenLocation.Y;
			}
		}
	}

	float scale = UWidgetLayoutLibrary::GetViewportScale(this);

	WireFrame.Empty();
	WireFrame.Add(FVector2D(Min.X, Min.Y) / scale);
	WireFrame.Add(FVector2D(Max.X, Min.Y) / scale);
	WireFrame.Add(FVector2D(Max.X, Max.Y) / scale);
	WireFrame.Add(FVector2D(Min.X, Max.Y) / scale);
	//WireFrame.Add(FVector2D(Min.X, Min.Y) / scale);
}

bool AWalker::FindNearestAnimFrame(const float& Time, FAnimFrame& CurrentAnimFrame)
{
	TArray<float> frameTimes;
	AnimFrames.GetKeys(frameTimes);
	
	if (frameTimes.Num() == 0)
	{
		return false;
	}

	for (int32 frameIndex = AnimStartIndex; frameIndex < frameTimes.Num(); frameIndex++)
	{
		if ((frameIndex + 1) < frameTimes.Num())
		{
			if (frameTimes[frameIndex ] > Time)
			{
				CurrentAnimFrame = AnimFrames.FindRef(frameTimes[frameIndex]);
				AnimStartIndex = frameIndex;
				return true;
			}
			else if (frameTimes[frameIndex] < Time && frameTimes[frameIndex + 1] > Time)
			{
				float deltTime = (Time - frameTimes[frameIndex]) - (frameTimes[frameIndex + 1] - Time);
				CurrentAnimFrame = AnimFrames.FindRef(frameTimes[deltTime > 0 ? frameIndex + 1 : frameIndex]);
				AnimStartIndex = frameIndex;
				return true;
			}
			else
			{
				AnimStartIndex = frameIndex;
			}
		}
		else
		{
			CurrentAnimFrame = AnimFrames.FindRef(frameIndex);
			AnimStartIndex = frameIndex;
			return true;
		}
	}
	return false;
}

