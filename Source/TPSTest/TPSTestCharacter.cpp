// Copyright Maxim Dudin, 2024. All Rights Reserved.

#include "TPSTestCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"


//////////////////////////////////////////////////////////////////////////
// ATPSTestCharacter

ATPSTestCharacter::ATPSTestCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ATPSTestCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPSTestCharacter, Weapon);
	DOREPLIFETIME(ATPSTestCharacter, Health);
	DOREPLIFETIME(ATPSTestCharacter, PistolAmmo);
	DOREPLIFETIME(ATPSTestCharacter, RiffleAmmo);
}

void ATPSTestCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATPSTestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATPSTestCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATPSTestCharacter::Look);

		//Switch
		EnhancedInputComponent->BindAction(Switch1Action, ETriggerEvent::Triggered, this, &ATPSTestCharacter::SwitchWeapon1);
		EnhancedInputComponent->BindAction(Switch2Action, ETriggerEvent::Triggered, this, &ATPSTestCharacter::SwitchWeapon2);
		EnhancedInputComponent->BindAction(Switch0Action, ETriggerEvent::Triggered, this, &ATPSTestCharacter::SwitchWeapon0);

		//Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ATPSTestCharacter::StartAim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ATPSTestCharacter::StopAim);

		//Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ATPSTestCharacter::StartFire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ATPSTestCharacter::StopFire);
	}
}

void ATPSTestCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ATPSTestCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ATPSTestCharacter::SwitchWeapon1(const FInputActionValue& Value)
{
	SwitchWeapon(1);
}

void ATPSTestCharacter::SwitchWeapon2(const FInputActionValue& Value)
{
	SwitchWeapon(2);
}

void ATPSTestCharacter::SwitchWeapon0(const FInputActionValue& Value)
{
	SwitchWeapon(0);
}

void ATPSTestCharacter::StartAim(const FInputActionValue& Value)
{
	FollowCamera->SetFieldOfView(30.0F);
}

void ATPSTestCharacter::StopAim(const FInputActionValue& Value)
{
	FollowCamera->SetFieldOfView(90.0F);
}

void ATPSTestCharacter::StartFire(const FInputActionValue& Value)
{
	Fire();
}

void ATPSTestCharacter::StopFire(const FInputActionValue& Value)
{
	
}

void ATPSTestCharacter::SwitchWeapon(int32 WeaponIndex)
{
	if (HasAuthority())
	{
		HandleWeaponSwitch(WeaponIndex);
	}
	else
	{
		ServerSwitchWeapon(WeaponIndex);
	}
}

void ATPSTestCharacter::ServerSwitchWeapon_Implementation(int32 WeaponIndex)
{
	HandleWeaponSwitch(WeaponIndex);
}

bool ATPSTestCharacter::ServerSwitchWeapon_Validate(int32 WeaponIndex)
{
	return true;
}

void ATPSTestCharacter::HandleWeaponSwitch(int32 WeaponIndex)
{
	Weapon = WeaponIndex;
}

void ATPSTestCharacter::OnRep_Weapon()
{
	ChangeWeapon(Weapon);
}

void ATPSTestCharacter::Fire()
{
	if (HasAuthority())
	{
		HandleFire();
	}
	else
	{
		ServerFire();
	}
}

void ATPSTestCharacter::ServerFire_Implementation()
{
	HandleFire();
}

bool ATPSTestCharacter::ServerFire_Validate()
{
	return true;
}

void ATPSTestCharacter::HandleFire_Implementation()
{
	if (Weapon)
	{
		switch (Weapon)
		{
		case 0:
			break;
		case 1:
			if (PistolAmmo > 0)
			{
				PistolAmmo -= 1;
			};
			break;
		case 2:
			if (RiffleAmmo > 0)
			{
				RiffleAmmo -= 1;
			};
			break;
		default:
			break;
		}
	}
}

FRotator ATPSTestCharacter::GetAimOffsets_Implementation() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}


