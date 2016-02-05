#pragma once
#include "Utility.h"
#include "XmlParser.h"

//This class encompasses game objects that are not meant to move around
//and have no animations, e.g. they just stand there, e.g. tiles or spikes
class StaticObject : public GameObject
{
protected: //typedefs TODO: more appropriate containers?
	typedef std::unordered_map <Speed, float> Speeds_t;
	typedef std::unordered_map <Stat, s32> Stats_t;
	typedef std::unordered_map <Flag, bool> Flags_t;

public: //methods
	//Default constructor TODO: proper initialization or make it private
	//do we want to be able to create gameobjects this way?
	StaticObject() { }

	//Constructor with values
	StaticObject(CL_GraphicContext* gc, CL_ResourceManager& resources, s32 posX, s32 posY, s8* objectType);

	//Destructor
	virtual ~StaticObject() { }

	//Connects @drawSignal to draw callback slot
	void ConnectSlots(CL_Signal_v0& drawSignal);

	//Returns object's current position
	CL_Point const& GetPosition() const override { return m_position; }

	//Returns object's collision bounds
	CL_Rect const& GetBounds() const override { return m_bounds; }

	//Returns the direction the object is facing /left or right/
	Direction GetDirection() const override { return m_direction; }

	//Returns the value of the flag specified by @flagName
	bool GetFlag(Flag flagName) const override { return m_flags.find(flagName)->second; }

	//Returns the value of the stat specified by @statName
	s32 GetStat(Stat statName) const override { return m_stats.find(statName)->second; }

	//Returns the value of the speed specified by @speedName
	float GetSpeed(Speed speedName) const override { return m_speeds.find(speedName)->second; }

	//TODO: what is this
	void GetType(s8* type, s32 size) const;	

	//Sets the position of the object
	void SetPosition(CL_Point const& value) override { m_position = value; }

	//Sets the direction of the object
	void SetDirection(Direction dir) { m_direction = dir; }

	//Sets the value of the flag specified by @flagName
	void SetFlag(Flag flagName, bool value) override { m_flags.find(flagName)->second = value; }

	//Sets the value of the stat specified by @statName
	void SetStat(Stat statName, s32 value) override { m_stats.find(statName)->second = value; }
	
	//Sets the value of the speed specified by @speedName
	void SetSpeed(Speed speedName, float value) override { m_speeds.find(speedName)->second = value; }
	
	//Lowers the life points of the object (if destructible) by the amount specified
	void Hurt(s32 damage) override;	

protected: //methods

	//Draws the sprite on the screen
	void Draw();

	//Loads object data from settings xmls
	virtual void Initialize(XmlParser& parser, CL_ResourceManager& resources);

	//Initialzes maps for flags, stats and speeds with default values
	//TODO: how bad would it be if we don't initialize the maps with default values?
	virtual void InitializeMaps();

	//Calculates the object's collision bounds at the current location using its collision offsets
	virtual void CalculateBounds();

	//Sets the Dead flag to true so that it can be cleaned up by the level to free up resources
	virtual void Destroy();

protected: //members
	s8 m_objectType[52];
	s32 m_boundOffsets[4];
	CL_Sprite m_activeSprite;
	CL_Sprite m_idleSprite;
	CL_GraphicContext* m_graphicContext; //TODO: why is this a pointer?
	CL_ResourceManager*	m_resourceManager; //TODO: see above
	CL_Slot	m_drawSlot;
	CL_Point m_position;
	CL_Rect	m_bounds;
	Direction m_direction;
	Speeds_t m_speeds;
	Stats_t	m_stats;
	Flags_t	m_flags;
};
