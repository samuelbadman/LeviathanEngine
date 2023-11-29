#pragma once

enum class Win32FunctionError : unsigned char
{
	Success = 0,
	AllocConsoleFailed,
	FreeConsoleFailed,
	ErrorMax
};