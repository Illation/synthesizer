#pragma once
#include <map>

//---------------------------------
// E_KeyState
//
// Reflects the state of a keyboard key in a simplistic way - pressed and released are only valid for one frame
//
enum E_KeyState
{
	Released,
	Up,			// Check <= up for up or released
	Down,		// Check >= down for down or pressed
	Pressed
};

//---------------------------------
// InputManager
//
// Input Manager class
//
class InputManager : public Singleton<InputManager>
{
public:
	E_KeyState GetKeyState(uint32 const key);

	bool IsRunning() const { return m_IsRunning; }

private:
	friend class Singleton<InputManager>;
	friend class Framework;
	friend class FrameworkWindow;

	// Defualt constructor and destructor
	InputManager() : m_KeyStates() {}
	virtual ~InputManager() = default;

	void OnKeyPressed(uint32 const key);
	void OnKeyReleased(uint32 const key);

	void Update();

	void Quit() { m_IsRunning = false; }

	// DATA
	///////
	bool m_IsRunning = true;

	// populated dynamically right now because I'm lazy
	std::map<uint32, E_KeyState> m_KeyStates;
};
