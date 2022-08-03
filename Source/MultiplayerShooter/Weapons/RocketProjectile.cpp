// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "RocketMovementComponent.h"

ARocketProjectile::ARocketProjectile()
{
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>(TEXT("Rocket Movement Component"));
	RocketMovementComponent->bRotationFollowsVelocity = true; 
	RocketMovementComponent->SetIsReplicated(true);
}

void ARocketProjectile::Destroyed()
{
}

void ARocketProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &ARocketProjectile::OnHit);
	}
	if (TrailSystem)
	{
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailSystem,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false);
	}
	if (ProjectileLoop && LoopingSoundAttenuation)
	{
		ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(
			ProjectileLoop,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			EAttachLocation::KeepWorldPosition,
			false,
			1.f,
			1.f,
			0.f,
			LoopingSoundAttenuation,
			(USoundConcurrency*)nullptr,
			false);
	}
}

void ARocketProjectile::DestroyTimerFinish()
{
	Destroy();
}

void ARocketProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetOwner())
	{
		return;
	}
	APawn* FiringPawn = GetInstigator();
	if (FiringPawn && HasAuthority())
	{
		AController* FiringController = FiringPawn->GetController();
		if (FiringController)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this,						// World COntext object
				Damage,						// base damage
				Damage / 10,				// minimum damage
				GetActorLocation(),			// origin
				300.f,						// inner radius
				750.f,						// outter radius
				1.f,						// fall off
				UDamageType::StaticClass(), // damage type class
				TArray<AActor*>(),			// Ignoreactors
				this,						// damage causer
				FiringController);			// instigator controller
		}
	}

	GetWorldTimerManager().SetTimer(DestroyTimer, this, &ARocketProjectile::DestroyTimerFinish, DestroyTime);

	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (RocketMesh)
	{
		RocketMesh->SetVisibility(false);
	}
	if (CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (TrailSystemComponent && TrailSystemComponent->GetSystemInstance())
	{
		TrailSystemComponent->GetSystemInstance()->Deactivate();
	}
	if (ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
	{
		ProjectileLoopComponent->Stop();
	}
}
