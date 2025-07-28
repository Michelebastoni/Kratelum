// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/DataAssets/Behaviors_DA.h"

FBurstFromBossData UBehaviorProgression_BurstFromBoss::EvalAllAt(float x)
{
	float DurationAt = Duration.Eval(x, "");
	float DelayAt = SpawnDelay.Eval(x, "");
	float AngleAt = AngleVariance.Eval(x, "");
	float SpeedVarianceAt = SpeedVarianceMult.Eval(x, "");
	float SpeedAt = BaseSpeed.Eval(x, "");

	return FBurstFromBossData(DurationAt, DelayAt, AngleAt, SpeedVarianceAt, SpeedAt);
}

FMinionLaserData UBehaviorProgression_MinionLasers::EvalAllAt(float x)
{
	float LaserSpeedAt = LaserSpeed.Eval(x, "");
	int HowManyAt = static_cast<int>(HowMany.Eval(x, ""));
	float DelayBetweenAt = DelayBewteen.Eval(x, "");
	float AnticipationDurationAt = AnticipationDuration.Eval(x, "");

	return FMinionLaserData(LaserSpeedAt, HowManyAt, DelayBetweenAt, AnticipationDurationAt);
}

FSpiralData UBehaviorProgression_Spiral::EvalAllAt(float x)
{
	float DurationAt = Duration.Eval(x, "");
	float FreqAt = Frequency.Eval(x, "");
	float ProbAt = ChangeDirProb.Eval(x, "");
	float AngleAt = RotationAngle.Eval(x, "");
	float SpeedAt = ProjectileSpeed.Eval(x, "");
	int PointCountAt = static_cast<int>(PointCount.Eval(x, ""));
	
	return FSpiralData(DurationAt, FreqAt, ProbAt, AngleAt, SpeedAt, PointCountAt); 
}

FLaserBehaviorData UBehaviorProgression_Laser::EvalAllAt(float x)
{
	float TimeTo360At = TimeTo360.Eval(x, "");
	int NumLasersAt = static_cast<int>(NumOfLasers.Eval(x, ""));
	int NumRotationsAt = static_cast<int>(NumOfRotations.Eval(x, ""));

	return FLaserBehaviorData(TimeTo360At, NumLasersAt, NumRotationsAt);
}
