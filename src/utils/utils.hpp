#pragma once
#include <stdexcept>
#include <string>
#include <vector>
#include <windows.h>

// Container for a memory pattern where std::nullopt represents a wildcard byte.
// TODO: This used to be std::optional<uint8_t>. Maybe a nicer solution to be made here now that
// we're on C++14
using RawPattern = std::vector<uint16_t>;

namespace utils
{

// Parse an IDA-style memory pattern string. Returns a vector of bytes where std::nullopt
// represents a wildcard byte. Throws std::invalid_argument on invalid pattern.
RawPattern parsePattern(const std::string& pattern);

// Write buffer data to a memory address. Throws std::runtime_error on failure.
void writeMemoryBuffer(void* address, const std::vector<uint8_t> data);

// Write IDA-style memory pattern to a memory address. The function will jump over wildcard bytes in
// the pattern. Throws std::runtime_error or std::invalid_argument on failure.
void writeMemoryPattern(void* address, const std::string& pattern);

// Fill a memory region with a specific value. Throws std::runtime_error on failure.
void fillMemory(void* address, size_t size, uint8_t value);

// Shorthand for fillMemory(address, size, NOP opcode). Throws std::runtime_error on failure.
void nopMemory(void* address, size_t size);

// Resolve an x64 relative call instruction (E8). The address parameter should point to the desired
// function call instruction. Throws std::invalid_argument on failure.
template <typename F> inline F resolveRelativeCall(const void* address);
template <typename T> inline T resolveMovCall(void* address);

} // namespace utils

///////////////////////////////////
// TEMPLATE FUNCTION DEFINITIONS //
///////////////////////////////////

template <typename F> inline F utils::resolveRelativeCall(const void* address)
{
    static_assert(std::is_pointer_v<F> && std::is_function_v<std::remove_pointer_t<F>>,
                  "F must be a function pointer type.");
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(address);
    if (ptr == NULL || *ptr != 0xE8)
        throw std::invalid_argument("Address does not point to an E8 instruction.");
    int32_t offset;
    std::memcpy(&offset, ptr + 1, sizeof(offset));
    // Add 5 because the call address is relative to the next instruction.
    return reinterpret_cast<F>(ptr + 5 + offset);
}

template <typename T> inline T utils::resolveMovCall(void* address)
{
    // mov rel32
    static_assert(std::is_pointer_v<T>, "T must be a pointer type.");
    uint8_t* ptr = reinterpret_cast<uint8_t*>(address);
    if (ptr == NULL || *ptr != 0x8B || *(ptr + 1) != 0x0D)
        throw std::invalid_argument("Address does not point to an 8B 0D instruction.");
    int32_t offset;
    std::memcpy(&offset, ptr + 2, sizeof(offset));
    // Add 6 because the call address is relative to the next instruction.
    return reinterpret_cast<T>(ptr + 6 + offset);
}