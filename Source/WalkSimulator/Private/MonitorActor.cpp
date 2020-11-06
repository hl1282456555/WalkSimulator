// Fill out your copyright notice in the Description page of Project Settings.


#include "MonitorActor.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "ProceduralMeshComponent.h"

// Sets default values
AMonitorActor::AMonitorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MonitorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MonitorMesh"));
	MonitorMesh->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MonitorCamera"));
	Camera->SetupAttachment(MonitorMesh);

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	CollisionSphere->SetupAttachment(MonitorMesh);

	ViewMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ViewMesh"));
	ViewMesh->SetupAttachment(MonitorMesh);

	ViewLength = 1000.0f;
}

float AMonitorActor::GetMonitorFOV()
{
	return Camera->FOVAngle;
}

void AMonitorActor::SetMonitorFOV(float NewFOV)
{
	Camera->FOVAngle = NewFOV;
}

void AMonitorActor::RefreshViewMesh(int32 SliceNum)
{
	ViewMesh->ClearAllMeshSections();

	TArray<FVector> viewPoints = CalculateViewPoints();
	if (!viewPoints.IsValidIndex(3)) {
		return;
	}
	
	float hLen = (viewPoints[3] - viewPoints[2]).Size();
	float stephLen = hLen / SliceNum;

	float vLen = (viewPoints[3] - viewPoints[0]).Size();
	float stepvLen = vLen / SliceNum;

	FVector startPoint = viewPoints[3];

	for (int32 vIndex = 0; vIndex < SliceNum; vIndex++)
	{
		TArray<FVector> sectionPoints;
		sectionPoints.Add(startPoint);
		TArray<int32> sectionTriangles;
		for (int32 hIndex = 0; hIndex < SliceNum; hIndex++)
		{

			FVector endPoint = viewPoints[3] + GetActorRightVector() * (stephLen * hIndex);
			endPoint.Z += vIndex * stepvLen;

			FCollisionQueryParams params(FName(TEXT("BlockAll")), true, this);
			params.bTraceComplex = true;
			params.bReturnPhysicalMaterial = false;
			
			FHitResult hitResult(ForceInit);

			if (!GetWorld()->LineTraceSingleByChannel(hitResult, startPoint, endPoint, ECollisionChannel::ECC_GameTraceChannel2, params)) {
				sectionPoints.Add(endPoint);
				continue;
			}

			sectionPoints.Add(hitResult.ImpactPoint);
			sectionTriangles.Add(0);
			sectionTriangles.Add(hIndex);
			sectionTriangles.Add(hIndex + 1);
		}

		ViewMesh->CreateMeshSection(vIndex, sectionPoints, sectionTriangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FVector2D>(), TArray<FVector2D>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	}
}

// Called when the game starts or when spawned
void AMonitorActor::BeginPlay()
{
	Super::BeginPlay();
	
}

TArray<FVector> AMonitorActor::CalculateViewPoints()
{
	TArray<FVector> resultPoints;

	FVector cameraLocation = GetActorLocation();
	FVector forwardVector = GetActorForwardVector();
	FVector rightVector = GetActorRightVector();
	FVector upVector = GetActorUpVector();

	float  fov = Camera->FOVAngle;

	FVector center = forwardVector * ViewLength + cameraLocation;
	float width = ViewLength * FMath::Tan(PI / (180.f) * fov / 2.f);
	float hight = width * 9.f / 16.f;

	FVector tempPoint = center - rightVector * width + upVector * hight;
	resultPoints.Add(tempPoint);
	tempPoint = center + rightVector * width + upVector * hight;
	resultPoints.Add(tempPoint);
	tempPoint = center + rightVector * width - upVector * hight;
	resultPoints.Add(tempPoint);
	tempPoint = center - rightVector * width - upVector * hight;
	resultPoints.Add(tempPoint);

	return resultPoints;
}

// Called every frame
void AMonitorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

