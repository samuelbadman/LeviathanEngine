#pragma once

#include "Callback.h"

namespace LeviathanCore
{
	namespace Core
	{
		using PreMainLoopCallbackType = void(*)();
		using PostMainLoopCallbackType = void(*)();
		using TickCallbackType = void(*)(float /* deltaSeconds */);
		using CleanupCallbackType = void(*)();

		int RunEngine();
		void Exit();
		Callback<PreMainLoopCallbackType>& GetPreMainLoopCallback();
		Callback<PostMainLoopCallbackType>& GetPostMainLoopCallback();
		Callback<TickCallbackType>& GetTickCallback();
		Callback<CleanupCallbackType>& GetCleanupCallback();
	}
}