#pragma once

#include "Callback.h"

namespace LeviathanCore
{
	namespace Core
	{
		using PreMainLoopCallbackType = void(*)();
		using PostMainLoopCallbackType = void(*)();
		using PreTickCallbackType = void(*)();
		using TickCallbackType = void(*)(float /* deltaSeconds */);
		using PostTickCallbackType = void(*)();
		using CleanupCallbackType = void(*)();

		int RunEngine();
		void Exit();
		Callback<PreMainLoopCallbackType>& GetPreMainLoopCallback();
		Callback<PostMainLoopCallbackType>& GetPostMainLoopCallback();
		Callback<PreTickCallbackType>& GetPreTickCallback();
		Callback<TickCallbackType>& GetTickCallback();
		Callback<PostTickCallbackType>& GetPostTickCallback();
		Callback<CleanupCallbackType>& GetCleanupCallback();
	}
}