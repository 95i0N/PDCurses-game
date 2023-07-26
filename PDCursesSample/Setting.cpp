#include <stdio.h>
#include <Windows.h>

#define CHARBUFF 124

int readInt(const char* section, const char* keyword, int defaultValue, const char* filePath) {
	return GetPrivateProfileInt(section, keyword, defaultValue, filePath);
}

bool readChar(const char* section, const char* keyword, const char* defaultValue, char* returnValue, const char
	* filePath) {
	return GetPrivateProfileString(section, keyword, defaultValue, returnValue, CHARBUFF, filePath) != 0;
}

double readDouble(const char* section, const char* keyword, double defaultValue, const char* filePath) {
	char value[CHARBUFF];
	char* endp;
	readChar(section, keyword, "-1", value, filePath);

	double returnValue = strtod(value, &endp);
	return returnValue;
}

void getCurrentDirectory(char* currentDirectory) {
	GetCurrentDirectory(CHARBUFF, currentDirectory);
}

bool writeChar(const char* section, const char* keyword, const char* returnValue, const char* filePath) {
	return WritePrivateProfileString(section, keyword, returnValue, filePath);
}

bool writeInt(const char* section, const char* keyword, int returnValue, const char* filePath) {
	char value[CHARBUFF];
	sprintf_s(value, "%d", returnValue);

	return writeChar(section, keyword, value, filePath);
}

bool writeDouble(const char* section, const char* keyword, double returnValue, const char* filePath) {
	char value[CHARBUFF];
	sprintf_s(value, "%f", returnValue);

	return writeChar(section, keyword, value, filePath);
}