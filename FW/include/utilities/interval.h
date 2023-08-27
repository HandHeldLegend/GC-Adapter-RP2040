#ifndef INTERVAL_H
#define INTERVAL_H

#include <stdint.h>
#include <stdbool.h>

// This is a static inline function that will return
// a value of 'true' if an interval is met.
// IE helps you run functions at set intervals without blocking (as much).
static inline bool interval_run(uint32_t timestamp, uint32_t interval)
{
  static uint32_t last_time = 0;
  static uint32_t this_time = 0;

  this_time = timestamp;

  // Clear variable
  uint32_t diff = 0;

  // Handle edge case where time has
  // looped around and is now less
  if (this_time < last_time)
  {
    diff = (0xFFFFFFFF - last_time) + this_time;
  }
  else if (this_time > last_time)
  {
    diff = this_time - last_time;
  }
  else
    return false;

  // We want a target rate according to our variable
  if (diff >= interval)
  {
    // Set the last time
    last_time = this_time;
    return true;
  }
  return false;
}

// This is a function that will return
// offers the ability to restart the timer
static inline bool interval_resettable_run(uint32_t timestamp, uint32_t interval, bool reset)
{
  static uint32_t last_time = 0;
  static uint32_t this_time = 0;

  this_time = timestamp;

  if (reset)
  {
    last_time = this_time;
    return false;
  }

  // Clear variable
  uint32_t diff = 0;

  // Handle edge case where time has
  // looped around and is now less
  if (this_time < last_time)
  {
    diff = (0xFFFFFFFF - last_time) + this_time;
  }
  else if (this_time > last_time)
  {
    diff = this_time - last_time;
  }
  else
    return false;

  // We want a target rate according to our variable
  if (diff >= interval)
  {
    // Set the last time
    last_time = this_time;
    return true;
  }
  return false;
}

#endif