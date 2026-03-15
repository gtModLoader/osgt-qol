#pragma once
#include <cstdint>
#include <string>
#include <vector>

std::string IntToTimeSeconds(uint32_t seconds, bool bTextFormat, bool showDays);
std::string ToLowerCaseString(const std::string& s);

std::vector<std::string> StringTokenize(const std::string& theString, const std::string& theDelimiter);
std::vector<std::wstring> StringTokenize(const std::wstring& theString, const std::wstring& theDelimiter);
bool StringFromStartMatches(const std::string& line, const std::string textToMatch);
bool StringFromEndMatches(const std::string& line, const std::string textToMatch);

std::string GetFileExtension(std::string fileName);