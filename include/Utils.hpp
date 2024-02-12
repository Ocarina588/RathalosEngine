#pragma once

namespace utils {

	#define TERMINAL_COLOR_RESET   "\x1B[0m"
	#define TERMINAL_COLOR_RED     "\x1B[31m"
	#define TERMINAL_COLOR_GREEN   "\x1B[32m"
	#define TERMINAL_COLOR_YELLOW  "\x1B[33m"
	#define TERMINAL_COLOR_BLUE    "\x1B[34m"
	#define TERMINAL_COLOR_MAGENTA "\x1B[35m"
	#define TERMINAL_COLOR_CYAN    "\x1B[36m"
	#define TERMINAL_COLOR_WHITE   "\x1B[37m"

	#define TAB "    "
	
	#define INVALID_UINT32 static_cast<uint32_t>(-1)
}