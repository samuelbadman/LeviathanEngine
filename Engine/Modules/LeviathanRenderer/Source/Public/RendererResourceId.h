#pragma once

namespace LeviathanRenderer
{
	namespace RendererResourceID
	{
		using IDType = uint64_t;

		static constexpr IDType InvalidID = 0;

		IDType GetAvailableID();
	}
}