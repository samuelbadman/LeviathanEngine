# pragma once

namespace LeviathanCore
{
	namespace Core
	{
		bool Initialize();
		bool Shutdown();
		void MainLoopIteration(float DeltaSeconds);
	}
}