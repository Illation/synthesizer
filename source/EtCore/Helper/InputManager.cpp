#include "stdafx.h"
#include "InputManager.h"


//---------------------------------
// InputManager::GetKeyState
//
// Returns the state of a given key
//
E_KeyState InputManager::GetKeyState(uint32 const key)
{
	auto keyIt = m_KeyStates.find(key);
	if (keyIt != m_KeyStates.cend())
	{
		return keyIt->second;
	}
	return E_KeyState::Up;
}

//---------------------------------
// InputManager::OnKeyPressed
//
// Sets the key to pressed
//
void InputManager::OnKeyPressed(uint32 const key)
{
	m_KeyStates[key] = E_KeyState::Pressed;
}

//---------------------------------
// InputManager::OnKeyReleased
//
// sets the key to Released
//
void InputManager::OnKeyReleased(uint32 const key)
{
	m_KeyStates[key] = E_KeyState::Released;
}

//---------------------------------
// InputManager::Update
//
// Updates key states
//
void InputManager::Update()
{
	for (auto & key : m_KeyStates)
	{
		if (key.second == E_KeyState::Pressed)
		{
			key.second = E_KeyState::Down;
		}
		else if (key.second == E_KeyState::Released)
		{
			key.second = E_KeyState::Up;
		}
	}
}
