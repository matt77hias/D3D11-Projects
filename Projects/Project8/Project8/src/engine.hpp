#pragma once

#include "targetver.h"

//-----------------------------------------------------------------------------
// System Includes
//-----------------------------------------------------------------------------
#pragma region

#include <stdint.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
using std::string;
using std::wstring;
#include <iostream>
using std::cin;
using std::cout;

#include <windows.h>
#include <d3d11_2.h>

#pragma endregion

//-----------------------------------------------------------------------------
// Engine Includes
//-----------------------------------------------------------------------------
#pragma region

#include "version.hpp"
#include "rendering.hpp"

#pragma endregion

//-----------------------------------------------------------------------------
// Engine Declarations and Definitions
//-----------------------------------------------------------------------------
#pragma region

/**
 A class of engines.
 */
class Engine {

public:

	/**
	 Constructs an engine.

	 @param[in]		hinstance
					A handle to the current instance of the application.
	 */
	explicit Engine(HINSTANCE hinstance);

	/**
	 Destructs this engine.
	 */
	~Engine();

	/**
	 Runs the engine.

	 @param[in]		nCmdShow
					Controls how the engine window is to be shown.
	 @param[in]		start_fullscreen
					If @c true the renderer should start in fullscreen mode.
					Otherwise the renderer should start in windowed mode.
	 */
	void Run(int nCmdShow, bool start_fullscreen);

	/**
	 Returns a handle to the window of this engine.

	 @return		A handle to the window of this engine.
	 */
	HWND GetWindow() const {
		return m_hwindow;
	}

	/**
	 Checks whether this engine is loaded.

	 @return		@c true if this engine is loaded.
					@c false otherwise.
	 */
	bool IsLoaded() const {
		return m_loaded;
	}

	/**
	 Sets the mode switch flag of this engine to the given value.

	 @param[in]		mode_switch
					The new value for the mode switch flag.
	 */
	void SetModeSwitchFlag(bool mode_switch) {
		m_mode_switch = mode_switch;
	}

protected:

	/**
	 A pointer to the renderer of this engine.
	 */
	Renderer *m_renderer;

private:

	/**
	 Initializes the engine window of this engine.

	 @return		A success/error value.
	 */
	HRESULT InitializeWindow();

	/**
	 Allocates a console to this engine for basic io and
	 redirects stdin, stdout and stderr to the allocated console.
	
	 @return		A success/error value.
	 */
	HRESULT AttachConsole();

	/**
	 A handle to the current instance of the application.
	 */
	HINSTANCE m_hinstance;

	/**
	 Main window handle of this engine.
	 */
	HWND m_hwindow;

	/**
	 Flag indicating wether this engine is loaded.
	 */
	bool m_loaded;

	/**
	 Flag indicating whether the application should switch between
	 full screen and windowed mode.
	 */
	bool m_mode_switch;
};

/**
 The engine used by the user.
 */
extern Engine *g_engine;

#pragma endregion
