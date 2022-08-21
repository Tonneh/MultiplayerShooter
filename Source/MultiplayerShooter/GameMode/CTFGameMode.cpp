// Fill out your copyright notice in the Description page of Project Settings.


#include "CTFGameMode.h"
#include "MultiplayerShooter/Weapons/Flag.h"
#include "MultiplayerShooter/CTF/FlagZone.h"
#include "MultiplayerShooter/GameState/ShooterGameState.h"

void ACTFGameMode::PlayerEliminated(AShooterCharacter* ElimmedCharacter, AShooterPlayerController* VictimController, AShooterPlayerController* AttackerController)
{
	AShooterGameMode::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);
}

void ACTFGameMode::FlagCaptured(AFlag* Flag, AFlagZone* FlagZone)
{
	bool bValidCapture = Flag->GetTeam() != FlagZone->Team;
	AShooterGameState* bGameState = Cast<AShooterGameState>(GameState);
	if (bGameState)
	{
		if (FlagZone->Team == ETeam::ET_BlueTeam)
		{
			bGameState->BlueTeamScores();
		}
		if (FlagZone->Team == ETeam::ET_RedTeam)
		{
			bGameState->RedTeamScores();
		}
	}

}
