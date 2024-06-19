// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Ghost.h"

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Item.h"
#include "Items/Weapon/Sword.h"
#include "Animation/AnimMontage.h"



AGhost::AGhost()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 380.f, 0.f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

}

void AGhost::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AGhost::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGhost::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AGhost::Jump);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AGhost::Run);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &AGhost::EquipKeyPressed);
		EnhancedInputComponent->BindAction(LMBAction, ETriggerEvent::Triggered, this, &AGhost::Attack);
	}
}

void AGhost::Jump()
{
	Super::Jump();
}

void AGhost::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(GhostContext, 0);
		}
	}

	Tags.Add(FName("Ghost"));
}

void AGhost::PlayLH_AttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && LH_AttackMontage)
	{
		AnimInstance->Montage_Play(LH_AttackMontage);
		FName SectionName = FName("LH_Attack1");
		AnimInstance->Montage_JumpToSection(SectionName, LH_AttackMontage);
	}
}

void AGhost::AttackEnd()
{
	ActionState = EActionState::Unoccupied;
}

bool AGhost::CanDisarm()
{
	return ActionState == EActionState::Unoccupied &&
		CharacterState == ECharacterState::ECS_EquippedTwoHandedWeapon;
}

bool AGhost::CanArm()
{
	return ActionState == EActionState::Unoccupied &&
		CharacterState == ECharacterState::ECS_EquippedOneHandedWeapon &&
		EquippedDagger;
}

void AGhost::PlayEquipMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}

}

void AGhost::AttachDagger()
{
	if (EquippedDagger)
	{
		EquippedDagger->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void AGhost::DettachDagger()
{
	if (EquippedDagger)
	{
		EquippedDagger->AttachMeshToSocket(GetMesh(), FName("LH_Dagger"));
	}
}

void AGhost::FinishEquipping()
{
	ActionState = EActionState::Unoccupied;
}

void AGhost::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::Unoccupied) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator ControlRotation = GetControlRotation();
	const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
	const FVector YawDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(YawDirection, MovementVector.Y);

	const FVector PitchDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(PitchDirection, MovementVector.X);
}

void AGhost::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxis = Value.Get<FVector2D>();

	if (GetController())
	{
		AddControllerPitchInput(LookAxis.Y);
		AddControllerYawInput(LookAxis.X);
	}
}

void AGhost::Run(const FInputActionValue& Value)
{
	const bool IsRunning = Value.Get<bool>();

	if (IsRunning)
	{
		GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
}

void AGhost::EquipKeyPressed(const FInputActionValue& Value)
{
	const bool Pick = Value.Get<bool>();

	if (Pick)
	{
		ASword* OverlappingWeapon = Cast<ASword>(OverlappingItem);
		if (OverlappingWeapon)
		{
			if (CharacterState == ECharacterState::ECS_Unequipped)
			{
				OverlappingWeapon->Equip(GetMesh(), FName("RH_Sword"), this, this);
				CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon; 
				EquippedSword = OverlappingWeapon;
			}
			else if (CharacterState == ECharacterState::ECS_EquippedOneHandedWeapon)
			{
				OverlappingWeapon->Equip(GetMesh(), FName("LH_Dagger"), this, this);
				CharacterState = ECharacterState::ECS_EquippedTwoHandedWeapon;
				EquippedDagger = OverlappingWeapon;
			}
		}
		else
		{
			if (CanDisarm())
			{
				PlayEquipMontage(FName("Unequip"));
				CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
				ActionState = EActionState::EquippingWeapon;
			}
			else if (CanArm())
			{
				PlayEquipMontage(FName("Equip"));
				CharacterState = ECharacterState::ECS_EquippedTwoHandedWeapon;
				ActionState = EActionState::EquippingWeapon;
			}
		}
		OverlappingItem = nullptr;
	}
}

void AGhost::Attack(const FInputActionValue& Value)
{
	const bool isLMB_Pressed = Value.Get<bool>();
	if (isLMB_Pressed)
	{
		if (ActionState == EActionState::Unoccupied && CharacterState == ECharacterState::ECS_EquippedOneHandedWeapon)
		{
			PlayAttackMontage();
			ActionState = EActionState::Attacking;
		}
		else if (ActionState == EActionState::Unoccupied && CharacterState == ECharacterState::ECS_EquippedTwoHandedWeapon)
		{
			PlayLH_AttackMontage();
			ActionState = EActionState::Attacking;
		}
	}
}