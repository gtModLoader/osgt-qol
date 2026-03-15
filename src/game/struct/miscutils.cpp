#include "miscutils.hpp"
#include <string>
#include <vector>

std::string IntToTimeSeconds(uint32_t seconds, bool bTextFormat, bool showDays)
{
    uint32_t originalSeconds = seconds;

    uint32_t minutes = seconds / 60;
    seconds -= minutes * 60;

    uint32_t hours = minutes / 60;
    minutes -= hours * 60;

    uint32_t days = hours / 24;
    hours -= days * 24;

    std::string r;

    if (!bTextFormat)
    {
        char temp[24];

        if (hours > 0 || days > 0)
        {
            if (showDays)
                sprintf(temp, "%d:%02d:%02d", days, hours, minutes);
            else
                sprintf(temp, "%d:%02d:%02d", hours + (days * 24), minutes, seconds);
        }
        else
        {
            sprintf(temp, "%d:%02d", minutes, seconds);
        }

        return std::string(temp);
    }

    if (originalSeconds <= 0)
    {
        return "None";
    }

    if (seconds > 0)
    {
        r += std::to_string(seconds) + " secs";
    }

    if (minutes > 0)
    {
        if (!r.empty())
        {
            r = ", " + r;
        }
        std::string mins = " mins";

        if (minutes == 1)
        {
            mins = " min";
        }

        r = std::to_string(minutes) + mins + r;
    }

    if (hours > 0)
    {
        if (!r.empty())
        {
            r = ", " + r;
        }

        std::string stHours = " hours";

        if (hours == 1)
        {
            stHours = " hour";
        }

        r = std::to_string(hours) + stHours + r;
    }

    if (days > 0)
    {
        if (!r.empty())
        {
            r = ", " + r;
        }

        std::string stDays = " days";

        if (days == 1)
        {
            stDays = " day";
        }

        r = std::to_string(days) + stDays + r;
    }

    if (r.empty())
        return "Now!";
    return r;
}

std::string ToLowerCaseString(const std::string& s)
{
    std::string d(s);
    for (unsigned int i = 0; i < d.length(); i++)
    {
        d[i] = tolower(d[i]);
    }
    return d;
}

std::vector<std::string> StringTokenize(const std::string& theString,
                                        const std::string& theDelimiter)
{
    std::vector<std::string> theStringVector;

    if (!theString.empty())
    {
        size_t start = 0, end = 0;

        while (end != std::string::npos)
        {
            end = theString.find(theDelimiter, start);

            // If at end, use length=maxLength.  Else use length=end-start.
            theStringVector.push_back(theString.substr(
                start, (end == std::string::npos) ? std::string::npos : end - start));

            // If at end, use start=maxSize.  Else use start=end+delimiter.
            start = ((end > (std::string::npos - theDelimiter.size())) ? std::string::npos
                                                                       : end + theDelimiter.size());
        }
    }

    return theStringVector;
}
std::vector<std::wstring> StringTokenize(const std::wstring& theString,
                                        const std::wstring& theDelimiter)
{
    std::vector<std::wstring> theStringVector;

    if (!theString.empty())
    {
        size_t start = 0, end = 0;

        while (end != std::wstring::npos)
        {
            end = theString.find(theDelimiter, start);

            // If at end, use length=maxLength.  Else use length=end-start.
            theStringVector.push_back(theString.substr(
                start, (end == std::wstring::npos) ? std::wstring::npos : end - start));

            // If at end, use start=maxSize.  Else use start=end+delimiter.
            start = ((end > (std::wstring::npos - theDelimiter.size())) ? std::wstring::npos
                                                                       : end + theDelimiter.size());
        }
    }

    return theStringVector;
}
bool StringFromStartMatches(const std::string& line, const std::string textToMatch)
{
    for (uint32_t i = 0; i < textToMatch.size(); i++)
    {
        if (i >= line.length())
            return false;
        if (line[i] != textToMatch[i])
            return false;
    }
    return true;
}

bool StringFromEndMatches(const std::string& line, const std::string textToMatch)
{
    if (line.size() < textToMatch.size())
        return false;
    int sizeOfTextToMatch = (int)strlen(textToMatch.c_str());
    if (strncmp(&(line.c_str()[line.size() - sizeOfTextToMatch]), textToMatch.c_str(),
                sizeOfTextToMatch) == 0)
        return true;

    return false;
}

std::string GetFileExtension(std::string fileName)
{
	size_t index = fileName.find_last_of('.');
	if (index == std::string::npos)
	{
		return "";
	}

	return fileName.substr(index+1, fileName.length());
}