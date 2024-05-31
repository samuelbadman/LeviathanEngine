#pragma once

#include "Callback.h"

namespace LeviathanCore
{
	class InputKey;

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
		using RuntimeWindowMouseInputCallbackType = void(*)(InputKey /* key */, float /* data */);
		using RenderCallbackType = void(*)();
		using PresentCallbackType = void(*)();

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
		Callback<RuntimeWindowMouseInputCallbackType>& GetRuntimeWindowMouseInputCallback();
		Callback<RenderCallbackType>& GetRenderCallback();
		Callback<PresentCallbackType>& GetPresentCallback();

#ifdef LEVIATHAN_WITH_TOOLS
		using ImGuiRendererNewFrameCallbackType = void(*)();
		using ImGuiRenderCallbackType = void(*)();

		Callback<ImGuiRendererNewFrameCallbackType>& GetImGuiRendererNewFrameCallback();
		Callback<ImGuiRenderCallbackType>& GetImGuiRenderCallback();
#endif // LEVIATHAN_WITH_TOOLS. 

		bool PreModuleInitialization();
		bool PostModuleInitialization();
		int RunEngine();
		void Exit();
		void* GetRuntimeWindowPlatformHandle();
		bool GetRuntimeWindowRenderAreaDimensions(int& outWidth, int& outHeight);
		bool RuntimeWindowCaptureCursor();
		bool IsRuntimeWindowFullscreen();
		bool IsRuntimeWindowFocused();
		bool RuntimeWindowEnterFullscreen();
		bool RuntimeWindowExitFullscreen();
		bool SetCursorPosInRuntimeWindow(int x, int y);
		float GetDeltaSeconds();
		unsigned int GetPerfFPS();
		float GetPerfMs();
	}
}