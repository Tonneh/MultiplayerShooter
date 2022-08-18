// Fill out your copyright notice in the Description page of Project Settings.

#include "TeamsGameMode.h"
#include "MultiplayerShooter/GameState/ShooterGameState.h"
#include "MultiplayerShooter/PlayerState/ShooterPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/PlayerController/ShooterPlayerController.h"

 ATeamsGameMode::ATeamsGameMode()
{
	 bTeamsMatch = true;
}

void ATeamsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AShooterGameState* ShooterGameState = Cast<AShooterGameState>(UGameplayStatics::GetGameState(this));
	if (ShooterGameState)
	{
		AShooterPlayerState* ShooterPlayerState = NewPlayer->GetPlayerState<AShooterPlayerState>();
		if (ShooterPlayerState && ShooterPlayerState->GetTeam() == ETeam::ET_NoTeam)
		{
			if (ShooterGameState->BlueTeam.Num() >= ShooterGameState->RedTeam.Num())
			{
				ShooterGameState->RedTeam.AddUnique(ShooterPlayerState);
				ShooterPlayerState->SetTeam(ETeam::ET_RedTeam);
			}
			else
			{
				ShooterGameState->BlueTeam.AddUnique(ShooterPlayerState);
				ShooterPlayerState->SetTeam(ETeam::ET_BlueTeam);
			}
		}
	}
}

void ATeamsGameMode::Logout(AController* Exiting)
{
	AShooterGameState* ShooterGameState = Cast<AShooterGameState>(UGameplayStatics::GetGameState(this));
	AShooterPlayerState* ShooterPlayerState = Exiting->GetPlayerState<AShooterPlayerState>();
	if (ShooterGameState && ShooterPlayerState)
	{
		if (ShooterGameState->RedTeam.Contains(ShooterPlayerState))
		{
			ShooterGameState->RedTeam.Remove(ShooterPlayerState);
		}
		if (ShooterGameState->BlueTeam.Contains(ShooterPlayerState))
		{
			ShooterGameState->BlueTeam.Remove(ShooterPlayerState);
		}
	}
}

float ATeamsGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	AShooterPlayerState* AttackerState = Attacker->GetPlayerState<AShooterPlayerState>();
	AShooterPlayerState* VictimState = Victim->GetPlayerState<AShooterPlayerState>();
	if (AttackerState == nullptr || VictimState == nullptr)
		return BaseDamage;
	if (VictimState == AttackerState)
		return BaseDamage;
	if (AttackerState->GetTeam() == VictimState->GetTeam())
		return 0.f; 
	return BaseDamage;
}

void ATeamsGameMode::PlayerEliminated(AShooterCharacter* ElimmedCharacter, AShooterPlayerController* VictimController, AShooterPlayerController* AttackerController)
{
	Super::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);
	
	AShooterGameState* ShooterGameState = Cast<AShooterGameState>(UGameplayStatics::GetGameState(this));
	AShooterPlayerState* AttackerPlayerState = AttackerController ? Cast<AShooterPlayerState>(AttackerController->PlayerState) : nullptr;
	if (ShooterGameState && AttackerPlayerState)
	{
		if (AttackerPlayerState->GetTeam() == ETeam::ET_BlueTeam)
		{
			ShooterGameState->BlueTeamScores();
		}
		if (AttackerPlayerState->GetTeam() == ETeam::ET_RedTeam)
		{
			ShooterGameState->RedTeamScores();
		}
	}
}

void ATeamsGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	AShooterGameState* ShooterGameState = Cast<AShooterGameState>(UGameplayStatics::GetGameState(this));
	if (ShooterGameState)
	{
		for (auto PlayerState : ShooterGameState->PlayerArray)
		{
			AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState.Get());
			if (ShooterPlayerState && ShooterPlayerState->GetTeam() == ETeam::ET_NoTeam)
			{
				if (ShooterGameState->BlueTeam.Num() >= ShooterGameState->RedTeam.Num())
				{
					ShooterGameState->RedTeam.AddUnique(ShooterPlayerState);
					ShooterPlayerState->SetTeam(ETeam::ET_RedTeam);
				}
				else
				{
					ShooterGameState->BlueTeam.AddUnique(ShooterPlayerState);
					ShooterPlayerState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
		}
	}
}
