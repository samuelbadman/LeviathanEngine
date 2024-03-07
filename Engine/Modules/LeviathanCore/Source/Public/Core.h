#pragma once

#include "Callback.h"

namespace LeviathanCore
{
	namespace Core
	{
		using PreMainLoopCallbackType = void(*)();
		using PostMainLoopCallbackType = void(*)();
		using FixedTickCallbackType = void(*)(float /* fixedTimestep */);
		using PreTickCallbackType = void(*)();
		using TickCallbackType = void(*)(float /* deltaSeconds */);
		using PostTickCallbackType = void(*)();
		using CleanupCallbackType = void(*)();
		using RuntimeWindowResizedCallbackType = void(*)(int /* newWidth */, int /* newHeight */);
		using RuntimeWindowMinimizedCallbackType = void(*)();
		using RuntimeWindowMaximizedCallbackType = void(*)();
		using RuntimeWindowRestoredCallbackType = void(*)();
		using RenderCallbackType = void(*)();

		Callback<PreMainLoopCallbackType>& GetPreMainLoopCallback();
		Callback<PostMainLoopCallbackType>& GetPostMainLoopCallback();
		Callback<FixedTickCallbackType>& GetFixedTickCallback();
		Callback<PreTickCallbackType>& GetPreTickCallback();
		Callback<TickCallbackType>& GetTickCallback();
		Callback<PostTickCallbackType>& GetPostTickCallback();
		Callback<CleanupCallbackType>& GetCleanupCallback();
		Callback<RuntimeWindowResizedCallbackType>& GetRuntimeWindowResizedCallback();
		Callback<RuntimeWindowMinimizedCallbackType>& GetRuntimeWindowMinimizedCallback();
		Callback<RuntimeWindowMaximizedCallbackType>& GetRuntimeWindowMaximizedCallback();
		Callback<RuntimeWindowRestoredCallbackType>& GetRuntimeWindowRestoredCallback();
		Callback<RenderCallbackType>& GetRenderCallback();

		bool PreModuleInitialization();
		bool PostModuleInitialization();
		int RunEngine();
		void Exit();
		void* GetRuntimeWindowPlatformHandle();
		bool GetRuntimeWindowRenderAreaDimensions(int& outWidth, int& outHeight);
		float GetDeltaSeconds();
	}
}