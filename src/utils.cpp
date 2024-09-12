#include "utils.hpp"
#include <cstdint>
#include <sstream>
#include <stdexcept>

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
                throw std::invalid_argument("Invalid pattern byte: " + byte);
            }
        }
    }
    return out;
}

void writeMemoryBuffer(void* address, const std::vector<uint8_t> data)
{
    DWORD old = 0;
    if (!VirtualProtect(address, data.size(), PAGE_EXECUTE_READWRITE, &old))
        throw std::runtime_error("Failed to change memory protection.");
    std::memcpy(address, data.data(), data.size());
    if (!VirtualProtect(address, data.size(), old, &old))
        throw std::runtime_error("Failed to restore memory protection.");
}

void writeMemoryPattern(void* address, const std::string& pattern)
{
    auto bytes = parsePattern(pattern);
    DWORD old = 0;
    if (!VirtualProtect(address, bytes.size(), PAGE_EXECUTE_READWRITE, &old))
        throw std::runtime_error("Failed to change memory protection.");
    // Perform special write that skips wildcard bytes.
    uint8_t* ptr = reinterpret_cast<uint8_t*>(address);
    for (size_t i = 0; i < bytes.size(); i++)
    {
        if (bytes[i].has_value())
            *ptr = bytes[i].value();
        ptr++;
    }
    if (!VirtualProtect(address, bytes.size(), old, &old))
        throw std::runtime_error("Failed to restore memory protection.");
}

void fillMemory(void* address, size_t size, uint8_t value)
{
    DWORD old = 0;
    if (!VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &old))
        throw std::runtime_error("Failed to change memory protection.");
    std::memset(address, value, size);
    if (!VirtualProtect(address, size, old, &old))
        throw std::runtime_error("Failed to restore memory protection.");
}

void nopMemory(void* address, size_t size) { fillMemory(address, size, 0x90); }

} // namespace utils