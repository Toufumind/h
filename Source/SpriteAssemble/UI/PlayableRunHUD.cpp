#include "PlayableRunHUD.h"

#include "RewardViewModelWorldSubsystem.h"
#include "RunViewModelWorldSubsystem.h"
#include "ViewModelTypes.h"

#include "../Combat/HealthComponent.h"
#include "../ThreeC/DamageReactionComponent.h"
#include "../ThreeC/PlayerMotorComponent.h"

#include "Engine/Canvas.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

namespace
{
const TCHAR* BoolText(bool bValue)
{
	return bValue ? TEXT("Y") : TEXT("N");
}
}

void APlayableRunHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas || !GetWorld())
	{
		return;
	}

	float Y = 32.0f;
	DrawLineText(TEXT("Sprite Assemble"), Y, FLinearColor::White, 1.35f);

	FRunViewSnapshot RunSnapshot;
	if (const URunViewModelWorldSubsystem* RunViewModel = GetWorld()->GetSubsystem<URunViewModelWorldSubsystem>())
	{
		RunSnapshot = RunViewModel->GetSnapshot();
	}

	const UEnum* PhaseEnum = StaticEnum<ERunPhase>();
	const FString PhaseName = PhaseEnum ? PhaseEnum->GetNameStringByValue(static_cast<int64>(RunSnapshot.CurrentPhase)) : TEXT("Unknown");
	DrawLineText(FString::Printf(TEXT("Phase: %s"), *PhaseName), Y, FLinearColor(0.82f, 0.92f, 1.0f));
	DrawLineText(FString::Printf(TEXT("Red Core: %d    Gem Slots: %d    Completed: %d"),
		RunSnapshot.RedCore,
		RunSnapshot.GemSlotCount,
		RunSnapshot.CompletedNodeCount), Y, FLinearColor(0.85f, 1.0f, 0.78f));
	DrawLineText(TEXT("Move: A/D or Arrow Keys    Jump: Space/W    Fire: J/LMB    Spawn Enemy: T    Restart: R"), Y, FLinearColor(0.80f, 0.84f, 0.92f));

	if (const APlayerController* OwningController = PlayerOwner)
	{
		const APawn* Pawn = OwningController->GetPawn();
		const UHealthComponent* HealthComponent = Pawn ? Pawn->FindComponentByClass<UHealthComponent>() : nullptr;
		const UDamageReactionComponent* DamageReactionComponent = Pawn ? Pawn->FindComponentByClass<UDamageReactionComponent>() : nullptr;
		const UPlayerMotorComponent* PlayerMotorComponent = Pawn ? Pawn->FindComponentByClass<UPlayerMotorComponent>() : nullptr;
		const bool bIsInvincible = DamageReactionComponent && DamageReactionComponent->IsInvincible();
		const bool bIsDead = (DamageReactionComponent && DamageReactionComponent->IsDead()) || (HealthComponent && HealthComponent->IsDead());

		DrawLineText(FString::Printf(TEXT("3C  HSpeed: %.0f    Falling: %s    Coyote: %s    JumpBuf: %s    Invincible: %s    Dead: %s"),
			PlayerMotorComponent ? PlayerMotorComponent->GetHorizontalSpeed() : 0.0f,
			BoolText(PlayerMotorComponent && PlayerMotorComponent->IsFalling()),
			BoolText(PlayerMotorComponent && PlayerMotorComponent->IsUsingCoyoteTime()),
			BoolText(PlayerMotorComponent && PlayerMotorComponent->IsJumpBuffered()),
			BoolText(bIsInvincible),
			BoolText(bIsDead)), Y, FLinearColor(1.0f, 0.88f, 0.42f));

		if (HealthComponent && HealthComponent->IsDead())
		{
			Y += 16.0f;
			DrawLineText(TEXT("YOU DIED"), Y, FLinearColor(1.0f, 0.15f, 0.1f), 1.8f);
			DrawLineText(TEXT("Press R to restart."), Y, FLinearColor::Yellow, 1.1f);
			return;
		}
		if (bIsInvincible)
		{
			DrawLineText(TEXT("Invincible"), Y, FLinearColor(0.45f, 0.85f, 1.0f));
		}
	}

	TArray<FRewardOptionViewSnapshot> RewardOptions;
	if (const URewardViewModelWorldSubsystem* RewardViewModel = GetWorld()->GetSubsystem<URewardViewModelWorldSubsystem>())
	{
		RewardOptions = RewardViewModel->GetRewardOptions();
	}

	Y += 12.0f;
	if (RewardOptions.Num() > 0)
	{
		DrawLineText(TEXT("Choose reward:"), Y, FLinearColor::Yellow, 1.1f);
		for (const FRewardOptionViewSnapshot& Option : RewardOptions)
		{
			DrawLineText(FString::Printf(TEXT("%d  %s x%d"),
				Option.OptionIndex + 1,
				*Option.RewardSpec.RewardId.ToString(),
				Option.RewardSpec.Quantity), Y, FLinearColor::White);
		}
	}
	else if (RunSnapshot.CurrentPhase == ERunPhase::MapSelection)
	{
		DrawLineText(TEXT("Press Enter to start the next encounter."), Y, FLinearColor::Yellow);
	}
	else if (RunSnapshot.CurrentPhase == ERunPhase::Combat)
	{
		DrawLineText(TEXT("Combat in progress."), Y, FLinearColor::Yellow);
	}
	else
	{
		DrawLineText(TEXT("Run is waiting for the next step."), Y, FLinearColor::Yellow);
	}
}

void APlayableRunHUD::DrawLineText(const FString& Text, float& Y, const FLinearColor& Color, float Scale) const
{
	if (!Canvas)
	{
		return;
	}

	Canvas->SetDrawColor(Color.ToFColor(true));
	Canvas->DrawText(GEngine->GetSmallFont(), Text, 32.0f, Y, Scale, Scale);
	Y += 20.0f * Scale;
}
