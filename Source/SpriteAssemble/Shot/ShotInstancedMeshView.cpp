#include "ShotInstancedMeshView.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

AShotInstancedMeshView::AShotInstancedMeshView()
{
	PrimaryActorTick.bCanEverTick = false;

	InstancedMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ShotInstances"));
	RootComponent = InstancedMeshComponent;
	InstancedMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMeshFinder.Succeeded())
	{
		DefaultMesh = CubeMeshFinder.Object;
		InstancedMeshComponent->SetStaticMesh(DefaultMesh);
	}
}

void AShotInstancedMeshView::BeginPlay()
{
	Super::BeginPlay();

	UMaterialInterface* BaseMaterial = InstancedMeshComponent->GetMaterial(0);
	if (!BaseMaterial)
	{
		BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	}

	UMaterialInstanceDynamic* DynamicMaterial = BaseMaterial
		? UMaterialInstanceDynamic::Create(BaseMaterial, InstancedMeshComponent)
		: nullptr;
	if (!DynamicMaterial)
	{
		return;
	}

	const FLinearColor ShotColor(1.0f, 0.82f, 0.05f, 1.0f);
	DynamicMaterial->SetVectorParameterValue(TEXT("Color"), ShotColor);
	DynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), ShotColor);
	InstancedMeshComponent->SetMaterial(0, DynamicMaterial);
}

int32 AShotInstancedMeshView::AddShotVisual(const FShotInstance& ShotInstance)
{
	EnsureDefaultMesh();
	return InstancedMeshComponent->AddInstance(BuildTransform(ShotInstance), true);
}

void AShotInstancedMeshView::UpdateShotVisual(int32 InstanceIndex, const FShotInstance& ShotInstance)
{
	if (InstanceIndex == INDEX_NONE)
	{
		return;
	}

	InstancedMeshComponent->UpdateInstanceTransform(InstanceIndex, BuildTransform(ShotInstance), true, true, true);
}

void AShotInstancedMeshView::ClearShotVisuals()
{
	InstancedMeshComponent->ClearInstances();
}

FTransform AShotInstancedMeshView::BuildTransform(const FShotInstance& ShotInstance) const
{
	const FVector Direction = ShotInstance.Direction.IsNearlyZero() ? FVector::ForwardVector : ShotInstance.Direction.GetSafeNormal();
	const FRotator Rotation = Direction.Rotation();
	const float VisualRadius = FMath::Max(1.0f, ShotInstance.Radius);
	const FVector Scale(VisualRadius / 25.0f, 0.04f, VisualRadius / 50.0f);
	return FTransform(Rotation, ShotInstance.Position, Scale);
}

void AShotInstancedMeshView::EnsureDefaultMesh()
{
	if (InstancedMeshComponent->GetStaticMesh() || !DefaultMesh)
	{
		return;
	}

	InstancedMeshComponent->SetStaticMesh(DefaultMesh);
}
