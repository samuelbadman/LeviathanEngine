#include "PerfStatsDisplay.h"
#include "LeviathanString.h"

void LeviathanTools::PerfStatsDisplay::Render([[maybe_unused]] const unsigned int FPS, [[maybe_unused]] const float Ms)
{
	ImGui::SetNextWindowSize(ImVec2(200.0f, 100.0f), ImGuiCond_FirstUseEver);
	ImGui::Begin("Perf stats");
	ImGui::Text(LeviathanCore::String::Printf("FPS: %d\nMs: %f", FPS, Ms).c_str());
	ImGui::End();
}
