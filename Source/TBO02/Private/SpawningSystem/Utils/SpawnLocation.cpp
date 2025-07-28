#include "SpawningSystem/Utils/SpawnLocation.h"

#include "Components/BillboardComponent.h"

ASpawnLocation::ASpawnLocation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR

	static ConstructorHelpers::FObjectFinder<UTexture2D> SpriteTexture(
		TEXT("/Engine/EditorResources/Ai_Spawnpoint.Ai_Spawnpoint"));
	if (SpriteTexture.Succeeded())
		GetSpriteComponent()->SetSprite(SpriteTexture.Object);

#endif // WITH_EDITOR
}
