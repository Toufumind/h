#include "PlayableRunPlayerController.h"

#include "UICommandControllerComponent.h"

#include "../Core/GameplaySpecs.h"
#include "../Enemy/EnemyBase.h"
#include "../Enemy/EnemyDefinition.h"
#include "../ThreeC/PlayerMotorComponent.h"
#include "../Weapon/ShooterComponent.h"

#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"

void APlayableRunPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

void APlayableRunPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	float HorizontalInput = 0.0f;
	if (IsInputKeyDown(EKeys::A) || IsInputKeyDown(EKeys::Left))
	{
		HorizontalInput -= 1.0f;
	}
	if (IsInputKeyDown(EKeys::D) || IsInputKeyDown(EKeys::Right))
	{
		HorizontalInput += 1.0f;
	}

	if (FMath::Abs(HorizontalInput) > HorizontalInputDeadZone)
	{
		if (UPlayerMotorComponent* MotorComponent = ControlledPawn->FindComponentByClass<UPlayerMotorComponent>())
		{
			MotorComponent->MoveHorizontal(HorizontalInput);
		}
		else
		{
			ControlledPawn->AddMovementInput(FVector::ForwardVector, HorizontalInput);
			SetControlRotation(HorizontalInput > 0.0f ? FRotator::ZeroRotator : FRotator(0.0f, 180.0f, 0.0f));
		}
	}
}

void APlayableRunPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!InputComponent)
	{
		return;
	}

	InputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &APlayableRunPlayerController::JumpPressed);
	InputComponent->BindKey(EKeys::SpaceBar, IE_Released, this, &APlayableRunPlayerController::JumpReleased);
	InputComponent->BindKey(EKeys::W, IE_Pressed, this, &APlayableRunPlayerController::JumpPressed);
	InputComponent->BindKey(EKeys::W, IE_Released, this, &APlayableRunPlayerController::JumpReleased);
	InputComponent->BindKey(EKeys::J, IE_Pressed, this, &APlayableRunPlayerController::FirePressed);
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &APlayableRunPlayerController::FirePressed);
	InputComponent->BindKey(EKeys::T, IE_Pressed, this, &APlayableRunPlayerController::SpawnTestEnemy);
	InputComponent->BindKey(EKeys::Enter, IE_Pressed, this, &APlayableRunPlayerController::StartEncounter);
	InputComponent->BindKey(EKeys::One, IE_Pressed, this, &APlayableRunPlayerController::ClaimRewardOne);
	InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &APlayableRunPlayerController::ClaimRewardTwo);
	InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &APlayableRunPlayerController::ClaimRewardThree);
	InputComponent->BindKey(EKeys::R, IE_Pressed, this, &APlayableRunPlayerController::RestartCurrentLevel);
}

void APlayableRunPlayerController::JumpPressed()
{
	if (APawn* ControlledPawn = GetPawn())
	{
		if (UPlayerMotorComponent* MotorComponent = ControlledPawn->FindComponentByClass<UPlayerMotorComponent>())
		{
			MotorComponent->RequestJump();
			return;
		}
	}

	if (ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn()))
	{
		ControlledCharacter->Jump();
	}
}

void APlayableRunPlayerController::JumpReleased()
{
	if (APawn* ControlledPawn = GetPawn())
	{
		if (UPlayerMotorComponent* MotorComponent = ControlledPawn->FindComponentByClass<UPlayerMotorComponent>())
		{
			MotorComponent->StopJump();
			return;
		}
	}

	if (ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn()))
	{
		ControlledCharacter->StopJumping();
	}
}

