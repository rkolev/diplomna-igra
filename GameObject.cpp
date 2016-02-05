#include "GameObject.h"

int GameObject::ms_idCounter = 0;

GameObject::GameObject()
{
	m_objectId = ms_idCounter++; 
}

GameObject::~GameObject()
{

}

int GameObject::GetId() const
{
	return m_objectId;
}

bool GameObject::operator==(GameObject const& other) const
{
	return m_objectId == other.GetId();
}