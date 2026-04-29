#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "../Core/GameplayCommand.h"
#include "AreaEffectTypes.h"
#include "AreaEffectWorldSubsystem.generated.h"

class UGameplayCommandDispatcher;

UCLASS()
class SPRITEASSEMBLE_API UAreaEffectWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|AreaEffect")
	FGuid CreateAreaEffect(const FAreaEffectSpec& AreaEffectSpec);

private:
	UPROPERTY()
	TArray<FAreaEffectInstance> AreaEffects;

	UPROPERTY(EditAnywhere, Category = "Gameplay|AreaEffect")
	float DamageTickInterval = 1.0f;

	FDelegateHandle CreateAreaEffectHandle;

	FGameplayCommandResult HandleCreateAreaEffect(const FGameplayCommand& Command);
	UGameplayCommandDispatcher* GetCommandDispatcher() const;

	void TickAreaEffect(FAreaEffectInstance& AreaEffect, float DeltaTime);
	void ApplyAreaEffectTick(const FAreaEffectInstance& AreaEffect);
	bool ShouldAffectActor(const FAreaEffectInstance& AreaEffect, AActor* Candidate) const;
	void RemoveDeadAreaEffects();
};
