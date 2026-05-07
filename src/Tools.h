#pragma once

/* Standard C++ header files. */
#include <string>
#include <vector>

/**
 * @brief Convert a vector of bytes to hex dump.
 *
 * @param bytes Bytes to convert.
 *
 * @return String containing the hex dump.
 */
std::string bytesToHex(const std::vector<uint8_t> &bytes);

/**
 * @brief Get current UTC time in milliseconds.
 *
 * @return String with current UTC time.
 *         format YYYY-MM-DDTHH:MM:SS.mmmZ (ex: "2026-05-07T14:23:45.123Z").
 */
std::string currentUtcTime();
