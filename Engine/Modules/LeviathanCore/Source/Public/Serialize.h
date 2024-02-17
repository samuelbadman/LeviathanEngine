#pragma once

namespace LeviathanCore
{
	namespace Serialize
	{
		enum class Endianness : uint8_t
		{
			LittleEndian,
			BigEndian
		};

		std::array<uint8_t, 4> UInt32ToBytes(const uint32_t uint, const Endianness endianness);

		uint32_t BytesToUInt32(const std::array<uint8_t, 4>& bytes, const Endianness endianness);

		std::vector<uint8_t> UInt32BufferToBytes(const std::vector<uint32_t>& uints, const Endianness endianness);

		std::vector<uint32_t> BytesToUInt32Buffer(const std::vector<uint8_t>& bytes, const Endianness endianness);

		// Writes the contents of the byte buffer to disk in the file location specified. Returns true if successful otherwise, returns false if the function fails.
		bool WriteBytesToFile(std::string_view file, const std::vector<uint8_t>& buffer);

		// Reads the contents of the file location specified into the out byte buffer. Returns true if succesful otherwise, returns false if the function fails.
		bool ReadBytesFromFile(std::string_view file, std::vector<uint8_t>& outBuffer);

		// Checks if the file or directory exists. Returns true if it does otherwise false.
		bool FileExists(std::string_view file);

		// Creates a new directory. Returns true if a directory was created otherwise false.
		bool MakeDirectory(std::string_view directory);
	}
}