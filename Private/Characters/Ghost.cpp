// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Ghost.h"

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "Items/Item.h"
#include "Items/Weapon/Sword.h"
#include "Animation/AnimMontage.h"
#include "HUD/GhostHUD.h"
#include "HUD/GhostOverlay.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Items/Health.h"


AGhost::AGhost()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 380.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void AGhost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Attributes && GhostOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		GhostOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
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
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &AGhost::Dodge);
	}
}

void AGhost::Jump()
{
	if (ActionState != EActionState::Unoccupied) return;
	Super::Jump();
}

void AGhost::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::HitReaction;
	}
}

float AGhost::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();

	return DamageAmount;
}

void AGhost::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void AGhost::AddSouls(ASoul* Soul)
{
	if (Attributes && GhostOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		GhostOverlay->SetSouls(Attributes->GetSouls());
	}
}

void AGhost::AddGold(ATreasure* Treasure)
{
	if (Attributes && GhostOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		GhostOverlay->SetGold(Attributes->GetGold());
	}
}

void AGhost::AddHealth(AHealth* Health)
{
	if (Attributes && GhostOverlay && Attributes->GetHealth() <= Attributes->GetMaxHealth() - Attributes->GetHealthPoints())
	{
		Attributes->GainHealth(Attributes->GetHealthPoints());
		GhostOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
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

		InitializeGhostOverlay(PlayerController);
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

void AGhost::Die()
{
	Super::Die();
	ActionState = EActionState::Dead;
}

void AGhost::PlayDodgeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DodgeMontage)
	{
		AnimInstance->Montage_Play(DodgeMontage);
		FName SectionName = FName("Dodge");
		AnimInstance->Montage_JumpToSection(SectionName, DodgeMontage);
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

void AGhost::HitReactEnd()
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

void AGhost::Dodge(const FInputActionValue& Value)
{
	if (ActionState != EActionState::Unoccupied)  return;

	//const bool isLAltPressed = Value.Get<bool>();

	if (Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost() && GhostOverlay)
	{
		PlayDodgeMontage();
		ActionState = EActionState::Dodge;
		Attributes->UseStamina(Attributes->GetDodgeCost());
		GhostOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void AGhost::InitializeGhostOverlay(APlayerController* PlayerController)
{
	if (PlayerController)
	{
		AGhostHUD* GhostHUD = Cast<AGhostHUD>(PlayerController->GetHUD());
		if (GhostHUD)
		{
			GhostOverlay = GhostHUD->GetGhostOverlay();
			if (GhostOverlay && Attributes)
			{
				GhostOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				GhostOverlay->SetStaminaBarPercent(1.f);
				GhostOverlay->SetGold(0);
				GhostOverlay->SetSouls(0);
			}
		}
	}
}

void AGhost::SetHUDHealth()
{
	if (GhostOverlay && Attributes)
	{
		GhostOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void AGhost::DodgeEnd()
{
	ActionState = EActionState::Unoccupied;
}
