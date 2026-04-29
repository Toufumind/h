#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EncounterSpawnPointComponent.generated.h"

class UEncounterManagerComponent;

UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent))
class SPRITEASSEMBLE_API UEncounterSpawnPointComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEncounterSpawnPointComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Encounter")
	FName SpawnPointTag = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|Encounter")
	bool bRegisterOnBeginPlay = true;

protected:
	virtual void BeginPlay() override;

private:
	UEncounterManagerComponent* FindEncounterManager() const;
};
