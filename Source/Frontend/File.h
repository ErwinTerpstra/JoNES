#ifndef _FILE_H_
#define _FILE_H_

namespace JoNES
{
	namespace File
	{
		static bool DeleteFile(const char* fileName)
		{
			return remove(fileName) == 0;
		}

		static int32_t ReadFile(const char* fileName, uint8_t* buffer, uint32_t bufferSize)
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

		static bool WriteFile(const char* fileName, const char* text, bool append = false)
		{
			// Open a file handle
			FILE* handle;
			errno_t error = fopen_s(&handle, fileName, append ? "a" : "w");

			if (error != 0)
				return false;

			// Write the buffer to the file
			int bytesWritten = fprintf(handle, "%s", text);

			fclose(handle);

			return true;
		}

		static bool WriteFile(const char* fileName, const uint8_t* buffer, uint32_t bufferSize, bool append = false)
		{
			// Open a file handle
			FILE* handle;
			errno_t error = fopen_s(&handle, fileName, append ? "ab" : "wb");

			if (error != 0)
				return false;

			// Write the buffer to the file
			size_t writtenBytes = fwrite(buffer, 1, bufferSize, handle);

			fclose(handle);

			return true;
		}

		static int32_t GetFileSize(const char* fileName)
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