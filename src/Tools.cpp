/* Standard C++ header files. */
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstdint>

/* Local header files. */
#include "Tools.h"

std::string bytesToHex(const std::vector<uint8_t> &bytes)
{
  std::ostringstream oss;

  oss << std::hex << std::uppercase << std::setfill('0');

  for (uint8_t byte : bytes) {
    oss << std::setw(2) << static_cast<int>(byte);
  }

  return oss.str();
}

std::string currentUtcTime()
{
  using namespace std::chrono;

  const auto now = system_clock::now();
  const auto millis = duration_cast<milliseconds>(
                        now.time_since_epoch()
                      ) % 1000;

  const std::time_t nowTime = system_clock::to_time_t(now);

  std::tm utcTm{};
  gmtime_r(&nowTime, &utcTm); // Linux / POSIX

  std::ostringstream oss;
  oss << std::put_time(&utcTm, "%Y-%m-%dT%H:%M:%S");
  oss << "." << std::setw(3) << std::setfill('0') << millis.count();
  oss << "Z";

  return oss.str();
}