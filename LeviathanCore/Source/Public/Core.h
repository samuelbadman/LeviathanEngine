#pragma once

#include "Callback.h"

namespace LeviathanCore
{
	namespace Core
	{
		using PreMainLoopCallbackType = void(*)();
		using PostMainLoopCallbackType = void(*)();

		extern Callback<PreMainLoopCallbackType> PreMainLoopCallback;
		extern Callback<PostMainLoopCallbackType> PostMainLoopCallback;

		int RunEngine();
		void Exit();
	}
}