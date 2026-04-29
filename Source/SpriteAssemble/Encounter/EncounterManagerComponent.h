#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Core/GameplayCommand.h"
#include "EncounterManagerComponent.generated.h"

class UEncounterConfig;
class USpawnDirector;
class UWaveDirector;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEncounterStateChangedSignature, UEncounterManagerComponent*, EncounterManager);

UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent))
class SPRITEASSEMBLE_API UEncounterManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEncounterManagerComponent();

	UPROPERTY(BlueprintAssignable, Category = "Gameplay|Encounter")
	FEncounterStateChangedSignature OnEncounterCompleted;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Encounter")
	void StartEncounter(UEncounterConfig* InEncounterConfig);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Encounter")
	void RegisterSpawnPoint(AActor* SpawnPoint);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Encounter")
	void ClearSpawnPoints();

	UFUNCTION(BlueprintPure, Category = "Gameplay|Encounter")
	bool IsEncounterRunning() const { return bRunning; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, Category = "Gameplay|Encounter")
	TObjectPtr<UEncounterConfig> DefaultEncounterConfig = nullptr;

	UPROPERTY()
	TObjectPtr<UEncounterConfig> EncounterConfig = nullptr;

	UPROPERTY()
	TObjectPtr<UWaveDirector> WaveDirector = nullptr;

	UPROPERTY()
	TObjectPtr<USpawnDirector> SpawnDirector = nullptr;

	TArray<TWeakObjectPtr<AActor>> SpawnPoints;
	TArray<TWeakObjectPtr<AActor>> AliveEnemies;

	int32 CurrentWaveIndex = INDEX_NONE;
	bool bRunning = false;
	bool bEncounterCompleted = false;
	FDelegateHandle StartEncounterHandle;

	FGameplayCommandResult HandleStartEncounterCommand(const FGameplayCommand& Command);
	void StartNextWave();
	void RegisterSpawnedEnemy(AActor* Enemy);
	void PruneDeadEnemies();
	bool IsCurrentWaveCleared() const;
	void CompleteEncounter();
};
