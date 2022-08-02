// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "RocketProjectile.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API ARocketProjectile : public AProjectile
{
	GENERATED_BODY()
protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

};
