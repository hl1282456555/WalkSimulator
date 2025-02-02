// Fill out your copyright notice in the Description page of Project Settings.


#include "MonitorActor.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "ProceduralMeshComponent.h"
#include "Materials/MaterialInterface.h"

// Sets default values
AMonitorActor::AMonitorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));

	MonitorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MonitorMesh"));
	MonitorMesh->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MonitorCamera"));
	Camera->SetupAttachment(GetRootComponent());

	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	CollisionCapsule->SetupAttachment(GetRootComponent());

	ViewMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ViewMesh"));
	ViewMesh->SetupAttachment(GetRootComponent());

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

TArray<AActor*> AMonitorActor::RefreshViewMesh(int32 VSliceNum, int32 HSliceNum)
{

	TArray<AActor*> hittedActors;
	ViewMesh->ClearAllMeshSections();

	TArray<FVector> viewPoints = CalculateViewPoints();
	if (!viewPoints.IsValidIndex(3)) {
		return hittedActors;
	}

	float hLen = (viewPoints[3] - viewPoints[2]).Size();
	float stephLen = hLen / HSliceNum;

	float vLen = (viewPoints[3] - viewPoints[0]).Size();
	float stepvLen = vLen / VSliceNum;

	FVector startPoint = GetActorLocation();

	for (int32 vIndex = 0; vIndex < VSliceNum + 1; vIndex++)
	{
		TArray<FVector> sectionPoints;
		sectionPoints.Add(FVector::ZeroVector);
		TArray<int32> sectionTriangles;
		TArray<FVector> sectionNormal;
		sectionNormal.Add(FVector(0.0f, 0.0f, 1.0f));
		for (int32 hIndex = 0; hIndex < HSliceNum + 1; hIndex++)
		{

			FVector endPoint = viewPoints[3] + GetActorRightVector() * (stephLen * hIndex);
			endPoint += GetActorUpVector() * vIndex * stepvLen;

			FCollisionQueryParams params;
			params.bTraceComplex = false;
			params.bReturnPhysicalMaterial = false;
			params.MobilityType = EQueryMobilityType::Any;
			params.AddIgnoredActor(this);
			
			FHitResult hitResult(ForceInit);

			if (!GetWorld()->LineTraceSingleByChannel(hitResult, startPoint, endPoint, ECollisionChannel::ECC_GameTraceChannel3, params)) {
				sectionPoints.Add(GetActorTransform().InverseTransformPosition(endPoint));
			}
			else {
				sectionPoints.Add(GetActorTransform().InverseTransformPosition(hitResult.ImpactPoint));
			}

			FCollisionObjectQueryParams objectParams;
			objectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel4);
			if (GetWorld()->LineTraceSingleByObjectType(hitResult, startPoint, endPoint, objectParams, params)) {
				AActor* hittedActor = hitResult.GetActor();
				if (hittedActor != nullptr) {
					hittedActors.AddUnique(hitResult.GetActor());
				}
			}

			sectionNormal.Add(FVector(0.0f, 0.0f, 1.0f));
		}

		for (int32 pointIndex = 1; pointIndex < sectionPoints.Num() - 1; pointIndex++)
		{
			sectionTriangles.Add(pointIndex + 1);
			sectionTriangles.Add(pointIndex);
			sectionTriangles.Add(0);

			sectionTriangles.Add(0);
			sectionTriangles.Add(pointIndex);
			sectionTriangles.Add(pointIndex + 1);
		}

		ViewMesh->CreateMeshSection(vIndex, sectionPoints, sectionTriangles, sectionNormal, TArray<FVector2D>(), TArray<FVector2D>(), TArray<FVector2D>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
		ViewMesh->SetMaterial(vIndex, ViewMeshMaterial);
	}

	if (hittedActors.IsValidIndex(0)) {
		hittedActors.Add(this);
	}

	return hittedActors;
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

