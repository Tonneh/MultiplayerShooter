// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MultiplayerShooter/ShooterTypes/TurningInPlace.h"
#include "MultiplayerSHooter/Interfaces/InteractWithCrosshairsInterface.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class MULTIPLAYERSHOOTER_API AShooterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	AShooterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; 
	virtual void PostInitializeComponents() override; 
	void PlayFireMontage(bool bAiming); 
	void PlayElimMontage();
	virtual void OnRep_ReplicatedMovement() override; 
	void Elim(); 

	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

protected:
	virtual void BeginPlay() override;

	void MoveForward(float Value); 
	void MoveRight(float Value); 
	void Turn(float Value); 
	void LookUp(float Value); 
	void EquipButtonPressed(); 
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased(); 
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn(); 
	virtual void Jump() override; 
	void FireButtonPressed(); 
	void FireButtonReleased(); 

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	void UpdateHUDHP();
private: 
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* Camera; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget; 

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon; 

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon); 

	UPROPERTY(VisibleAnywhere) 
	class UCombatComponent* Combat; 

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation; 

	ETurningInPlace TurningInPlace; 
	void TurnInPlace(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* ElimMontage;

	void PlayHitReactMontage();

	void HideCameraIfCharacterClose(); 

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	bool bRotateRootBone; 
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();
	
	// Player Stats

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f; 

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health; 

	class AShooterPlayerController* ShooterPlayerController; 

	UFUNCTION()
	void OnRep_Health();

	bool bElimmed = false; 

	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 5.f;

	void ElimTimerFinished();
public:	
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped(); 
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AWeapon* GetEquippedWeapon(); 
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const; 
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
};
