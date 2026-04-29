#include "EnemyBase.h"
#include "EnemyActionComponent.h"
#include "EnemyBrainComponent.h"
#include "EnemyDefinition.h"
#include "../Attribute/AttributeComponent.h"
#include "../Combat/CombatComponent.h"
#include "../Combat/HealthComponent.h"
#include "../Core/GameplayEvent.h"
#include "../Core/GameplayEventBus.h"
#include "../Effect/EffectComponent.h"
#include "../Weapon/ShooterComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
void ApplyEnemyDebugPlaneMaterial(UStaticMeshComponent* Component, const FLinearColor& Color)
{
	if (!Component)
	{
		return;
	}

	UMaterialInterface* BaseMaterial = Component->GetMaterial(0);
	if (!BaseMaterial)
	{
		BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	}

	UMaterialInstanceDynamic* DynamicMaterial = BaseMaterial
		? UMaterialInstanceDynamic::Create(BaseMaterial, Component)
		: nullptr;
	if (!DynamicMaterial)
	{
		return;
	}

	DynamicMaterial->SetVectorParameterValue(TEXT("Color"), Color);
	DynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), Color);
	Component->SetMaterial(0, DynamicMaterial);
}
}

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	EffectComponent = CreateDefaultSubobject<UEffectComponent>(TEXT("EffectComponent"));
	ShooterComponent = CreateDefaultSubobject<UShooterComponent>(TEXT("ShooterComponent"));
	BrainComponent = CreateDefaultSubobject<UEnemyBrainComponent>(TEXT("EnemyBrainComponent"));
	ActionComponent = CreateDefaultSubobject<UEnemyActionComponent>(TEXT("EnemyActionComponent"));
	MarkerPlaneComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyMarkerPlane"));
	MarkerPlaneComponent->SetupAttachment(RootComponent);
	MarkerPlaneComponent->SetRelativeLocation(FVector::ZeroVector);
	MarkerPlaneComponent->SetUsingAbsoluteRotation(true);
	MarkerPlaneComponent->SetWorldRotation(FRotator(0.0f, 0.0f, 90.0f));
	MarkerPlaneComponent->SetRelativeScale3D(FVector(0.55f, 1.0f, 1.0f));
	MarkerPlaneComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MarkerPlaneComponent->SetHiddenInGame(false);
	MarkerPlaneComponent->SetCastShadow(false);
	MarkerPlaneBackComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyMarkerPlaneBack"));
	MarkerPlaneBackComponent->SetupAttachment(RootComponent);
	MarkerPlaneBackComponent->SetRelativeLocation(FVector::ZeroVector);
	MarkerPlaneBackComponent->SetUsingAbsoluteRotation(true);
	MarkerPlaneBackComponent->SetWorldRotation(FRotator(0.0f, 0.0f, -90.0f));
	MarkerPlaneBackComponent->SetRelativeScale3D(FVector(0.55f, 1.0f, 1.0f));
	MarkerPlaneBackComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MarkerPlaneBackComponent->SetHiddenInGame(false);
	MarkerPlaneBackComponent->SetCastShadow(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MarkerPlaneMesh(TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (MarkerPlaneMesh.Succeeded())
	{
		MarkerPlaneComponent->SetStaticMesh(MarkerPlaneMesh.Object);
		MarkerPlaneBackComponent->SetStaticMesh(MarkerPlaneMesh.Object);
	}

	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));
	GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Y);
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	ApplyEnemyDebugPlaneMaterial(MarkerPlaneComponent, FLinearColor(1.0f, 0.16f, 0.08f, 0.92f));
	ApplyEnemyDebugPlaneMaterial(MarkerPlaneBackComponent, FLinearColor(1.0f, 0.16f, 0.08f, 0.92f));

	ApplyDefinition(Definition);
	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &AEnemyBase::HandleDeath);
	}
}

void AEnemyBase::ApplyDefinition(UEnemyDefinition* InDefinition)
{
	if (InDefinition)
	{
		Definition = InDefinition;
		Team = Definition->Team;
		if (HealthComponent)
		{
			HealthComponent->InitializeHealth(Definition->MaxHealth);
		}
		if (BrainComponent)
		{
			BrainComponent->SetTeam(Team);
			BrainComponent->SetRanges(Definition->DetectionRange, Definition->AttackRange);
		}
		if (ActionComponent)
		{
			ActionComponent->Configure(Definition->MoveSpeed, Definition->ContactDamage, Definition->AttackCooldown, Team, Definition->DefaultAction);
		}
		if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
		{
			MovementComponent->MaxWalkSpeed = Definition->MoveSpeed;
		}
	}
	else if (HealthComponent)
	{
		HealthComponent->InitializeHealth(HealthComponent->GetMaxHealth());
	}
}

FGameplayCommandResult AEnemyBase::ApplyDamageSpec_Implementation(const FDamageSpec& DamageSpec)
{
	return HealthComponent ? HealthComponent->ApplyDamageSpec(DamageSpec) : FGameplayCommandResult::Unhandled();
}

bool AEnemyBase::IsTargetable_Implementation() const
{
	return HealthComponent && !HealthComponent->IsDead();
}

EGameplayTeam AEnemyBase::GetGameplayTeam_Implementation() const
{
	return Team;
}

void AEnemyBase::HandleDeath(UHealthComponent* InHealthComponent)
{
	if (BrainComponent)
	{
		BrainComponent->SetDead(true);
	}

	SetActorEnableCollision(false);
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->DisableMovement();
	}

	if (UGameplayEventBus* EventBus = GetWorld() ? GetWorld()->GetSubsystem<UGameplayEventBus>() : nullptr)
	{
		FGameplayEvent Event = FGameplayEvent::Make(EGameplayEventType::Death);
		Event.SourceActor = this;
		EventBus->QueueEvent(Event);
	}

	SetLifeSpan(2.0f);
}
