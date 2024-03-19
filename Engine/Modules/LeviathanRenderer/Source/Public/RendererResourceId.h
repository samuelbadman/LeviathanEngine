#pragma once

namespace LeviathanRenderer
{
	namespace RendererResourceId
	{
		using IdType = uint64_t;

		static constexpr IdType InvalidResourceId = 0;

		IdType GetAvailableId();
	}
}