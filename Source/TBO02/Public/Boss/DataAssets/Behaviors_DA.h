// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Behaviors_DA.generated.h"

USTRUCT(BlueprintType)
struct FBurstFromBossData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AngleVariance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpeedVarianceMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseSpeed;

	FBurstFromBossData()
	{
		Duration = 0.0f;
		SpawnDelay = 0.0f;
		AngleVariance = 0.0f;
		SpeedVarianceMult = 0.0f;
		BaseSpeed = 0.0f;
	}

	FBurstFromBossData(const float Duration, const float SpawnDelay, const float AngleVariance,
	                   const float SpeedVarianceMult, const float BaseSpeed)
		: Duration(Duration),
		  SpawnDelay(SpawnDelay),
		  AngleVariance(AngleVariance),
		  SpeedVarianceMult(SpeedVarianceMult),
		  BaseSpeed(BaseSpeed)
	{
	}
};

USTRUCT(BlueprintType)
struct FMinionLaserData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LaserSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int HowMany;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DelayBetween;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnticipationDuration;

	FMinionLaserData()
	{
		LaserSpeed = 0.0f;
		HowMany = 1;
		DelayBetween = 0.0f;
		AnticipationDuration = 0.0f;
	}

	FMinionLaserData(const float LaserSpeed, const int HowMany, const float DelayBetween,
	                 const float AnticipationDuration)
		: LaserSpeed(LaserSpeed),
		  HowMany(HowMany),
		  DelayBetween(DelayBetween),
		  AnticipationDuration(AnticipationDuration)
	{
	}
};

USTRUCT(BlueprintType)
struct FSpiralData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Duration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Frequency;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ChangeDirProb;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RotationAngle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ProjectileSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int PointCount;

	FSpiralData()
	{
		Duration = 0.0f;
		Frequency = 0.0f;
		ChangeDirProb = 0.0f;
		RotationAngle = 0.0f;
		ProjectileSpeed = 0.0f;
		PointCount = 0;
	}

	FSpiralData(const float Duration, const float Frequency, const float ChangeDirProb, const float RotationAngle,
		const float ProjectileSpeed, const int PointCount)
		: Duration(Duration),
		  Frequency(Frequency),
		  ChangeDirProb(ChangeDirProb),
		  RotationAngle(RotationAngle),
		  ProjectileSpeed(ProjectileSpeed),
		  PointCount(PointCount)
	{
	}
};

USTRUCT(BlueprintType)
struct FLaserBehaviorData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TimeTo360;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int NumOfLasers;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int NumOfRotations;

	FLaserBehaviorData()
	{
		TimeTo360 = 0.0f;
		NumOfLasers = 0;
		NumOfRotations = 0;
	}

	FLaserBehaviorData(const float TimeTo360, const int NumOfLasers, const int NumOfRotations)
		: TimeTo360(TimeTo360),
		  NumOfLasers(NumOfLasers),
		  NumOfRotations(NumOfRotations)
	{
	}
};

UCLASS()
class UBehaviorProgression_BurstFromBoss : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCurveTableRowHandle Duration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCurveTableRowHandle SpawnDelay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCurveTableRowHandle AngleVariance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCurveTableRowHandle SpeedVarianceMult;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCurveTableRowHandle BaseSpeed;

	// Functions
	UFUNCTION(BlueprintCallable)
	FBurstFromBossData EvalAllAt(float x);
};

UCLASS()
class UBehaviorProgression_MinionLasers : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCurveTableRowHandle LaserSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCurveTableRowHandle HowMany;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCurveTableRowHandle DelayBewteen;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCurveTableRowHandle AnticipationDuration;

	// Functions
	UFUNCTION(BlueprintCallable)
	FMinionLaserData EvalAllAt(float x);
};

UCLASS()
class UBehaviorProgression_Spiral : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCurveTableRowHandle Duration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCurveTableRowHandle Frequency;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCurveTableRowHandle ChangeDirProb;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCurveTableRowHandle RotationAngle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCurveTableRowHandle ProjectileSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCurveTableRowHandle PointCount;

	// Functions
	UFUNCTION(BlueprintCallable)
	FSpiralData EvalAllAt(float x);
};

UCLASS()
class UBehaviorProgression_Laser : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCurveTableRowHandle TimeTo360;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCurveTableRowHandle NumOfLasers;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCurveTableRowHandle NumOfRotations;

	// Functions
	UFUNCTION(BlueprintCallable)
	FLaserBehaviorData EvalAllAt(float x);
};