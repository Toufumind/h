#include "SpriteAssembleAltar.h"
#include "Components/BoxComponent.h"
#include "SpriteAssembleCharacter.h"

ASpriteAssembleAltar::ASpriteAssembleAltar()
{
	PrimaryActorTick.bCanEverTick = false;
	InteractVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractVolume"));
	RootComponent = InteractVolume;
	InteractVolume->OnComponentBeginOverlap.AddDynamic(this, &ASpriteAssembleAltar::OnOverlapBegin);
	InteractVolume->OnComponentEndOverlap.AddDynamic(this, &ASpriteAssembleAltar::OnOverlapEnd);
}

void ASpriteAssembleAltar::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASpriteAssembleCharacter* Player = Cast<ASpriteAssembleCharacter>(OtherActor))
	{
		Player->bIsNearAltar = true;
	}
}

void ASpriteAssembleAltar::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ASpriteAssembleCharacter* Player = Cast<ASpriteAssembleCharacter>(OtherActor))
	{
		Player->bIsNearAltar = false;
	}
}