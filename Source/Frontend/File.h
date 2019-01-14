#ifndef _FILE_H_
#define _FILE_H_

namespace JoNES
{
	namespace File
	{
		int32_t ReadFile(const char* fileName, uint8_t* buffer, uint32_t bufferSize)
		{
			// Open a file handle
			FILE* handle;
			errno_t error = fopen_s(&handle, fileName, "rb");

			if (error != 0)
				return -1;

			// Read the file contents
			int32_t readBytes = fread(buffer, 1, bufferSize, handle);

			fclose(handle);

			return readBytes;
		}

		bool WriteFile(const char* fileName, uint8_t* buffer, uint32_t bufferSize)
		{
			// Open a file handle
			FILE* handle;
			errno_t error = fopen_s(&handle, fileName, "wb");

			if (error != 0)
				return false;

			// Write the buffer to the file
			size_t writtenBytes = fwrite(buffer, 1, bufferSize, handle);

			fclose(handle);

			return true;
		}

		int32_t GetFileSize(const char* fileName)
		{
			// Open a file handle
			FILE* handle;
			errno_t error = fopen_s(&handle, fileName, "rb");

			if (error != 0)
				return -1;

			fseek(handle, 0, SEEK_END);

			int32_t size = ftell(handle);

			fclose(handle);

			return size;
		}
	}
}

#endif