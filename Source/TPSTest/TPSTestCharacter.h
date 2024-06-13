// Copyright Maxim Dudin, 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TPSTestCharacter.generated.h"


UCLASS(config=Game)
class ATPSTestCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Switch1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Switch2Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Switch0Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

public:
	ATPSTestCharacter();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FRotator GetAimOffsets() const;
	virtual FRotator GetAimOffsets_Implementation() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Weapon)
	int Weapon = 0;

	UFUNCTION()
	void OnRep_Weapon();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ChangeWeapon(int32 Value) const;
	virtual void ChangeWeapon_Implementation(int32 Value) const {};


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HandleFire() const;
	virtual void HandleFire_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Health)
	int Health = 100;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnRep_Health() const;
	virtual void OnRep_Health_Implementation() const {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Health)
	int PistolAmmo = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	int RiffleAmmo = 100;

	UFUNCTION(BlueprintCallable)
	void TakeDamage(float DamageAmount) 
	{
		if (HasAuthority())
		{
			Health -= DamageAmount;

			if (Health <= 0)
			{
				//Destroy();
				this->bLockLocation = true;
			};
		};
	};
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			
	void SwitchWeapon1(const FInputActionValue& Value);
	void SwitchWeapon2(const FInputActionValue& Value);
	void SwitchWeapon0(const FInputActionValue& Value);

	void StartAim(const FInputActionValue& Value);
	void StopAim(const FInputActionValue& Value);

	void StartFire(const FInputActionValue& Value);
	void StopFire(const FInputActionValue& Value);

	void SwitchWeapon(int32 WeaponIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSwitchWeapon(int32 WeaponIndex);

	void HandleWeaponSwitch(int32 WeaponIndex);

	void Fire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

