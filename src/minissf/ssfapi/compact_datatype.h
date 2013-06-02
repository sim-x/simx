/**
 * \file compact_datatype.h
 * \brief Header file for the compact data type for serialization.
 *
 * This header file contains the definition of the CompactDataType
 * class; Users do not need to include this header file directly; it
 * is included from ssf.h.
 */

#ifndef __MINISSF_COMPACT_H__
#define __MINISSF_COMPACT_H__

#include <sys/param.h>
#include "ssfapi/ssf_common.h"

namespace minissf {

// Endian conversions: we don't need to do anything if the execution
// environment is homogeneous. That is, all machines running the
// simulation are of the same architecture. Note that running on a
// shared-memory multiprocessor machine or running a sequential
// simulation is considered as homogeneous. We only need to change the
// layout of the primitive data types to the little-endian format
// (i.e., least significant bit first). We chose little endian as the
// default layout since Intel x86 machines are the most popular.

#ifdef SSF_LITTLE_ENDIAN
#undef SSF_LITTLE_ENDIAN
#endif

#ifdef SSF_BIG_ENDIAN
#undef SSF_BIG_ENDIAN
#endif

#ifdef __BYTE_ORDER
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define SSF_LITTLE_ENDIAN
#elif __BYTE_ORDER == __BIG_ENDIAN
#define SSF_BIG_ENDIAN
#else
#error "unknown endian type!"
#endif
#else // if __BYTE_ORDER is not defined, we assume little endian
#define SSF_LITTLE_ENDIAN
#endif

#if defined(SSF_LITTLE_ENDIAN) || defined(HOMOGENEOUS_ENVIRONMENT)
#define SSF_NO_ENDIAN_FORMATTING
#endif

#ifdef SSF_NO_ENDIAN_FORMATTING
#define SSF_BYTE_ORDER_16(x) (x)
#define SSF_BYTE_ORDER_32(x) (x)
#define SSF_BYTE_ORDER_64(x) (x)
#else
#define SSF_BYTE_ORDER_16(x) \
   ((((prime::ssf::uint16)(x) & 0xff00) >> 8) | \
    (((prime::ssf::uint16)(x) & 0x00ff) << 8))
#define SSF_BYTE_ORDER_32(x) \
   ((((prime::ssf::uint32)(x) & 0xff000000) >> 24) | \
    (((prime::ssf::uint32)(x) & 0x00ff0000) >> 8)  | \
    (((prime::ssf::uint32)(x) & 0x0000ff00) << 8)  | \
    (((prime::ssf::uint32)(x) & 0x000000ff) << 24))
#define SSF_BYTE_ORDER_64(x) \
   ((prime::ssf::uint64(SSF_BYTE_ORDER_32((((prime::ssf::uint64)(x))<<32)>>32))<<32) | \
    SSF_BYTE_ORDER_32(((prime::ssf::uint64)(x))>>32))
#endif

/** 
 * \brief Compact data format for data serialization.
 *
 * Data serialization is needed for message passing between
 * distributed machines that potentially have heterogeneous
 * architectures (e.g., with different endian types). It is also
 * useful for collecting statistics to be stored in a platform
 * independent file. 
 *
 * This class offers a simple (yet flexible) facility for data
 * serialization. It is used both by the user and the simulator kernel
 * to store data in a memory efficient manner while maintaining
 * platform independence. The compact data type can be used on any
 * platform regardless of the machine architecture (or endian type of
 * the machine).
 */
class CompactDataType {
public:
  /** \brief The constructor of a compact data type. */
  CompactDataType();

  /** \brief The destructor of a compact data type. */
  virtual ~CompactDataType();

  /** @name Pack a single primitive value. */
  /* @{ */
  void add_float(float val); ///< pack a float
  void add_double(double val);  ///< pack a double
  void add_char(char val);  ///< pack a character
  void add_unsigned_char(unsigned char val);  ///< pack an unsigned character
  void add_short(short val); ///< pack a short
  void add_unsigned_short(unsigned short val); ///< pack an unsigned short
  void add_int(int val); ///< pack an integer
  void add_unsigned_int(unsigned int val); ///< pack an unsigned integer
  void add_long(long val); ///< pack a long integer
  void add_unsigned_long(unsigned long val); ///< pack an unsigned long integer
#ifdef HAVE_LONG_LONG_INT
  void add_long_long(long long val);  ///< pack a long long integer
  void add_unsigned_long_long(unsigned long long val); ///< pack an unsigned long long integer
#endif
  void add_virtual_time(VirtualTime val); ///< pack a virtual time
  /** @} */