void APlayableRunPlayerController::FirePressed()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	UShooterComponent* ShooterComponent = ControlledPawn->FindComponentByClass<UShooterComponent>();
	if (!ShooterComponent)
	{
		return;
	}

	const FVector Direction = ControlledPawn->GetActorForwardVector().X >= 0.0f ? FVector::ForwardVector : -FVector::ForwardVector;
	const FVector ShotSpawnLocation = ControlledPawn->GetActorLocation() + Direction * 85.0f + FVector(0.0f, 0.0f, 35.0f);

	FDamageSpec DamageSpec;
	DamageSpec.SourceActor = ControlledPawn;
	DamageSpec.InstigatorActor = ControlledPawn;
	DamageSpec.Team = EGameplayTeam::Player;
	DamageSpec.BaseDamage = 20.0f;
	DamageSpec.FinalDamage = 20.0f;

	FShotSpawnSpec ShotSpawnSpec;
	ShotSpawnSpec.ShotCount = 1;
	ShotSpawnSpec.SpawnTransform = FTransform(ShotSpawnLocation);
	ShotSpawnSpec.Direction = Direction;
	ShotSpawnSpec.Speed = 1200.0f;
	ShotSpawnSpec.Radius = 12.0f;
	ShotSpawnSpec.Lifetime = 1.2f;
	ShotSpawnSpec.VisualType = TEXT("Manual");
	ShotSpawnSpec.OwnerActor = ControlledPawn;
	ShotSpawnSpec.InstigatorActor = ControlledPawn;
	ShotSpawnSpec.Team = EGameplayTeam::Player;
	ShotSpawnSpec.DamageSpec = DamageSpec;

	ShooterComponent->SubmitShotSpawnSpec(ShotSpawnSpec);
}

void APlayableRunPlayerController::SpawnTestEnemy()
{
	UWorld* World = GetWorld();
	APawn* ControlledPawn = GetPawn();
	if (!World || !ControlledPawn)
	{
		return;
	}

	const FVector Direction = ControlledPawn->GetActorForwardVector().X >= 0.0f ? FVector::ForwardVector : -FVector::ForwardVector;
	const FVector EnemySpawnLocation = ControlledPawn->GetActorLocation() + Direction * TestEnemySpawnDistance;
	const FRotator SpawnRotation = Direction.X >= 0.0f ? FRotator(0.0f, 180.0f, 0.0f) : FRotator::ZeroRotator;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AEnemyBase* Enemy = World->SpawnActor<AEnemyBase>(AEnemyBase::StaticClass(), EnemySpawnLocation, SpawnRotation, SpawnParameters);
	if (!Enemy)
	{
		return;
	}

	UEnemyDefinition* EnemyDefinition = LoadObject<UEnemyDefinition>(nullptr, TEXT("/Game/Data/Enemies/DA_Enemy_BasicMelee.DA_Enemy_BasicMelee"));
	Enemy->ApplyDefinition(EnemyDefinition);
}

void APlayableRunPlayerController::StartEncounter()
{
	if (UUICommandControllerComponent* CommandController = FindCommandController())
	{
		CommandController->SelectNode(PlayableNodeId);
	}
}

void APlayableRunPlayerController::ClaimRewardOne()
{
	ClaimReward(0);
}

void APlayableRunPlayerController::ClaimRewardTwo()
{
	ClaimReward(1);
}

void APlayableRunPlayerController::ClaimRewardThree()
{
	ClaimReward(2);
}

void APlayableRunPlayerController::RestartCurrentLevel()
{
	if (GetWorld())
	{
		UGameplayStatics::OpenLevel(this, RestartMapName, false);
	}
}

void APlayableRunPlayerController::ClaimReward(int32 RewardIndex)
{
	if (UUICommandControllerComponent* CommandController = FindCommandController())
	{
		CommandController->ClaimReward(RewardIndex);
	}
}

UUICommandControllerComponent* APlayableRunPlayerController::FindCommandController() const
{
	if (const APawn* ControlledPawn = GetPawn())
	{
		if (UUICommandControllerComponent* Component = ControlledPawn->FindComponentByClass<UUICommandControllerComponent>())
		{
			return Component;
		}
	}

	if (!GetWorld())
	{
		return nullptr;
	}

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		if (UUICommandControllerComponent* Component = It->FindComponentByClass<UUICommandControllerComponent>())
		{
			return Component;
		}
	}

	return nullptr;
}
