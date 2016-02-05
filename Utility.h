#pragma once
#include "GameObject.h"

namespace Utility
{
	struct Line
	{
		CL_Point p1;
		CL_Point p2;
	};

	//TODO: REFERENCES!

	//Checks if bounds of @rect1 and @rect2 intersect
	bool BoundsIntersect(CL_Rect rect1, CL_Rect bounds2);

	//Returns intersection of @rect1 and @rect2
	//TODO: will it be too much of a performance hit if we combine this and the above function
	CL_Rect GetIntersection(CL_Rect rect1, CL_Rect rect2);

	//Checks if point described by @x and @y exists in the bounds of @rect
	bool ContainsPoint(CL_Rect rect, s32 x, s32 y);

	//Checks if @line intersects with @obstacle
	//TODO: rename params to more generic names
	bool LineIntesectsRectangle(Line sight, CL_Rect obstacle);

	//Checks if @line and @obstacle intersect
	//TODO: rename params to more generic names
	bool LineIntersectsLine(Line sight, Line obstacle);

	//Get all objects from @gameObjects that collide with the @movementBounds
	//TODO: very bad idea to copy the whole list, use reference or pointer
	GameObjects GetCollidingObjects(CL_Rect &movementBounds, GameObjects *gameObjects);

	//Checks for speculative contacts
	//TODO: document
	void SpecContacts(CL_Rect const &collidingObjectBounds, CL_Rect &movementBounds, CL_Rect &spriteBounds, s32 &nextMoveX, s32 &nextMoveY, CL_Point *collisionPoints);

	//Resolves penetrations caused by collision resolution
	//TODO: document
	void PenetrationResolution(CL_Rect const &collidingObjectBounds, CL_Rect &spriteBounds, s32 &nextMoveX, s32 &nextMoveY, CL_Point *collisionPoints);

	//Detects vertical or horizontal contacts based on originally planned movement vector and
	//calculated movement vector after collision resolution
	//TODO: document
	void DetectContact(bool *contacts, s32 origMoveX, s32 origMoveY, s32 nextMoveX, s32 nextMoveY);

	//Calculates arc trajectory from @start to @end
	//TODO: document
	double CalculateTrajectory(const CL_Point& start, const CL_Point& end, const float speed, const bool useHighArc);

	//Get the distance from @pointA to @pointB
	s32 GetDistance(CL_Point const& pointA, CL_Point const& pointB);
}; //namespace Utility