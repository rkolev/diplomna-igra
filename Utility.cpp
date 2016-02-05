#include "Utility.h"
#include <vector>

namespace Utility
{
	bool BoundsIntersect(CL_Rect rect1, CL_Rect rect2) 
	{
		return (rect1.left < rect2.right) && (rect1.top < rect2.bottom) && (rect2.left < rect1.right) && (rect2.top < rect1.bottom);
	}

	CL_Rect GetIntersection(CL_Rect rect1, CL_Rect rect2)
	{
		CL_Rect intersection;
		if (BoundsIntersect(rect1, rect2) == false) {
			//there is no intersection, return a rectangle with size 0
			intersection.left = 0;
			intersection.top = 0;
			intersection.right = 0;
			intersection.bottom = 0;
			return intersection;
		}

		//calculate the intersection
		intersection.left = max(rect1.left, rect2.left);
		intersection.top = max(rect1.top, rect2.top);
		intersection.right = min(rect1.right, rect2.right);
		intersection.bottom = min(rect1.bottom, rect2.bottom);
		return intersection;
	}	

	bool ContainsPoint(CL_Rect rect, int x, int y)
	{		
		return (x >= rect.left && x <= rect.right && y >= rect.top && y <= rect.bottom);
	}

	bool LineIntesectsRectangle(Line sight, CL_Rect obstacle)
	{
		//TODO: is this the most optimal way to check line and rectangle intersection?
		Line topLine;
		topLine.p1 = CL_Point(obstacle.left, obstacle.top);
		topLine.p2 = CL_Point(obstacle.right, obstacle.top);

		Line bottomLine;
		bottomLine.p1 = CL_Point(obstacle.left, obstacle.bottom);
		bottomLine.p2 = CL_Point(obstacle.right, obstacle.bottom);

		Line leftLine;
		leftLine.p1 = CL_Point(obstacle.left, obstacle.top);
		leftLine.p2 = CL_Point(obstacle.left, obstacle.bottom);

		Line rightLine;
		rightLine.p1 = CL_Point(obstacle.right, obstacle.top);
		rightLine.p2 = CL_Point(obstacle.right, obstacle.bottom);

		return LineIntersectsLine(sight, topLine) ||
			LineIntersectsLine(sight, bottomLine) ||
			LineIntersectsLine(sight, leftLine) ||
			LineIntersectsLine(sight, rightLine) ||
			(obstacle.contains(sight.p1) && obstacle.contains(sight.p2));
	}

	bool LineIntersectsLine(Line sight, Line obstacle)
	{
		//TODO: refactor
		float q = (sight.p1.y - obstacle.p1.y) * (obstacle.p2.x - obstacle.p1.x) - (sight.p1.x - obstacle.p1.x) * (obstacle.p2.y - obstacle.p1.y);
		float d = (sight.p2.x - sight.p1.x) * (obstacle.p2.y - obstacle.p1.y) - (sight.p2.y - sight.p1.y) * (obstacle.p2.x - obstacle.p1.x);

		if (d == 0)	{
			return false;
		}

		float r = q / d;

		q = (sight.p1.y - obstacle.p1.y) * (sight.p2.x - sight.p1.x) - (sight.p1.x - obstacle.p1.x) * (sight.p2.y - sight.p1.y);
		float s = q / d;

		if (r < 0 || r > 1 || s < 0 || s > 1) {
			return false;
		}

		return true;
	}

