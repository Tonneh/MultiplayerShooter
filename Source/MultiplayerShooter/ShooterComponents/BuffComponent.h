// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"

class AShooterCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuffComponent();
	friend class AShooterCharacter;
	void Heal(float HealAmount, float HealTime); 
	void Shield(float ShieldAmount, float ShieldTime);
	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);
	void BuffJump(float BuffJumpVelocity, float BuffTime);
	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);
	void SetInitialJumpVelocity(float JumpVelocity);

protected:
	virtual void BeginPlay() override;
	void HealRampUp(float DeltaTime);
	void ShieldRampUp(float DeltaTime);

private:
	UPROPERTY()
	AShooterCharacter* Character;

	// Healing 
	bool bHealing = false; 
	float HealingRate = 0.f; 
	float AmountToHeal = 0.f;

	// Shielding 
	bool bShielding = false;
	float ShieldingRate = 0.f; 
	float AmountToShield = 0.f; 

	// Speed Buff 
	FTimerHandle SpeedBuffTimer; 
	void ResetSpeed();
	float InitialBaseSpeed;
	float InitialCrouchSpeed; 

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed);
	
	// Jump Buff
	FTimerHandle JumpBuffTimer; 
	void ResetJump();
	float InitialJumpVelocity; 

	UFUNCTION(NetMulticast, Reliable)
	void MulticastJumpBuff(float JumpVelocity);

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
};
