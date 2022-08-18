// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MultiplayerShooter/ShooterTypes/Team.h"
#include "ShooterPlayerState.generated.h"

class AShooterCharacter;
class AShooterPlayerController;

UCLASS()
class MULTIPLAYERSHOOTER_API AShooterPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Score() override; 
	UFUNCTION()
	virtual void OnRep_Deaths();
	void AddToScore(float ScoreAmount); 
	void AddToDeaths(int32 DeathsAmount);
private:
	UPROPERTY()
	AShooterCharacter* Character; 
	UPROPERTY()
	AShooterPlayerController* Controller; 

	UPROPERTY(ReplicatedUsing = OnRep_Deaths)
	int32 Deaths; 

	UPROPERTY(ReplicatedUsing = OnRep_Team)
	ETeam Team = ETeam::ET_NoTeam;

	UFUNCTION() 
	void OnRep_Team();
public:
	FORCEINLINE ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam TeamToSet);
};
