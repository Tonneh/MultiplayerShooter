// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"


USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	FVector BoxExtent;
};

USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float Time;
	
	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfo; 

	UPROPERTY()
	AShooterCharacter* Character; 
};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool bHitConfirmed;

	UPROPERTY()
	bool bHeadShot;
};

class AShooterCharacter;

USTRUCT(BlueprintType)
struct FShotgunServerSideRewindResult
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<AShooterCharacter*, uint32> HeadShots;
	UPROPERTY()
	TMap<AShooterCharacter*, uint32> BodyShots;
};

class AShooterPlayerController; 
class AShooterCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MULTIPLAYERSHOOTER_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULagCompensationComponent();
	friend class AShooterCharacter; 
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ShowFramePackage(const FFramePackage& Package, FColor Color);
	FServerSideRewindResult ServerSideRewind(AShooterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime);
	FServerSideRewindResult ProjectileServerSideRewind(AShooterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime);
	FShotgunServerSideRewindResult ShotgunServerSideRewind(const TArray<AShooterCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations, float HitTime);
	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(AShooterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime);
	UFUNCTION(Server, Reliable)
	void ProjectileServerScoreRequest(AShooterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime);
	UFUNCTION(Server, Reliable)
	void ShotgunServerScoreRequest(const TArray<AShooterCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations, float HitTime);
protected:
	virtual void BeginPlay() override;
	void SaveFramePackage(FFramePackage& Package); 
	FFramePackage InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, float HitTime);
	void CacheBoxPositions(AShooterCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveBoxes(AShooterCharacter* HitCharacter, const FFramePackage& Package);
	void ResetHitBoxes(AShooterCharacter* HitCharacter, const FFramePackage& Package);
	void EnableCharacterMeshCollision(AShooterCharacter* HitCharacter, ECollisionEnabled::Type Collision);
	void SaveFramePackage();
	FFramePackage GetFrameToCheck(AShooterCharacter* HitCharacter, float HitTime);

	FServerSideRewindResult ConfirmHit(const FFramePackage& Package, AShooterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation);
	FServerSideRewindResult ProjectileConfirmHit(const FFramePackage& Package, AShooterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime);
	FShotgunServerSideRewindResult ShotgunConfirmHits(const TArray<FFramePackage>& FramePackages, const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations);
private:
	UPROPERTY() 
	AShooterCharacter* Character; 

	UPROPERTY()
	AShooterPlayerController* Controller; 

	TDoubleLinkedList<FFramePackage> FrameHistory;

	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 4.f; 
public:

};
