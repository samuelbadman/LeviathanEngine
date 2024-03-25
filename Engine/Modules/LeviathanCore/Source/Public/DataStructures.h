#pragma once
#include "LeviathanAssert.h"

namespace LeviathanCore
{
	namespace DataStructures
	{
		template <typename T>
		class SparseArray
		{
		private:
			std::vector<T> Dense = {}; // Implemented with dynamic array to save memory. O(n) space where n is the number of values stored.
			std::vector<size_t> Sparse = {};

		public:
			SparseArray(const size_t sparseSize, const size_t denseReserveSize = 0)
			{
				Dense.reserve(denseReserveSize);
				Sparse.resize(sparseSize, std::numeric_limits<size_t>::max());
			}

			void Add(const size_t id, T&& value)
			{
				LEVIATHAN_ASSERT(id < Sparse.size());

				Sparse[id] = Dense.size();
				Dense.emplace_back(std::move(value));
			}

			void Add(const size_t id, const T& value)
			{
				LEVIATHAN_ASSERT(id < Sparse.size());

				Sparse[id] = Dense.size();
				Dense.push_back(value);
			}

			const T& GetValue(const size_t id)
			{
				LEVIATHAN_ASSERT(id < Sparse.size());
				LEVIATHAN_ASSERT(Sparse[id] < Dense.size());

				return Dense[Sparse[id]];
			}

			bool IsValidID(const size_t id)
			{
				return (Sparse[id] < Dense.size());
			}

			void GetValues(const T*& pValues, size_t& valueCount) const
			{
				pValues = &Dense[0];
				valueCount = Dense.size();
			}
		};
	}
}