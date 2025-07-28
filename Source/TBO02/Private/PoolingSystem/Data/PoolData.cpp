#include "PoolingSystem/Data/PoolData.h"

#if WITH_EDITOR
void UPoolData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (MaxPoolSize < PoolSize)
	{
		// Ensure MaxPoolSize is at least equal to PoolSize
		MaxPoolSize = PoolSize;
	}
}
#endif