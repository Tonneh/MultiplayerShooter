// Fill out your copyright notice in the Description page of Project Settings.

#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MultiplayerShooter/Character/ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "MultiplayerShooter/ShooterComponents/LagCompensationComponent.h"
#include "MultiplayerShooter/PlayerController/ShooterPlayerController.h"

void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)
		return;
	AController* InstigatorController = OwnerPawn->GetController();
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector Start = SocketTransform.GetLocation();

		// Maps character and times hit
		TMap<AShooterCharacter*, uint32> HitMap;
		TMap<AShooterCharacter*, uint32> HeadShotHitMap;
		for (auto HitTarget : HitTargets)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);
			AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(FireHit.GetActor());
			if (ShooterCharacter)
			{
				// Checks if characters hit are in map, if so add hit to map, if not in map, then add to map.
				// this is incase the shotgun hits multiple targets

				const bool bHeadShot = FireHit.BoneName.ToString() == FString("head");
				if (bHeadShot)
				{
					if (HeadShotHitMap.Contains(ShooterCharacter))
						HeadShotHitMap[ShooterCharacter]++;
					else
						HeadShotHitMap.Emplace(ShooterCharacter, 1);
				} 
				else
				{
					if (HitMap.Contains(ShooterCharacter))
						HitMap[ShooterCharacter]++;
					else
						HitMap.Emplace(ShooterCharacter, 1);
				}
				if (ImpactParticles)
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
				if (HitSound)
					UGameplayStatics::PlaySoundAtLocation(this, HitSound, FireHit.ImpactPoint, 0.5f, FMath::FRandRange(-0.5f, 0.5f));
			}
		}
		TArray<AShooterCharacter*> HitCharacters;
		TMap<AShooterCharacter*, float> DamageMap; // Maps Character hit to total damage
		for (auto HitPair : HitMap) // Calculates body shot damage by multiply times hit * damage
		{
			if (HitPair.Key)
			{
				DamageMap.Emplace(HitPair.Key, HitPair.Value * Damage);
				HitCharacters.AddUnique(HitPair.Key);
			}
		}
		for (auto HeadShotHitPair : HeadShotHitMap) // Calcuates headshot damage by muliplying times hit * daamge - stores in damagemap
		{
			if (HeadShotHitPair.Key)
			{
				if (DamageMap.Contains(HeadShotHitPair.Key))
					DamageMap[HeadShotHitPair.Key] += HeadShotHitPair.Value * HeadShotDamage;
				else
					DamageMap.Emplace(HeadShotHitPair.Key, HeadShotHitPair.Value * HeadShotDamage);
				HitCharacters.AddUnique(HeadShotHitPair.Key);
			}
		}
		for (auto DamagePair : DamageMap) // loop through damage map applying damage for each character hit
		{
			if (DamagePair.Key && InstigatorController)
			{
				bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
				if (HasAuthority() && bCauseAuthDamage)
				{
					UGameplayStatics::ApplyDamage(DamagePair.Key, DamagePair.Value, InstigatorController, this, UDamageType::StaticClass());
				}
			}
		}

		if (!HasAuthority() && bUseServerSideRewind)
		{
			ShooterOwnerCharacter = ShooterOwnerCharacter == nullptr ? Cast<AShooterCharacter>(OwnerPawn) : ShooterOwnerCharacter;
			ShooterOwnerController = ShooterOwnerController == nullptr ? Cast<AShooterPlayerController>(InstigatorController) : ShooterOwnerController;
			if (ShooterOwnerController && ShooterOwnerCharacter && ShooterOwnerCharacter->GetLagCompensation() && ShooterOwnerCharacter->IsLocallyControlled())
			{
				ShooterOwnerCharacter->GetLagCompensation()->ShotgunServerScoreRequest(HitCharacters, Start, HitTargets, ShooterOwnerController->GetServerTime() - ShooterOwnerController->SingleTripTime);
			}
		}
	}
}

void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket == nullptr)
		return;
	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector TraceStart = SocketTransform.GetLocation();
	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	for (uint32 i = 0; i < NumberOfPellets; i++)
	{
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		const FVector EndLoc = SphereCenter + RandVec;
		const FVector ToEndLoc = EndLoc - TraceStart;
		HitTargets.Add(FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size()));
	}
}