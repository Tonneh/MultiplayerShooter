// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MultiplayerShooter/Weapons/Weapon.h"
#include "MultiplayerShooter/ShooterTypes/CombatStates.h"

void UShooterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner()); 
}

void UShooterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime); 

    if (ShooterCharacter == nullptr) 
    {
        ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner()); 
    }
    if (ShooterCharacter == nullptr) return; 

    FVector Velocity = ShooterCharacter->GetVelocity(); 
    Velocity.Z = 0.f; 
    Speed = Velocity.Size(); 
	
    bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling(); 
    bIsAccelerating = ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false; 
    bWeaponEquipped = ShooterCharacter->IsWeaponEquipped();
    EquippedWeapon = ShooterCharacter->GetEquippedWeapon();
    bIsCrouched = ShooterCharacter->bIsCrouched;
    bAiming = ShooterCharacter->IsAiming();
    TurningInPlace = ShooterCharacter->GetTurningInPlace();
    bRotateRootBone = ShooterCharacter->ShouldRotateRootBone();
    bElimmed = ShooterCharacter->IsElimmed();
    bHoldingFlag = ShooterCharacter->IsHoldingTheFlag();

    // Offset Yaw for Strafing
    FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
    FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
    FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation); 
    DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
    YawOffset = DeltaRotation.Yaw;

    CharacterRotationLastFrame = CharacterRotation; 
    CharacterRotation = ShooterCharacter->GetActorRotation(); 
    const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame); 
    const float Target = Delta.Yaw / DeltaTime; 
    const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
    Lean = FMath::Clamp(Interp, -90.f, 90.f);

    AO_Yaw = ShooterCharacter->GetAO_Yaw();
    AO_Pitch = ShooterCharacter->GetAO_Pitch();

    if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && ShooterCharacter->GetMesh())
    {
        LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
        FVector OutPosition;
        FRotator OutRotation; 
        ShooterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
        LeftHandTransform.SetLocation(OutPosition);
        LeftHandTransform.SetRotation(FQuat(OutRotation));

        if (ShooterCharacter->IsLocallyControlled())
        {
            bLocallyControlled = true;
            FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("Hand_R"), ERelativeTransformSpace::RTS_World);
            FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - ShooterCharacter->GetHitTarget()));
            RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaTime, 30.f);
        }
    }

    bUseFabrik = ShooterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied;
	if (ShooterCharacter->IsLocallyControlled() && ShooterCharacter->GetCombatState() != ECombatState::ECS_ThrowingGrenade && ShooterCharacter->bFinishedSwapping)
	{
		bUseFabrik = !ShooterCharacter->isLocallyReloading();
    }
    bUseAimOffsets = ShooterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !ShooterCharacter->GetDisableGameplay();
    bTransformRightHand = ShooterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !ShooterCharacter->GetDisableGameplay();
}
