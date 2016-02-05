#pragma once

enum ConnectionMode
{
	None,
	eClient,
	eServer
};

enum ConnectionState
{
	eDisconnected,
	eListening,
	eConnecting,
	eConnected,
	eFull
};

enum GameMode
{
	eSinglePlayer,
	eHotseat,
	eMultiplayerServer,
	eMultiplayerClient
};

enum Speed
{
	eCompositeSpeed,
	eXSpeed,
	eYSpeed,
	eXCurrentSpeed,
	eYCurrentSpeed,
	eSpeedElementCount = 5
};

enum Stat
{
	eCurrentLife,
	eMaxLife,
	eContactDamage,
	eSightRadius,
	eAttackPower,
	eAttackRange,
	eAttackCooldown,
	eAttackFrame,
	eTimeSinceLastAttack,
	eJumpTime,
	eMaxJumpTime,
	eStatElementCount = 11
};

enum Flag
{
	eAttacking,
	eMoving,
	eJumping,
	eFalling,
	eDead,
	eDestructible,
	eCollidable,
	eSpectral,
	eAffectedByGravity,
	eCanAttack,
	eCanShoot,
	eOmnidirectional,
	eFlagElementCount = 12
};

enum Direction
{
	eLeft,
	eRight
};

enum NpcBehavior
{
	eAggression, 
	eDesire,
	eFear,
	eMisc,
	eWander,	
	eNpcBehaviorElementCount = 5
};

enum BasicAction
{
	eAttack,
	eMove,
	eJump,
	eShoot,
	eFlee,
	eStop
};

enum AttackType
{
	eMelee,
	eFireball,
	eBlueFlame
};

enum PlayerType
{
	eKnight,
	eVampire,
	ePlayerTypeCount = 2
};