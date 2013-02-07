// Copyright (c) 2012, Cloudera, inc.
// Some portions Copyright (c) 2011 The LevelDB Authors.
//
// Endian-neutral encoding:
// * Fixed-length numbers are encoded with least-significant byte first
// * In addition we support variable length "varint" encoding
// * Strings are encoded prefixed by their length in varint format

#ifndef KUDU_UTIL_CODING_INL_H
#define KUDU_UTIL_CODING_INL_H

#include <stdint.h>
#include <string.h>

namespace kudu {

inline uint8_t *InlineEncodeVarint32(uint8_t *dst, uint32_t v) {
  // Operate on characters as unsigneds
  uint8_t *ptr = dst;
  static const int B = 128;
  if (v < (1<<7)) {
    *(ptr++) = v;
  } else if (v < (1<<14)) {
    *(ptr++) = v | B;
    *(ptr++) = v>>7;
  } else if (v < (1<<21)) {
    *(ptr++) = v | B;
    *(ptr++) = (v>>7) | B;
    *(ptr++) = v>>14;
  } else if (v < (1<<28)) {
    *(ptr++) = v | B;
    *(ptr++) = (v>>7) | B;
    *(ptr++) = (v>>14) | B;
    *(ptr++) = v>>21;
  } else {
    *(ptr++) = v | B;
    *(ptr++) = (v>>7) | B;
    *(ptr++) = (v>>14) | B;
    *(ptr++) = (v>>21) | B;
    *(ptr++) = v>>28;
  }
  return ptr;
}

inline void InlineEncodeFixed32(uint8_t *buf, uint32_t value) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
  memcpy(buf, &value, sizeof(value));
#else
  buf[0] = value & 0xff;
  buf[1] = (value >> 8) & 0xff;
  buf[2] = (value >> 16) & 0xff;
  buf[3] = (value >> 24) & 0xff;
#endif
}

inline void InlineEncodeFixed64(uint8_t *buf, uint64_t value) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
  memcpy(buf, &value, sizeof(value));
#else
  buf[0] = value & 0xff;
  buf[1] = (value >> 8) & 0xff;
  buf[2] = (value >> 16) & 0xff;
  buf[3] = (value >> 24) & 0xff;
  buf[4] = (value >> 32) & 0xff;
  buf[5] = (value >> 40) & 0xff;
  buf[6] = (value >> 48) & 0xff;
  buf[7] = (value >> 56) & 0xff;
#endif
}


// Standard Put... routines append to a string
template <class StrType>
inline void InlinePutFixed32(StrType *dst, uint32_t value) {
  uint8_t buf[sizeof(value)];
  InlineEncodeFixed32(buf, value);
  dst->append(buf, sizeof(buf));
}

template <class StrType>
inline void InlinePutFixed64(StrType *dst, uint64_t value) {
  uint8_t buf[sizeof(value)];
  InlineEncodeFixed64(buf, value);
  dst->append(buf, sizeof(buf));
}

template <class StrType>
inline void InlinePutVarint32(StrType* dst, uint32_t v) {
  uint8_t buf[5];
  uint8_t *ptr = InlineEncodeVarint32(buf, v);
  dst->append(buf, ptr - buf);
}

}

#endif
