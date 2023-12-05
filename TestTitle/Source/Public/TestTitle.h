#pragma once

namespace TestTitle
{
	void Initialize();
	void OnPreMainLoop();
	void OnPostMainLoop();
	void OnTick(float DeltaSeconds);
	void OnCleanup();
}