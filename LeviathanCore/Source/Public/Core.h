#pragma once

#include "Callback.h"

namespace LeviathanCore
{
	namespace Core
	{
		using PreMainLoopCallbackType = void(*)();
		using PostMainLoopCallbackType = void(*)();
		using TickCallbackType = void(*)(float);

		extern Callback<PreMainLoopCallbackType> PreMainLoopCallback;
		extern Callback<PostMainLoopCallbackType> PostMainLoopCallback;
		extern Callback<TickCallbackType> TickCallback;

		int RunEngine();
		void Exit();
	}
}