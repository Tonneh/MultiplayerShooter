// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketProjectile.h"
#include "Kismet/GameplayStatics.h"
void ARocketProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	APawn* FiringPawn = GetInstigator();
	if (FiringPawn)
	{
		AController* FiringController = FiringPawn->GetController();
		if (FiringController)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this,						// World COntext object
				Damage,						// base damage
				Damage / 10,				// minimum damage
				GetActorLocation(),			// origin
				200.f,						// inner radius
				500.f,						// outter radius
				1.f,						// fall off
				UDamageType::StaticClass(), // damage type class
				TArray<AActor*>(),			// Ignoreactors
				this,						// damage causer
				FiringController);			// instigator controller
		}
	}
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
