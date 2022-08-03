// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MultiplayerShooter/HUD/ShooterHUD.h"
#include "MultiplayerShooter/Weapons/WeaponTypes.h"
#include "MultiplayerShooter/ShooterTypes/CombatStates.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000

class AWeapon; 

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	UCombatComponent();
	friend class AShooterCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(AWeapon* WeaponToEquip);
	void Reload();
	
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	void FireButtonPressed(bool bPressed);
protected:
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming); 

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void Fire();

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);
	
	void TraceUnderCrosshairs(FHitResult& TraceHitResult); 

	void SetHUDCrosshairs(float DeltaTime); 

	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandleReload();

	int32 AmountToReload();
private: 
	UPROPERTY()
	class AShooterCharacter* Character; 
	UPROPERTY()
	class AShooterPlayerController* Controller; 
	UPROPERTY()
	class AShooterHUD* HUD;
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon; 

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere) 
	float BaseWalkSpeed; 

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed; 

	bool bFireButtonPressed;

	// HUD and Crosshairs

	float CrosshairVelocityFactor; 
	float CrosshairInAirFactor;
	float CrosshairAimFactor; 
	float CrosshairShootingFactor; 

	FVector HitTarget;

	FHUDPackage HUDPackage;

	// Aiming and fov, when not aiming, set to cameras base pov
	float DefaultFOV; 
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomedFOV = 30.f; 

	float CurrentFOV; 

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);

	//auto fire

	FTimerHandle FireTimer; 

	bool bCanFire = true;
	void StartFireTimer();
	void FireTimerFinished(); 

	bool CanFire(); 

	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30; 


	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 0;

	void InitializedCarriedAmmo(); 

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	void UpdateAmmoValues();
public:	
		
};
