#pragma once

//-----------------------------------------------------------------------------
// Engine Defines
//-----------------------------------------------------------------------------
#pragma region

#define ENGINE_VERSION_MAJOR 0
#define ENGINE_VERSION_MINOR 1
#define ENGINE_VERSION_PATCH 0

#define ENGINE_QUOTE(S) #S
#define ENGINE_STR(S) ENGINE_QUOTE(S)
#define ENGINE_VERSION_STRING ENGINE_STR(ENGINE_VERSION_MAJOR) "." ENGINE_STR(ENGINE_VERSION_MINOR) "." ENGINE_STR(ENGINE_VERSION_PATCH)

#pragma endregion

//-----------------------------------------------------------------------------
// Engine Declarations and Definitions
//-----------------------------------------------------------------------------
#pragma region

/**
 Prints the header of the engine to the console.
 */
static void PrintConsoleHeader() {
	printf("Engine version %s of %s at %s\n", ENGINE_VERSION_STRING, __DATE__, __TIME__);
	printf("Copyright (c) 2016 Matthias Moulin.\n");
	fflush(stdout);
}

#pragma endregion