	GameObjects GetCollidingObjects(CL_Rect &movementBounds, GameObjects *gameObjects)
	{
		GameObjects collidingObjects;

		GameObjects::iterator it = gameObjects->begin();
		for ( ; it != gameObjects->end(); ++it)
		{
			//create a rectangle formed containing the player bounds and the current enviroment object bounds			
			CL_Rect totalBounds;
			totalBounds.left = min(movementBounds.left, (*it)->GetBounds().left);
			totalBounds.top = min(movementBounds.top, (*it)->GetBounds().top);
			totalBounds.right = max(movementBounds.right, (*it)->GetBounds().right);
			totalBounds.bottom = max(movementBounds.bottom, (*it)->GetBounds().bottom);

			//check if the player bounds and the current object bounds intersect
			if (totalBounds.right - totalBounds.left < (*it)->GetBounds().right - (*it)->GetBounds().left + movementBounds.right - movementBounds.left
				&& totalBounds.bottom - totalBounds.top < (*it)->GetBounds().bottom - (*it)->GetBounds().top + movementBounds.bottom - movementBounds.top) {
				//if the player bounds and the current object bounds intersect, add the object to the list of objects to check for collision
				collidingObjects.push_back((*it));
			}
		}

		return collidingObjects;
	}

	void SpecContacts(CL_Rect const &collidingObjectBounds, CL_Rect &movementBounds, CL_Rect &spriteBounds, s32 &nextMoveX, s32 &nextMoveY, CL_Point *collisionPoints)
	{
		CL_Rect specIntersection = GetIntersection(movementBounds, collidingObjectBounds);

		if (specIntersection.bottom - specIntersection.top >= 1 && specIntersection.right - specIntersection.left >= 1)	{
			for (s32 dir = 0; dir < 4; dir++) {
				if (dir == 0 && nextMoveY > 0) continue; //direction: up
				if (dir == 1 && nextMoveY < 0) continue; //direction: down
				if (dir == 2 && nextMoveX > 0) continue; //direction: left
				if (dir == 3 && nextMoveX < 0) continue; //direction: right

				//calculate the safe movement by x and y where no collision should happen
				//and the safe movement vector
				s32 safeMoveX = max(max(specIntersection.left - spriteBounds.right, spriteBounds.left - specIntersection.right), 0);
				s32 safeMoveY = max(max(specIntersection.top - spriteBounds.bottom, spriteBounds.top - specIntersection.bottom), 0);
				float safeVecLen = sqrt(static_cast<float>(safeMoveX * safeMoveX + safeMoveY * safeMoveY));

				//calculate the next move movement vector
				float vectorLength = sqrt(static_cast<float>(nextMoveX * nextMoveX + nextMoveY * nextMoveY));

				//calculate the projected movement
				s32 projectedMoveX = static_cast<s32>(nextMoveX * (safeVecLen / vectorLength));
				s32 projectedMoveY = static_cast<s32>(nextMoveY * (safeVecLen / vectorLength));

				vectorLength -= safeVecLen;
				int segments = 0;

				//traverse the next move vector until a collision is found or we reach the end of the vector
				while (ContainsPoint(collidingObjectBounds, collisionPoints[dir * 2].x + spriteBounds.left + projectedMoveX, collisionPoints[dir * 2].y + spriteBounds.top + projectedMoveY) == false
					&& ContainsPoint(collidingObjectBounds, collisionPoints[dir * 2 + 1].x + spriteBounds.left + projectedMoveX, collisionPoints[dir * 2 + 1].y + spriteBounds.top + projectedMoveY) == false
					&& segments < static_cast<s32>(vectorLength)) {
					//if no collision is detected, move the player further along the movement vector vector
					projectedMoveX += static_cast<s32>(nextMoveX / vectorLength);
					projectedMoveY += static_cast<s32>(nextMoveY / vectorLength);
					segments++;
				}

				if (segments < static_cast<s32>(vectorLength)) {
					//executed only if a collision has occurred
					if (segments > 0) {
						//revert the player position to the last point where no collision occurred
						projectedMoveX -= static_cast<s32>(nextMoveX / vectorLength);
						projectedMoveY -= static_cast<s32>(nextMoveY / vectorLength);
					}

					//if the movement we're exploring is horizontal, adjust the x coordinate
					if (dir >= 2 && dir <= 3) nextMoveX = projectedMoveX;
					//if the movement we're exploring is vertical, adjust the y coordinate
					if (dir >= 0 && dir <= 1) nextMoveY = projectedMoveY;
				}
			}
		}
	}