  /** @name Pack an array of primitive values. */
  /** @{ */
  void add_float_array(int nitems, float* val_array); ///< Pack an array of floats
  void add_double_array(int nitems, double* val_array); ///< Pack an array of doubles
  void add_char_array(int nitems, char* val_array); ///< Pack a character array
  void add_unsigned_char_array(int nitems, unsigned char* val_array); ///< Pack an unsigned character array
  void add_short_array(int nitems, short* val_array); ///< Pack an array of shorts
  void add_unsigned_short_array(int nitems, unsigned short* val_array); ///< Pack an array of unsigned shorts
  void add_int_array(int nitems, int* val_array); ///< Pack an array of integers
  void add_unsigned_int_array(int nitems, unsigned int* val_array); ///< Pack an array of unsigned integers
  void add_long_array(int nitems, long* val_array); ///< Pack an array of long integers
  void add_unsigned_long_array(int nitems, unsigned long* val_array); ///< Pack an array of unsigned long integers
#ifdef HAVE_LONG_LONG_INT
  void add_long_long_array(int nitems, long long* val_array); ///< Pack an array of long long integers
  void add_unsigned_long_long_array(int nitems, unsigned long long* val_array); ///< Pack an array of unsigned long long integers
#endif
  void add_virtual_time_array(int nitems, VirtualTime* val_array); ///< Pack an array of virtual times
  void add_string(const char* valstr); ///< Pack a null-terminated character string
  /** @} */

  /** @name Unpack an array of primitive values.
   *
   * All these functions require a pointer to the buffer where
   * unpacked data will be stored.The user can specify the number of
   * items to be retrived.  By default, the functions unpack only one
   * data item. The functions return the number of items really get
   * to be unpacked.
   */
  /** @{ */
  int get_float(float* addr, int nitems = 1); ///< Unpack a number of floats.
  int get_double(double* addr, int nitems = 1); ///< Unpack a number of doubles.
  int get_char(char* addr, int nitems = 1); ///< Unpack a number of characters.
  int get_unsigned_char(unsigned char* addr, int nitems = 1); ///< Unpack a number of unsigned characters.
  int get_short(short* addr, int nitems = 1); ///< Unpack a number of shorts.
  int get_unsigned_short(unsigned short* addr, int nitems = 1); ///< Unpack a number of unsigned shorts.
  int get_int(int* addr, int nitems = 1); ///< Unpack a number of integers.
  int get_unsigned_int(unsigned int* addr, int nitems = 1); ///< Unpack a number of unsigned integers.
  int get_long(long* addr, int nitems = 1); ///< Unpack a number of long integers.
  int get_unsigned_long(unsigned long* addr, int nitems = 1); ///< Unpack a number of unsigned long integers.
#ifdef HAVE_LONG_LONG_INT
  int get_long_long(long long* addr, int nitems = 1); ///< Unpack a number of long long integers.
  int get_unsigned_long_long(unsigned long long* addr, int nitems = 1); ///< Unpack a number of unsigned long long integers.
#endif
  int get_virtual_time(VirtualTime* addr, int nitems = 1); ///< Unpack a number of virtual times.
  char* get_string(); ///< Unpack to a null-terminated string (user manages the buffer).
  /** @} */

  /** @name Generic "static" methods for serializing primitives (of given size). 
   *
   * These functions pack data of a given primitive type into the
   * buffer, provided in the second argument, and the given offset
   * position, given in the third argument. They return the pointer to
   * the buffer, and update the position.
   */
  /** @{ */
  static void serialize(uint8 data,  char* buf, int bufsiz, int* pos = 0); ///< Pack an 8-bit unsigned character.
  static void serialize(uint16 data, char* buf, int bufsiz, int* pos = 0); ///< Pack a 16-bit unsigned integer.
  static void serialize(uint32 data, char* buf, int bufsiz, int* pos = 0); ///< Pack a 32-bit unsigned integer.
  static void serialize(uint64 data, char* buf, int bufsiz, int* pos = 0); ///< Pack a 64-bit unsigned integer.
  static void serialize(int8 data,   char* buf, int bufsiz, int* pos = 0); ///< Pack an 8-bit signed character.
  static void serialize(int16 data,  char* buf, int bufsiz, int* pos = 0); ///< Pack a 16-bit signed integer.
  static void serialize(int32 data,  char* buf, int bufsiz, int* pos = 0); ///< Pack a 32-bit signed integer.
  static void serialize(int64 data,  char* buf, int bufsiz, int* pos = 0); ///< Pack a 64-bit signed integer.
  static void serialize(float data,  char* buf, int bufsiz, int* pos = 0); ///< Pack a floating point number.
  static void serialize(double data, char* buf, int bufsiz, int* pos = 0); ///< Pack a double floating point number.
  /** @} */

