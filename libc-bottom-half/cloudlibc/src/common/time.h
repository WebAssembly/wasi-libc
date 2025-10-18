// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef COMMON_TIME_H
#define COMMON_TIME_H

#include <common/limits.h>

#include <sys/time.h>

#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#else
#include <wasi/api.h>
#endif
#include <stdbool.h>
#include <time.h>

#define NSEC_PER_SEC 1000000000
#define USEC_PER_SEC 1000000

static inline bool timespec_to_timestamp_exact(
#ifdef __wasilibc_use_wasip2
  const struct timespec *timespec, wall_clock_datetime_t *timestamp) {
#else
  const struct timespec *timespec, __wasi_timestamp_t *timestamp) {
#endif
  // Invalid nanoseconds field.
  if (timespec->tv_nsec < 0 || timespec->tv_nsec >= NSEC_PER_SEC)
    return false;

  // Timestamps before the Epoch are not supported.
  if (timespec->tv_sec < 0)
    return false;

#ifdef __wasilibc_use_wasip2
  timestamp->seconds = timespec->tv_sec;
  timestamp->nanoseconds = timespec->tv_nsec;
  return true;
#else
  // Make sure our timestamp does not overflow.
  return !__builtin_mul_overflow(timespec->tv_sec, NSEC_PER_SEC, timestamp) &&
         !__builtin_add_overflow(*timestamp, timespec->tv_nsec, timestamp);
#endif
}

static inline bool timespec_to_timestamp_clamp(
#ifdef __wasilibc_use_wasip2
  const struct timespec *timespec, wall_clock_datetime_t *timestamp) {
#else
  const struct timespec *timespec, __wasi_timestamp_t *timestamp) {
#endif
  // Invalid nanoseconds field.
  if (timespec->tv_nsec < 0 || timespec->tv_nsec >= NSEC_PER_SEC)
    return false;

  if (timespec->tv_sec < 0) {
    // Timestamps before the Epoch are not supported.
#if __wasilibc_use_wasip2
    timestamp->seconds = 0;
    timestamp->nanoseconds = 0;
  } else {
    timestamp->seconds = timespec->tv_sec;
    timestamp->nanoseconds = timespec->tv_nsec;
#else
 *timestamp = 0;
  } else if (__builtin_mul_overflow(timespec->tv_sec, NSEC_PER_SEC, timestamp) ||
             __builtin_add_overflow(*timestamp, timespec->tv_nsec, timestamp)) {
    // Make sure our timestamp does not overflow.
    *timestamp = NUMERIC_MAX(__wasi_timestamp_t);
#endif
  }
  return true;
}

#ifdef __wasilibc_use_wasip2
static inline struct timespec timestamp_to_timespec(
  wall_clock_datetime_t *timestamp) {
  return (struct timespec){.tv_sec = timestamp->seconds,
                           .tv_nsec = timestamp->nanoseconds};
}
#else
static inline struct timespec timestamp_to_timespec(
  __wasi_timestamp_t timestamp) {
  // Decompose timestamp into seconds and nanoseconds.
  return (struct timespec){.tv_sec = timestamp / NSEC_PER_SEC,
                           .tv_nsec = timestamp % NSEC_PER_SEC};
}
#endif

#ifdef __wasilibc_use_wasip2
static inline struct timespec instant_to_timespec(
  monotonic_clock_instant_t ns) {
    // Decompose instant into seconds and nanoseconds
  return (struct timespec){.tv_sec = ns / NSEC_PER_SEC,
                           .tv_nsec = ns % NSEC_PER_SEC};
}

static inline bool timespec_to_instant_clamp(
  const struct timespec* timespec, monotonic_clock_instant_t* instant) {

  // Invalid nanoseconds field
  if (timespec->tv_nsec < 0 || timespec->tv_nsec >= NSEC_PER_SEC)
    return false;
  if (timespec->tv_sec < 0) {
    // Timestamps before the Epoch are not supported
    *instant = 0;
  } else if (__builtin_mul_overflow(timespec->tv_sec, NSEC_PER_SEC, instant) ||
             __builtin_add_overflow(*instant, timespec->tv_nsec, instant)) {
   // Make sure our timestamp does not overflow
   *instant = NUMERIC_MAX(monotonic_clock_instant_t);
  }
  return true;
}

static inline struct timeval instant_to_timeval(
  monotonic_clock_instant_t ns) {
    // Decompose instant into seconds and microoseconds
  return (struct timeval){.tv_sec = ns / NSEC_PER_SEC,
                          .tv_usec = (ns % NSEC_PER_SEC) / 1000};
}

static inline struct timeval duration_to_timeval(
  monotonic_clock_duration_t ns) {
    // Decompose duration into seconds and microoseconds
  return (struct timeval){.tv_sec = ns / NSEC_PER_SEC,
                          .tv_usec = (ns % NSEC_PER_SEC) / 1000};
}

static inline bool timeval_to_duration(
  struct timeval* timeval, monotonic_clock_duration_t* duration) {
    // Invalid microseconds field
    if (timeval->tv_usec < 0 || timeval->tv_usec >= USEC_PER_SEC)
      return false;
    if (timeval->tv_sec < 0) {
      // Timestamps before the Epoch are not supported
      *duration = 0;
    } else if (__builtin_mul_overflow(timeval->tv_sec, NSEC_PER_SEC, duration) ||
               __builtin_add_overflow(*duration, timeval->tv_usec * 1000, duration)) {
      // Make sure our duration does not overflow
      *duration = NUMERIC_MAX(monotonic_clock_instant_t);
    }
    return true;
}

static inline struct timeval timestamp_to_timeval(
  wall_clock_datetime_t *timestamp) {
  return (struct timeval){.tv_sec = timestamp->seconds,
                          .tv_usec = timestamp->nanoseconds / 1000};
}
#else
static inline struct timeval timestamp_to_timeval(
  __wasi_timestamp_t timestamp) {
  struct timespec ts = timestamp_to_timespec(timestamp);
  return (struct timeval){.tv_sec = ts.tv_sec, ts.tv_nsec / 1000};
}
#endif
#endif
