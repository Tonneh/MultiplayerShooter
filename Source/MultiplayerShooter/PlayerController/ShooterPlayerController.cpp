// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"
#include "MultiplayerShooter/HUD/ShooterHUD.h"
#include "MultiplayerShooter/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MultiplayerShooter/Character/ShooterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerShooter/GameMode/ShooterGameMode.h"
#include "MultiplayerShooter/HUD/Announcement.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/ShooterComponents/CombatComponent.h"
#include "MultiplayerShooter/GameState/ShooterGameState.h"
#include "MultiplayerShooter/PlayerState/ShooterPlayerState.h"

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay(); 

	ShooterHUD = Cast<AShooterHUD>(GetHUD());
	ServerCheckMatchState();
}

void AShooterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();
}

void AShooterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AShooterPlayerController, MatchState);
}

void AShooterPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void AShooterPlayerController::SetHUDHP(float Health, float MaxHealth)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	if (ShooterHUD && ShooterHUD->CharacterOverlay && ShooterHUD->CharacterOverlay->HealthBar && ShooterHUD->CharacterOverlay->HealthText)
	{
		const float HealthPercent = Health / MaxHealth; 
		ShooterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d"), FMath::CeilToInt(Health));
		ShooterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDHealth = Health; 
		HUDMaxHealth = MaxHealth;
	}
}

void AShooterPlayerController::SetHUDScore(float Score)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	if (ShooterHUD && ShooterHUD->CharacterOverlay && ShooterHUD->CharacterOverlay->ScoreAmount)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::CeilToInt(Score));
		ShooterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	else
	{
		bInitializeCharacterOverlay = true; 
		HUDScore = Score; 
	}
}

void AShooterPlayerController::SetHUDDeaths(int32 Deaths)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	if (ShooterHUD && ShooterHUD->CharacterOverlay && ShooterHUD->CharacterOverlay->DeathAmount)
	{
		FString DeathText = FString::Printf(TEXT("%d"), Deaths);
		ShooterHUD->CharacterOverlay->DeathAmount->SetText(FText::FromString(DeathText));
	}
	else
	{
		bInitializeCharacterOverlay = true; 
		HUDDeaths = Deaths;
	}
}

void AShooterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	if (ShooterHUD && ShooterHUD->CharacterOverlay && ShooterHUD->CharacterOverlay->WeaponAmmoAmount)
	{
		FString AmmoText = FString::Printf(TEXT("%02d|"), Ammo);
		ShooterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void AShooterPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	if (ShooterHUD && ShooterHUD->CharacterOverlay && ShooterHUD->CharacterOverlay->CarriedAmmoAmount)
	{
		FString CarriedAmmoText = FString::Printf(TEXT("%02d"), Ammo);
		ShooterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(CarriedAmmoText));
	}
}

void AShooterPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	if (ShooterHUD && ShooterHUD->CharacterOverlay && ShooterHUD->CharacterOverlay->MatchCountdownText)
	{
		if (CountdownTime <= 0.f)
		{
			ShooterHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60; 
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		ShooterHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void AShooterPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	if (ShooterHUD && ShooterHUD->Announcement && ShooterHUD->Announcement->WarmupTime)
	{
		if (CountdownTime <= 0.f)
		{
			ShooterHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		ShooterHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void AShooterPlayerController::SetHUDTime()
{
	// check if we on server, if so then we may get the gamemode and set the level starting time
	// since on the server begin() in player coontroller is called b4 the beginplay in gamemode, so we need to
	// retrieve the LevelStartingTime from gamemode. 
	if (HasAuthority())
	{
		ShooterGameMode = ShooterGameMode == nullptr ? Cast<AShooterGameMode>(UGameplayStatics::GetGameMode(this)) : ShooterGameMode;
		if (ShooterGameMode)
		{
			LevelStartingTime = ShooterGameMode->LevelStartingTime;
		}
	}
	
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::Cooldown) TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	if (HasAuthority())
	{
		ShooterGameMode = ShooterGameMode == nullptr ? Cast<AShooterGameMode>(UGameplayStatics::GetGameMode(this)) : ShooterGameMode;
		if (ShooterGameMode)
		{
			SecondsLeft = FMath::CeilToInt(ShooterGameMode->GetCountdownTime() + LevelStartingTime);
		}
	}

	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}
	CountdownInt = SecondsLeft;
}

void AShooterPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (ShooterHUD && ShooterHUD->CharacterOverlay)
		{
			CharacterOverlay = ShooterHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				SetHUDHP(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDeaths(HUDDeaths);
			}
		}
	}
}

void AShooterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(InPawn);
	if (ShooterCharacter)
	{
		SetHUDHP(ShooterCharacter->GetHealth(), ShooterCharacter->GetMaxHealth());
	}
}

void AShooterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AShooterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float AShooterPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AShooterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void AShooterPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State; 

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AShooterPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AShooterPlayerController::HandleMatchHasStarted()
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	if (ShooterHUD)
	{
		if (ShooterHUD->CharacterOverlay == nullptr) ShooterHUD->AddCharacterOverlay();
		if (ShooterHUD->Announcement)
		{
			ShooterHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AShooterPlayerController::HandleCooldown()
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	if (ShooterHUD)
	{
		ShooterHUD->CharacterOverlay->RemoveFromParent();
		if (ShooterHUD->Announcement && ShooterHUD->Announcement->AnnouncementText && ShooterHUD->Announcement->InfoText)
		{
			ShooterHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("New Match Starts In:"); 
			ShooterHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));
			AShooterGameState* ShooterGameState = Cast<AShooterGameState>(UGameplayStatics::GetGameState(this));
			AShooterPlayerState* ShooterPlayerState = GetPlayerState<AShooterPlayerState>();
			if (ShooterGameState && ShooterPlayerState)
			{
				TArray<AShooterPlayerState*> TopPlayers = ShooterGameState->TopScoringPlayers;
				FString InfoTextString;
				if (TopPlayers.Num() == 0)
				{
					InfoTextString = FString("No Winner");
				}
				else if (TopPlayers.Num() == 1 && TopPlayers[0] == ShooterPlayerState)
				{
					InfoTextString = FString("You are the Winner");
				}
				else if (TopPlayers.Num() == 1)
				{
					InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *TopPlayers[0]->GetPlayerName());
				}
				else if (TopPlayers.Num() > 1)
				{
					InfoTextString = FString::Printf(TEXT("Winners: \n"));
					for (auto TiedPlayer : TopPlayers)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
				}
				ShooterHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));

			}
		}
	}
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetPawn());
	if (ShooterCharacter && ShooterCharacter->GetCombat())
	{
		ShooterCharacter->bDisableGameplay = true;
		ShooterCharacter->GetCombat()->FireButtonPressed(false);
	}
}

void AShooterPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime)
{
	WarmupTime = Warmup; 
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime; 
	MatchState = StateOfMatch; 
	OnMatchStateSet(MatchState);
	if (ShooterHUD && MatchState == MatchState::WaitingToStart)
	{
		ShooterHUD->AddAnnouncement();
	}
}

void AShooterPlayerController::ServerCheckMatchState_Implementation()
{
	AShooterGameMode* GameMode = Cast<AShooterGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);
	}
}
