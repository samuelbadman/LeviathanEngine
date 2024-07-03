#include "Serialize.h"

namespace LeviathanCore
{
	namespace Serialize
	{
		std::array<uint8_t, 4> UInt32ToBytes(const uint32_t uint, const Endianness endianness)
		{
			std::array<uint8_t, 4> byteBuffer = {};
			switch (endianness)
			{
			case Endianness::LittleEndian:
				byteBuffer[0] = static_cast<uint8_t>((uint >> 0) & 0xff);
				byteBuffer[1] = static_cast<uint8_t>((uint >> 8) & 0xff);
				byteBuffer[2] = static_cast<uint8_t>((uint >> 16) & 0xff);
				byteBuffer[3] = static_cast<uint8_t>((uint >> 24) & 0xff);
				break;
			case Endianness::BigEndian:
				byteBuffer[0] = static_cast<uint8_t>((uint >> 24) & 0xff);
				byteBuffer[1] = static_cast<uint8_t>((uint >> 16) & 0xff);
				byteBuffer[2] = static_cast<uint8_t>((uint >> 8) & 0xff);
				byteBuffer[3] = static_cast<uint8_t>((uint >> 0) & 0xff);
				break;
			default:
				break;
			}
			return byteBuffer;
		}

		uint32_t BytesToUInt32(const std::array<uint8_t, 4>& bytes, const Endianness endianness)
		{
			switch (endianness)
			{
			case Endianness::LittleEndian:
				return static_cast<uint32_t>(bytes[0]) + (static_cast<uint32_t>(bytes[1]) << 8) + (static_cast<uint32_t>(bytes[2]) << 16) + (static_cast<uint32_t>(bytes[3]) << 24);
			case Endianness::BigEndian:
				return (static_cast<uint32_t>(bytes[0]) << 24) + (static_cast<uint32_t>(bytes[1]) << 16) + (static_cast<uint32_t>(bytes[2]) << 8) + static_cast<uint32_t>(bytes[3]);
			default:
				return 0;
			}
		}

		std::vector<uint8_t> UInt32BufferToBytes(const std::vector<uint32_t>& uints, const Endianness endianness)
		{
			const size_t integerBufferSize = uints.size();
			std::vector<uint8_t> byteBuffer(integerBufferSize * 4);
			for (size_t i = 0, j = 0; i < integerBufferSize; ++i, j += 4)
			{
				std::array<uint8_t, 4> valueBytes = UInt32ToBytes(uints[i], endianness);
				memcpy(&byteBuffer[j], valueBytes.data(), valueBytes.size() * sizeof(uint8_t));
			}
			return byteBuffer;
		}

		std::vector<uint32_t> BytesToUInt32Buffer(const std::vector<uint8_t>& bytes, const Endianness endianness)
		{
			const size_t bytesSize = bytes.size();
			std::vector<uint32_t> buffer(bytesSize / 4);
			for (size_t i = 0, j = 0; i < bytesSize; i += 4, ++j)
			{
				std::array<uint8_t, 4> valueBytes =
				{
					bytes[i],
					bytes[i + 1],
					bytes[i + 2],
					bytes[i + 3]
				};

				buffer[j] = BytesToUInt32(valueBytes, endianness);
			}
			return buffer;
		}

		bool WriteBytesToFile(std::string_view file, const std::vector<uint8_t>& buffer)
		{
			std::ofstream ofStream(file.data(), std::ios::out | std::ios::binary);

			if (!ofStream)
			{
				return false;
			}

			ofStream.write(reinterpret_cast<const char*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));

			return true;
		}

		bool ReadFile(std::string_view file, bool binary, std::vector<uint8_t>& outBuffer)
		{
			std::ifstream ifStream;
			ifStream.open(file, ((binary) ? std::ifstream::in | std::ifstream::binary : std::ifstream::in));

			if (!ifStream.good())
			{
				return false;
			}

			outBuffer.resize(static_cast<size_t>(std::filesystem::file_size(file)), 0);
			ifStream.seekg(0, std::ios::beg);
			ifStream.read(reinterpret_cast<char*>(outBuffer.data()), static_cast<std::streamsize>(outBuffer.size()));
			ifStream.close();

			return true;
		}

		bool FileExists(std::string_view file)
		{
			return std::filesystem::exists(file);
		}

		bool MakeDirectory(std::string_view directory)
		{
			return std::filesystem::create_directory(directory);
		}
	}
}
