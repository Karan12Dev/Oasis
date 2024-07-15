#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequiped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	Unoccupied UMETA(DisplayName = "Unoccupied"),
	HitReaction UMETA(DisplayName = "HitReaction"),
	Attacking UMETA(DisplayName = "Attacking"),
	Dodge UMETA(DisplayName = "Dodge"),
	EquippingWeapon UMETA(DisplayName = "Equipping Weapon"),
	Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum EDeadPose
{
	Dead1 UMETA(DisplayName = "Dead 1"),
	Dead2 UMETA(DisplayName = "Dead 2"),
	Dead3 UMETA(DisplayName = "Dead 3"),
	Dead4 UMETA(DisplayName = "Dead 4"),
	Dead5 UMETA(DisplayName = "Dead 5"),

	MAX UMETA(DisplayName = "DefalutMAX")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	NoState UMETA(DisplayName = "NoState"),

	Dead UMETA(DisplayName = "Dead"),
	Patrolling UMETA(DisplayName = "Patolling"),
	Chasing UMETA(DisplayName = "Chasing"),
	Attacking UMETA(DisplayName = "Attacking"),
	Engaged UMETA(DisplayName = "Engaged")
};