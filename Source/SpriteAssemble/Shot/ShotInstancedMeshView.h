#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShotTypes.h"
#include "ShotInstancedMeshView.generated.h"

class UInstancedStaticMeshComponent;
class UStaticMesh;

UCLASS()
class SPRITEASSEMBLE_API AShotInstancedMeshView : public AActor
{
	GENERATED_BODY()

public:
	AShotInstancedMeshView();

	virtual void BeginPlay() override;

	int32 AddShotVisual(const FShotInstance& ShotInstance);
	void UpdateShotVisual(int32 InstanceIndex, const FShotInstance& ShotInstance);
	void RemoveShotVisual(int32 InstanceIndex);
	void ClearShotVisuals();

private:
	UPROPERTY(VisibleAnywhere, Category = "Shot")
	TObjectPtr<UInstancedStaticMeshComponent> InstancedMeshComponent;

	UPROPERTY(EditAnywhere, Category = "Shot")
	TObjectPtr<UStaticMesh> DefaultMesh = nullptr;

	FTransform BuildTransform(const FShotInstance& ShotInstance) const;
	void EnsureDefaultMesh();
};
