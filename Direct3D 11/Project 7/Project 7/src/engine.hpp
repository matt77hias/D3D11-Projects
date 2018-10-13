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
#include <directxmath.h>
using namespace DirectX;

#pragma endregion

//-----------------------------------------------------------------------------
// Engine Includes
//-----------------------------------------------------------------------------
#pragma region

#include "dds_loader.hpp"

#include "version.hpp"
#include "timer.hpp"
#include "geometry.hpp"
#include "camera.hpp"
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
	virtual ~Engine();

	/**
	 Runs the engine.

	 @param[in]		nCmdShow
					Controls how the engine window is to be shown.
	 */
	void Run(int nCmdShow);

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
};

#pragma endregion
