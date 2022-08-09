// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"
#include "MultiplayerShooter/Character/ShooterCharacter.h"
#include "MultiplayerShooter/ShooterComponents/BuffComponent.h"

 AHealthPickup::AHealthPickup()
{
	 bReplicates = true; 
 }

 void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
 {
	 Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	 AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	 if (ShooterCharacter)
	 {
		 UBuffComponent* Buff = ShooterCharacter->GetBuff();
		 if (Buff)
		 {
			 Buff->Heal(HealAmount, HealingTime);
		 }
	 }
	 Destroy();
 }
