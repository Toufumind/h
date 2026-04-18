#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpriteAssembleAltar.generated.h"

UCLASS()
class SPRITEASSEMBLE_API ASpriteAssembleAltar : public AActor
{
	GENERATED_BODY()
public:
	ASpriteAssembleAltar();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* InteractVolume;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};