  /** @name Generic "static" methods for deserializing primitives (of given size).
   *
   * These functions unpack data of a given primitive type from the
   * buffer, provided in the second argument, at the given offset
   * position, provided as the third argument. The data is returned in
   * the first argument; the position is updated.
   */
  /** @{ */
  static void deserialize(uint8& data,  char* buf, int bufsiz, int* pos = 0); ///< Unpack an 8-bit unsigned character.
  static void deserialize(uint16& data, char* buf, int bufsiz, int* pos = 0); ///< Unpack a 16-bit unsigned integer.
  static void deserialize(uint32& data, char* buf, int bufsiz, int* pos = 0); ///< Unpack a 16-bit unsigned integer.
  static void deserialize(uint64& data, char* buf, int bufsiz, int* pos = 0); ///< Unpack a 64-bit unsigned integer.
  static void deserialize(int8& data,   char* buf, int bufsiz, int* pos = 0); ///< Unpack an 8-bit signed character.
  static void deserialize(int16& data,  char* buf, int bufsiz, int* pos = 0); ///< Unpack a 16-bit signed integer.
  static void deserialize(int32& data,  char* buf, int bufsiz, int* pos = 0); ///< Unpack a 32-bit signed integer.
  static void deserialize(int64& data,  char* buf, int bufsiz, int* pos = 0); ///< Unpack a 64-bit signed integer.
  static void deserialize(float& data,  char* buf, int bufsiz, int* pos = 0); ///< Unpack a floating point number.
  static void deserialize(double& data, char* buf, int bufsiz, int* pos = 0); ///< Unpack a double floating point number.
  /** @} */

 protected:
  // all recognizable data types are enumerated here
  enum { 
    TYPE_FLOAT, TYPE_DOUBLE, TYPE_LONG_DOUBLE,
    TYPE_INT8,  TYPE_INT16,  TYPE_INT32,  TYPE_INT64,
    TYPE_UINT8, TYPE_UINT16, TYPE_UINT32, TYPE_UINT64,
    TYPE_VIRTUAL_TIME, TYPE_STRING
  };

  int* compact_record; // struct { int type; int size; };
  int compact_rec_capacity;
  int compact_rec_add_offset;
  int compact_rec_get_offset;

  unsigned char* compact_data;
  int compact_capacity;
  int compact_add_offset;
  int compact_get_offset;
  
  void compact_reset();
  void compact_add_record(int type, int size);
  void compact_append_raw(int ch, int size, unsigned char* buf);
  void compact_retrieve_raw(int ch, int size, unsigned char* buf);

 public:
#ifdef HAVE_MPI_H
  // packing and unpacking to and from a byte array using mpi
  int pack(char* buf, int bufsiz);
  int pack_and_delete(char* buf, int bufsiz); // reclaiming this after use!
  void unpack(char* buf, int siz);
#endif
}; // class CompactDataType

}; /*namespace minissf*/

#endif /*__MINISSF_COMPACT_H__*/

/*
 * Copyright (c) 2011-2013 Florida International University.
 *
 * Permission is hereby granted, free of charge, to any individual or
 * institution obtaining a copy of this software and associated
 * documentation files (the "software"), to use, copy, modify, and
 * distribute without restriction.
 *
 * The software is provided "as is", without warranty of any kind,
 * express or implied, including but not limited to the warranties of
 * merchantability, fitness for a particular purpose and
 * noninfringement.  In no event shall Florida International
 * University be liable for any claim, damages or other liability,
 * whether in an action of contract, tort or otherwise, arising from,
 * out of or in connection with the software or the use or other
 * dealings in the software.
 *
 * This software is developed and maintained by
 *
 *   Modeling and Networking Systems Research Group
 *   School of Computing and Information Sciences
 *   Florida International University
 *   Miami, Florida 33199, USA
 *
 * You can find our research at http://www.primessf.net/.
 */
