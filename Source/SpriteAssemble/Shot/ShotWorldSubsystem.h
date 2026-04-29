#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "../Core/GameplayCommand.h"
#include "ShotTypes.h"
#include "ShotWorldSubsystem.generated.h"

class AShotInstancedMeshView;

UCLASS()
class SPRITEASSEMBLE_API UShotWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Shot")
	TArray<FShotId> SubmitShotSpawnSpec(const FShotSpawnSpec& SpawnSpec);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Shot")
	bool IsShotAlive(FShotId ShotId) const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Shot")
	void KillShot(FShotId ShotId);

private:
	UPROPERTY()
	TObjectPtr<AShotInstancedMeshView> MeshView = nullptr;

	TArray<FShotInstance> Shots;
	TArray<int32> FreeIndices;
	TMap<int32, int32> GenerationsByIndex;
	TMap<FShotId, int32> ShotIdToInstanceIndex;

	FShotId AllocateShotId();
	FShotInstance* FindShot(FShotId ShotId);
	const FShotInstance* FindShot(FShotId ShotId) const;

	void UpdateShots(float DeltaTime);
	void ProcessShotCollision(FShotInstance& Shot);
	bool ShouldHitActor(const FShotInstance& Shot, AActor* Candidate) const;
	void ResolveShotHit(FShotInstance& Shot, AActor* TargetActor, const FHitResult& HitResult);
	void DispatchGeneratedCommands(const TArray<FGameplayCommand>& Commands) const;
	void RemoveDeadShots();
	void SyncVisuals();
	void EnsureMeshView();
};
