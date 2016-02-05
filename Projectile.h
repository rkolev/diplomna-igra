#pragma once
#include "MovingObject.h"

class Projectile : public MovingObject
{
public:
	Projectile(CL_GraphicContext* gc, CL_ResourceManager& resources, s32 posX, s32 posY, s8* objectType);
	virtual ~Projectile();
};

