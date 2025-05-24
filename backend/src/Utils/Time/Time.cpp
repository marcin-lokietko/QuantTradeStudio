#include "Time.hpp"

#include <chrono>
#include <thread>

namespace Time {

void Time::sleepFor(const ::std::chrono::seconds& seconds) const { ::std::this_thread::sleep_for(seconds); }

}  // namespace Time