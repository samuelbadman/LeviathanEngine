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
			static constexpr size_t InvalidID = std::numeric_limits<size_t>::max();

		public:
			struct DenseValue
			{
				T Value = {};
				size_t ReferencedByID = InvalidID;
			};

		private:
			// Dense array implemented with dynamic array to save memory. O(n) space where n is the number of values stored.
			// Can use a fixed allocation to improved insertion time to O(1) without requiring an allocation and copy constructing.
			std::vector<DenseValue> Dense = {}; 
			std::vector<size_t> Sparse = {};
			size_t DenseSize = 0;

		public:
			SparseArray(const size_t maxID, const size_t denseReserveSize = 0)
			{
				Dense.reserve(denseReserveSize);
				Sparse.resize(maxID, InvalidID);
			}

			void Add(const size_t id, T&& value)
			{
				LEVIATHAN_ASSERT(id < Sparse.size());

				// Use std::vector strategy to dynamically allocate more memory as values are added.

				// Move the value onto the end of the dense array, set the sparse index for the id to point to the current end of the dense array (the added value), and increment end index of dense array.
				Dense.emplace_back(DenseValue{ std::move(value), id });
				Sparse[id] = DenseSize++;
			}

			void Add(const size_t id, const T& value)
			{
				LEVIATHAN_ASSERT(id < Sparse.size());

				// Use std::vector strategy to dynamically allocate more memory as values are added.

				// Copy the value onto the end of the dense array, set the sparse index for the id to point to the current end of the dense array (the added value), and increment end index of dense array.
				Dense.emplace_back(DenseValue{ value, id });
				Sparse[id] = DenseSize++;
			}

			void Remove(const size_t id)
			{
				// Removes the value in the dense array pointed to by id in constant time O(1).
				LEVIATHAN_ASSERT(id < Sparse.size());

				if (!IsValidID(id))
				{
					return;
				}

				// Swap the removed value with the last value in the dense array.
				DenseValue temp = Dense[Sparse[id]];
				Dense[Sparse[id]] = Dense[DenseSize - 1];
				Dense[DenseSize - 1] = temp;

				// Repoint id referencing the old end value to the swapped position and invalidate the new end value referenced by ID as the value is no longer being referenced by an ID.
				Sparse[Dense[Sparse[id]].ReferencedByID] = Sparse[id];
				Dense[DenseSize - 1].ReferencedByID = InvalidID;

				// Decrement dense size.
				--DenseSize;

				// Invalidate removed id.
				Sparse[id] = InvalidID;
			}

			const DenseValue& GetValue(const size_t id)
			{
				LEVIATHAN_ASSERT(IsValidID(id));

				return Dense[Sparse[id]];
			}

			bool IsValidID(const size_t id)
			{
				LEVIATHAN_ASSERT(id < Sparse.size());

				return (Sparse[id] < DenseSize);
			}

			// Reallocates memory for the data structure at the current size of the dense array. Removed values are destoyed.
			void Refit()
			{
				// No need to refit the dense array if the current end of the dense array is equal to the end of the allocated memory. Only refit when the allocation is larger than the current end of the array.
				if (DenseSize == Dense.size())
				{
					return;
				}

				// Refit the dense array. Requires reallocation and copying values to new array.
				Dense = std::vector<DenseValue>(Dense.begin(), Dense.begin() + DenseSize);
			}

			void GetValues(const DenseValue*& outValues, size_t& outCount) const
			{
				outValues = &Dense[0];
				outCount = DenseSize;
			}
		};
	}
}