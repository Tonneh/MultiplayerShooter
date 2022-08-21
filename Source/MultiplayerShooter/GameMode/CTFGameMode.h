// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TeamsGameMode.h"
#include "CTFGameMode.generated.h"

class AFlag; 
class AFlagZone;

UCLASS()
class MULTIPLAYERSHOOTER_API ACTFGameMode : public ATeamsGameMode
{
	GENERATED_BODY()
public:
	virtual void PlayerEliminated(class AShooterCharacter* ElimmedCharacter, class AShooterPlayerController* VictimController, AShooterPlayerController* AttackerController) override;
	void FlagCaptured(AFlag* Flag, AFlagZone* FlagZone);
};
