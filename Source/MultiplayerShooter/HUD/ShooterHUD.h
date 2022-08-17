// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

class UTexture2D;
class UElimAnnouncement;
class APlayerController;

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsLeft;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsBottom;
	float CrosshairSpread; 
	FLinearColor CrosshairsColor; 
};

class UUserWidget;
class UCharacterOverlay;
class UAnnouncement;

UCLASS()
class MULTIPLAYERSHOOTER_API AShooterHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override; 

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<UUserWidget> CharacterOverlayClass;
	
	UPROPERTY()
	UCharacterOverlay* CharacterOverlay; 

	void AddCharacterOverlay();

	UPROPERTY(EditAnywhere, Category = "Announcements")
	TSubclassOf<UUserWidget> AnnouncementClass; 

	UPROPERTY()
	UAnnouncement* Announcement;

	void AddAnnouncement();
	void AddElimAnnouncement(FString Attacker, FString Victim);
protected:
	virtual void BeginPlay() override; 
private:
	UPROPERTY()
	APlayerController* OwningPlayer;

	FHUDPackage HUDPackage; 

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UElimAnnouncement> ElimAnnouncementClass;

	UPROPERTY(EditAnywhere)
	float ElimAnnouncementTime = 3.f;

	UFUNCTION()
	void ElimAnnouncementTimerFinished(UElimAnnouncement* MsgToRemove); 

	UPROPERTY()
	TArray<UElimAnnouncement*> ElimMessages; 
public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
};
