#pragma once

namespace TestTitle
{
	bool Initialize();
	void OnPreMainLoop();
	void OnPostMainLoop();
	void OnTick(float DeltaSeconds);
	void OnCleanup();
}