#include "utils.hpp"
#include <cstdint>
#include <sstream>

namespace utils
{

RawPattern parsePattern(const std::string& pattern)
{
    RawPattern out;
    std::istringstream ss(pattern);
    std::string byte;
    // Parse pattern in tokens.
    while (ss >> byte)
    {
        if (byte == "?" || byte == "??")
            out.emplace_back(std::nullopt);
        else
        {
            try
            {
                out.emplace_back(std::stoi(byte, nullptr, 16));
            }
            catch (...)
            {
                // Return empty vector in case of an invalid pattern.
                return {};
            }
        }
    }
    return out;
}

bool writeMemoryBuffer(void* address, const std::vector<uint8_t> data)
{
    DWORD old = 0;
    if (!VirtualProtect(address, data.size(), PAGE_EXECUTE_READWRITE, &old))
        return false;
    std::memcpy(address, data.data(), data.size());
    return VirtualProtect(address, data.size(), old, &old);
}

bool writeMemoryPattern(void* address, const std::string& pattern)
{
    auto bytes = parsePattern(pattern);
    if (bytes.empty())
        return false;

    DWORD old = 0;
    if (!VirtualProtect(address, bytes.size(), PAGE_EXECUTE_READWRITE, &old))
        return false;
    // Perform special write that skips wildcard bytes.
    uint8_t* ptr = reinterpret_cast<uint8_t*>(address);
    for (size_t i = 0; i < bytes.size(); i++)
    {
        if (bytes[i].has_value())
            *ptr = bytes[i].value();
        ptr++;
    }
    return VirtualProtect(address, bytes.size(), old, &old);
}

bool fillMemory(void* address, size_t size, uint8_t value)
{
    DWORD old = 0;
    if (!VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &old))
        return false;
    std::memset(address, value, size);
    return VirtualProtect(address, size, old, &old);
}

bool nopMemory(void* address, size_t size) { return fillMemory(address, size, 0x90); }

} // namespace utils