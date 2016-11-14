#include "stdafx.h"

//-----------------------------------------------------------------------------
// WindowProc for handling Windows messages.
//-----------------------------------------------------------------------------
#pragma region

/**
 The application-defined function that processes messages sent to the engine window.
 The WindowProc type defines a pointer to this callback function.

 @param[in]		hWnd
				A handle to the window.
 @param[in]		msg
				The message.
 @param[in]		wParam
				Additional message information.
				The contents of this parameter depend on the value of @a msg.
 @param[in]		lParam
				Additional message information.
				The contents of this parameter depend on the value of @a msg.
 @return		The return value is the result of the message processing
				and depends on the message sent.
*/
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_PAINT: {
		// Sent when the system or another application makes a request 
		// to paint a portion of an application's window.

		// Prepare the specified window for painting and 
		// fill a PAINTSTRUCT with information about the painting and
		// return a handle to to a display device context for the specified window.
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		// Mark the end of painting in the specified window.
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY: {
		// Sent when a window is being destroyed. 
		// It is sent to the window procedure of the window 
		// being destroyed after the window is removed from the screen.

		// Indicate to the system that the window thread requests
		// to terminate (quit) with exit code 0.
		PostQuitMessage(0);
		break;
	}
	default: {
		// Calls the default window procedure to provide default processing 
		// for any window messages that an application does not process.
		// This function ensures that every message is processed.
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	}

	return 0;
}

#pragma endregion

//-----------------------------------------------------------------------------
// Engine Definitions
//-----------------------------------------------------------------------------
#pragma region

Engine::Engine(HINSTANCE hinstance) : m_loaded(false), m_hinstance(hinstance), m_renderer(nullptr) {

	//Initialize a window.
	const HRESULT result_window = InitializeWindow();
	if (FAILED(result_window)) {
		return;
	}

	// Attach a console.
	const HRESULT result_console = AttachConsole();
	if (FAILED(result_console)) {
		return;
	}
	PrintConsoleHeader();

	// Create renderer.
	m_renderer = new Renderer(m_hwindow);

	m_loaded = m_renderer->IsLoaded();
}

Engine::~Engine() {

	if (m_renderer) {
		delete m_renderer;
	}

	// Unregister the window class.
	UnregisterClass(L"WindowClass", m_hinstance);
}

HRESULT Engine::InitializeWindow() {

	// Prepare and register the window class.
	//-----------------------------------------------------------------------------
	// Structure ontaining window class information. 
	WNDCLASSEX wcex;
	// The size, in bytes, of this structure.
	wcex.cbSize = sizeof(WNDCLASSEX);
	// The class style(s)
	// CS_HREDRAW: Redraws the entire window if a movement or size adjustment changes the width  of the client area.
	// CS_VREDRAW: Redraws the entire window if a movement or size adjustment changes the height of the client area.
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	// A pointer to the window procedure.
	wcex.lpfnWndProc = WindowProc;
	// The number of extra bytes to allocate following the window-class structure.
	wcex.cbClsExtra = 0;
	// The number of extra bytes to allocate following the window instance.
	wcex.cbWndExtra = 0;
	//A handle to the instance that contains the window procedure for the class.
	wcex.hInstance = m_hinstance;
	// A handle to the class icon. This member must be a handle to an icon resource.
	wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	// A handle to the class cursor. This member must be a handle to a cursor resource.
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	// A handle to the class background brush. This member can be a handle to
	// the brush to be used for painting the background, or it can be a color value.
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	// Pointer to a null-terminated character string that specifies the resource name 
	// of the class menu, as the name appears in the resource file. 
	// If this member is nullptr, windows belonging to this class have no default menu.
	wcex.lpszMenuName = nullptr;
	// A pointer to a null-terminated string or is an atom.
	// If lpszClassName is a string, it specifies the window class name.
	wcex.lpszClassName = L"WindowClass";
	// A handle to a small icon that is associated with the window class.
	wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	// Register a window class
	if (!RegisterClassEx(&wcex)) {
		return E_FAIL;
	}
	//-----------------------------------------------------------------------------

	RECT rectangle = { 0, 0, 800, 600 };
	// Calculate the required size of the window rectangle, based on the desired client-rectangle size.
	// A client rectangle is the smallest rectangle that completely encloses a client area. 
	// A window rectangle is the smallest rectangle that completely encloses the window, which includes the client area and the nonclient area.
	// 1. A pointer to a RECT structure.
	// 2. The window style of the window.
	// 3. Flag indicating whether the window has a menu.
	AdjustWindowRect(&rectangle, WS_OVERLAPPEDWINDOW, FALSE);

	// Creates the window and retrieve a handle to it.
	// WS_OVERLAPPED: The window is an overlapped window. An overlapped window has a title bar and a border.
	// WS_CAPTION: The window has a title bar(includes the WS_BORDER style).
	// WS_BORDER: The window has a thin-line border.
	// WS_SYSMENU: The window has a window menu on its title bar.
	// WS_MINIMIZEBOX: The window has a minimize button.
	m_hwindow = CreateWindow(L"WindowClass", L"Engine Test", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, rectangle.right - rectangle.left, rectangle.bottom - rectangle.top, nullptr, nullptr, m_hinstance, nullptr);

	if (!m_hwindow) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Engine::AttachConsole() {
	// Allocate a console for basic io
	if (!AllocConsole()) {
		return E_FAIL;
	}

	FILE *stream_in, *stream_out, *stream_err;
	// Redirect stdin, stdout and stderr to the allocated console
	// Reuse stdin to open the file "CONIN$"
	freopen_s(&stream_in, "CONIN$", "r", stdin);
	// Reuse stdout to open the file "CONOUT$"
	freopen_s(&stream_out, "CONOUT$", "w", stdout);
	// Reuse stderr to open the file "CONIN$
	freopen_s(&stream_err, "CONOUT$", "w", stderr);

	return S_OK;
}

void Engine::Run(int nCmdShow) {
	if (!m_loaded) {
		return;
	}

	// Set the specified window's show state.
	ShowWindow(m_hwindow, nCmdShow);

	// Enter the message loop.
	MSG msg;
	SecureZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT) {
		// Retrieves messages for any window that belongs to the current thread
		// without performing range filtering. Furthermore messages are removed
		// after processing.
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			// Translates virtual-key messages into character messages.
			TranslateMessage(&msg);
			// Dispatches a message to a window procedure.
			DispatchMessage(&msg);
		}
		else {
			m_renderer->Render();
		}
	}
}

#pragma endregion

//-----------------------------------------------------------------------------
// Application entry point
//-----------------------------------------------------------------------------
#pragma region

/**
 The user-provided entry point for the engine.

 @param[in]		hinstance
				A handle to the current instance of the application.
 @param[in]		hPrevInstance
				A handle to the previous instance of the application.
				This parameter is always @c nullptr.
 @param[in]		lpCmdLine
				The command line for the application, excluding the program name.
 @param[in]		nCmdShow
				Controls how the window is to be shown.
 @return		If the function succeeds, terminating when it receives a WM_QUIT message,
				it returns the exit value contained in that message's @c wParam parameter.
				If the function terminates before entering the message loop, it returns 0.
 */
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, LPSTR, int nCmdShow) {
	// Create the engine, then run it.
	Engine *engine = new Engine(hinstance);
	engine->Run(nCmdShow);
	delete engine;
	
	return 0;
}

#pragma endregion