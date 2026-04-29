#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RunConfigWorldSubsystem.generated.h"

class URunConfigDefinition;

UCLASS(Config = Game)
class SPRITEASSEMBLE_API URunConfigWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Run")
	void SetActiveRunConfig(URunConfigDefinition* InRunConfig);

	UFUNCTION(BlueprintPure, Category = "Gameplay|Run")
	URunConfigDefinition* GetActiveRunConfig() const { return ActiveRunConfig; }

	UFUNCTION(BlueprintPure, Category = "Gameplay|Run")
	UObject* FindConfigObject(FName ConfigKey) const;

	template <typename TObjectClass>
	TObjectClass* FindConfigObject(FName ConfigKey) const
	{
		return Cast<TObjectClass>(FindConfigObject(ConfigKey));
	}

private:
	UPROPERTY(Config, EditAnywhere, Category = "Gameplay|Run")
	TSoftObjectPtr<URunConfigDefinition> DefaultRunConfig;

	UPROPERTY()
	TObjectPtr<URunConfigDefinition> ActiveRunConfig = nullptr;
};
