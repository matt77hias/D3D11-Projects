//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#pragma region

#include <SDKDDKVer.h>
#include "exception\exception.hpp"
#include "logging\dump.hpp"
#include "ui\window.hpp"

#pragma endregion

//-----------------------------------------------------------------------------
// Declarations and Definitions
//-----------------------------------------------------------------------------
using namespace mage;

namespace {

	constexpr U32x2 g_display_resolution = { 800u, 600u };
	UniquePtr< Window > g_window;

	void Init(NotNull< HINSTANCE > instance) {
		#ifdef _DEBUG
		const int debug_flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
		// Perform automatic leak checking at program exit through a call to
		// _CrtDumpMemoryLeaks and generate an error report if the application
		// failed to free all the memory it allocated.
		_CrtSetDbgFlag(debug_flags | _CRTDBG_LEAK_CHECK_DF);
		#endif

		// Add filter for unhandled exceptions.
		AddUnhandledExceptionFilter();

		// Initialize a console.
		InitializeConsole();
		Print("Copyright (c) 2016-2018 Matthias Moulin.\n");

		// Initialize a window.
		{
			auto window_desc = MakeShared< WindowDescriptor >(instance, L"Demo");
			g_window = MakeUnique< Window >(std::move(window_desc),
											L"Demo",
											g_display_resolution);
		}
	}

	[[nodiscard]]
	int Run(int nCmdShow) {
		// Show the main window.
		g_window->Show(nCmdShow);

		// Enter the message loop.
		MSG msg;
		SecureZeroMemory(&msg, sizeof(msg));
		while (WM_QUIT != msg.message) {

			// Retrieves messages for any window that belongs to the current
			// thread without performing range filtering. Furthermore messages
			// are removed after processing.
			if (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE)) {
				// Translates virtual-key messages into character messages.
				TranslateMessage(&msg);
				// Dispatches a message to a window procedure.
				DispatchMessage(&msg);
				continue;
			}

			// Nothing useful yet.
		}

		return static_cast< int >(msg.wParam);
	}
}

int WINAPI WinMain(_In_ HINSTANCE instance,
				   _In_opt_ [[maybe_unused]] HINSTANCE prev_instance,
				   _In_     [[maybe_unused]] LPSTR lpCmdLine,
				   _In_ int nCmdShow) {

	Init(NotNull< HINSTANCE >(instance));
	return Run(nCmdShow);
}