	void PenetrationResolution(CL_Rect const &collidingObjectBounds, CL_Rect &spriteBounds, s32 &nextMoveX, s32 &nextMoveY, CL_Point *collisionPoints)
	{
		//calculate the bounds of the expected player position
		CL_Rect nextBounds;
		nextBounds.left = spriteBounds.left + nextMoveX;
		nextBounds.top = spriteBounds.top + nextMoveY;
		nextBounds.right = spriteBounds.right + nextMoveX;
		nextBounds.bottom = spriteBounds.bottom + nextMoveY;

		if (BoundsIntersect(nextBounds, collidingObjectBounds) == false) {
			//No point in executing the rest of the code if the bounds don't intersect
			return;
		}

		CL_Rect intersection = GetIntersection(nextBounds, collidingObjectBounds);		

		//calculate the penetration between the expected player position and the collidint object
		s32 intX = intersection.right - intersection.left;
		s32 intY = intersection.bottom - intersection.top;

		for (int dir = 0; dir < 4; dir++) {
			//if a collision has occurred, nudge the player to resolve the penetration
			if (ContainsPoint(intersection, collisionPoints[dir * 2].x + spriteBounds.left + nextMoveX, collisionPoints[dir * 2].y + spriteBounds.top + nextMoveY)
				|| ContainsPoint(intersection, collisionPoints[dir * 2 + 1].x + spriteBounds.left + nextMoveX, collisionPoints[dir * 2 + 1].y + spriteBounds.top + nextMoveY)) {
				switch (dir) {
					case 0: nextMoveY += intY; break;
					case 1: nextMoveY -= intY; break;
					case 2: nextMoveX += intX; break;
					case 3: nextMoveX -= intX; break;
				}
			}
		}
	}

	void DetectContact(bool *contacts, s32 origMoveX, s32 origMoveY, s32 nextMoveX, s32 nextMoveY)
	{
		if (nextMoveY > origMoveY && origMoveY < 0)	{
			contacts[0] = true;
		}

		if (origMoveY > nextMoveY && origMoveY >= 0) {
			contacts[1] = true;
		}

		if (abs(nextMoveX - origMoveX) > 0) {
			contacts[2] = true;
		}
	}

	// Implementation of the formula for calculating the angle of launch
	// needed by a projectile so it can reach a given destination
	double CalculateTrajectory(const CL_Point& start, const CL_Point& end, float speed, bool useHighArc)
	{
		// We use doubles instead of floats because we need a lot of
		// precision for some uses of the pow() function coming up.
		double term1 = 0.0f;
		double term2 = 0.0f;
		double root = 0.0f;

		CL_Point diffVector = end - start;

		// We shrink our values by this factor to prevent too much
		// precision loss.
		const float factor = 100.0;

		//TODO: refactor
		float x = diffVector.x / factor;
		float y = diffVector.y / factor;
		float v = speed / factor;
		float g = GRAVITY / factor;

		term1 = pow(v, 4) - (g * ((g * pow(x, 2)) + (2 * y * pow(v, 2))));

		// If term1 is negative, then the 'end' point can be reached
		// at the given 'speed'.
		term1 = -term1; // we switch the sign, because we're working in the IV quadrant
		if (term1 >= 0)	{
			term2 = sqrt(term1);
			double divisor = (g * x);

			if (divisor != 0.0f) {
				if (useHighArc)	{
					root = (pow(v, 2) + term2) / divisor;
				} else {
					root = (pow(v, 2) - term2) / divisor;
				}
				root = atan(root);
			}
		}
		return root;
	}

	s32 GetDistance(CL_Point const& pointA, CL_Point const& pointB)
	{
		float xDistance, yDistance;
		xDistance = static_cast<float>(pointA.x - pointB.x);
		yDistance = static_cast<float>(pointA.y - pointB.y);
		return static_cast<s32>(sqrt(pow(xDistance, 2) + pow(yDistance, 2)));
	}
}