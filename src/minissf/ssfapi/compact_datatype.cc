#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "kernel/ssfmachine.h"
#include "ssfapi/compact_datatype.h"

namespace minissf {

static int compact_make_alignment(int offset, int size) {
  int align;
  if (size == 1) align = 1;
  else if (size <= 2) align = 2;
  else if (size <= 4) align = 4;
  else if (size <= 8) align = 8;
  else if (size <= 16) align = 16;
  else if (size <= 32) align = 32;
  else if (size <= 64) align = 64;
  else align = 128;
  return ((offset + (align - 1)) & (~(align - 1)));
}

CompactDataType::CompactDataType() : compact_record(0), compact_data(0) { compact_reset(); }
CompactDataType::~CompactDataType() { compact_reset(); }

void CompactDataType::add_float(float val) { add_float_array(1, &val); }
void CompactDataType::add_double(double val) { add_double_array(1, &val); }
void CompactDataType::add_char(char val) { add_char_array(1, &val); }
void CompactDataType::add_unsigned_char(unsigned char val) { add_unsigned_char_array(1, &val); }
void CompactDataType::add_short(short val) { add_short_array(1, &val); }
void CompactDataType::add_unsigned_short(unsigned short val) { add_unsigned_short_array(1, &val); }
void CompactDataType::add_int(int val) { add_int_array(1, &val); }
void CompactDataType::add_unsigned_int(unsigned int val) { add_unsigned_int_array(1, &val); }
void CompactDataType::add_long(long val) { add_long_array(1, &val); }
void CompactDataType::add_unsigned_long(unsigned long val) { add_unsigned_long_array(1, &val); }
#ifdef HAVE_LONG_LONG_INT
void CompactDataType::add_long_long(long long val) { add_long_long_array(1, &val); }
void CompactDataType::add_unsigned_long_long(unsigned long long val) { add_unsigned_long_long_array(1, &val); }
#endif
void CompactDataType::add_virtual_time(VirtualTime val) { add_virtual_time_array(1, &val); }

void CompactDataType::add_float_array(int size, float* vals) {
 assert(sizeof(float) == 4); // assuming IEEE 754 compliant; check should be done at configuration/compilation time
 compact_add_record(TYPE_FLOAT, size);
 compact_append_raw(size, sizeof(float), (unsigned char*) vals);
}
void CompactDataType::add_double_array(int size, double* vals) {
 assert(sizeof(double) == 8); // assuming IEEE 754 compliant; check should be done at configuration/compilation time
 compact_add_record(TYPE_DOUBLE, size);
 compact_append_raw(size, sizeof(double), (unsigned char*) vals);
}
void CompactDataType::add_char_array(int size, char* vals) {
 compact_add_record(TYPE_INT8, size);
 compact_append_raw(size, sizeof(char), (unsigned char*) vals);
}
void CompactDataType::add_unsigned_char_array(int size, unsigned char* vals) {
 compact_add_record(TYPE_UINT8, size);
 compact_append_raw(size, sizeof(unsigned char), (unsigned char*) vals);
}
void CompactDataType::add_short_array(int size, short* vals) {
 assert(sizeof(short) == sizeof(int16)); // assuming short is always 16-bit
 compact_add_record(TYPE_INT16, size);
 compact_append_raw(size, sizeof(short), (unsigned char*) vals);
}
void CompactDataType::add_unsigned_short_array(int size, unsigned short* vals) {
 assert(sizeof(unsigned short) == sizeof(uint16)); // assuming short is always 16-bit
 compact_add_record(TYPE_UINT16, size);
 compact_append_raw(size, sizeof(unsigned short), (unsigned char*) vals);
}
void CompactDataType::add_int_array(int size, int* vals) {
 if (sizeof(int) == sizeof(int32))
  compact_add_record(TYPE_INT32, size);
 else if (sizeof(int) == sizeof(int64))
  compact_add_record(TYPE_INT64, size);
 else  assert(0); // assuming int has either 32 or 64 bits
 compact_append_raw(size, sizeof(int), (unsigned char*)vals);
}
void CompactDataType::add_unsigned_int_array(int size, unsigned int* vals) {
  if(sizeof(unsigned int) == sizeof(uint32)) compact_add_record(TYPE_UINT32, size);
  else if(sizeof(unsigned int) == sizeof(uint64)) compact_add_record(TYPE_UINT64, size);
  else assert(0); // assuming int has either 32 or 64 bits
  compact_append_raw(size, sizeof(unsigned int), (unsigned char*)vals);
}
void CompactDataType::add_long_array(int size, long* vals) {
  if(sizeof(long) == sizeof(int32)) compact_add_record(TYPE_INT32, size);
  else if(sizeof(long) == sizeof(int64)) compact_add_record(TYPE_INT64, size);
  else assert(0); // assuming long has either 32 or 64 bits
  compact_append_raw(size, sizeof(long), (unsigned char*)vals);
}
void CompactDataType::add_unsigned_long_array(int size, unsigned long* vals) {
  if(sizeof(unsigned long) == sizeof(int32)) compact_add_record(TYPE_INT32, size);
  else if(sizeof(unsigned long) == sizeof(int64)) compact_add_record(TYPE_INT64, size);
  else assert(0); // assuming long has either 32 or 64 bits
  compact_append_raw(size, sizeof(unsigned long), (unsigned char*)vals);
}
#ifdef HAVE_LONG_LONG_INT
void CompactDataType::add_long_long_array(int size, long long* vals) {
  assert(sizeof(long long) == sizeof(int64)); // assuming long long is 64 bit
  compact_add_record(TYPE_INT64, size);
  compact_append_raw(size, sizeof(long long), (unsigned char*)vals);
}
void CompactDataType::add_unsigned_long_long_array(int size, unsigned long long* vals) {
  assert(sizeof(unsigned long long) == sizeof(uint64)); // assuming long long is 64 bit
  compact_add_record(TYPE_UINT64, size);
  compact_append_raw(size, sizeof(unsigned long long), (unsigned char*)vals);
}
#endif
void CompactDataType::add_virtual_time_array(int size, VirtualTime* vals) {
  compact_add_record(TYPE_VIRTUAL_TIME, size);
  compact_append_raw(size, sizeof(int64), (unsigned char*)vals);
}
void CompactDataType::add_string(const char* val) {
  int size = strlen(val)+1;
  compact_add_record(TYPE_STRING, size);
  compact_append_raw(size, sizeof(char), (unsigned char*)val);
}

int CompactDataType::get_float(float* val, int c) {
  if(compact_rec_get_offset < compact_rec_add_offset) {
   if(compact_record[compact_rec_get_offset] == TYPE_FLOAT &&
     compact_record[compact_rec_get_offset+1] >= c) { // either everything or nothing
    compact_record[compact_rec_get_offset+1] -= c;
    if(compact_record[compact_rec_get_offset+1] == 0)
    compact_rec_get_offset += 2;
    compact_retrieve_raw(c, sizeof(float), (unsigned char*)val);
    return c;
   }
  }
  SSF_THROW("mismatch data type");
  return 0;
}
int CompactDataType::get_double(double* val, int c) {
  if(compact_rec_get_offset < compact_rec_add_offset) {
   if(compact_record[compact_rec_get_offset] == TYPE_DOUBLE &&
     compact_record[compact_rec_get_offset+1] >= c) {
    compact_record[compact_rec_get_offset+1] -= c;
    if(compact_record[compact_rec_get_offset+1] == 0)
    compact_rec_get_offset += 2;
    compact_retrieve_raw(c, sizeof(double), (unsigned char*)val);
    return c;
   }
  }
  SSF_THROW("mismatch data type");
  return 0;
}
int CompactDataType::get_char(char* val, int c) {
  if(compact_rec_get_offset < compact_rec_add_offset) {
   if(compact_record[compact_rec_get_offset] == TYPE_INT8 &&
     compact_record[compact_rec_get_offset+1] >= c) {
    compact_record[compact_rec_get_offset+1] -= c;
    if(compact_record[compact_rec_get_offset+1] == 0)
    compact_rec_get_offset += 2;
    compact_retrieve_raw(c, sizeof(char), (unsigned char*)val);
    return c;
   }
  }
  SSF_THROW("mismatch data type");
  return 0;
}
int CompactDataType::get_unsigned_char(unsigned char* val, int c) {
  if(compact_rec_get_offset < compact_rec_add_offset) {
   if(compact_record[compact_rec_get_offset] == TYPE_UINT8 &&
     compact_record[compact_rec_get_offset+1] >= c) {
    compact_record[compact_rec_get_offset+1] -= c;
    if(compact_record[compact_rec_get_offset+1] == 0)
    compact_rec_get_offset += 2;
    compact_retrieve_raw(c, sizeof(unsigned char), (unsigned char*)val);
    return c;
   }
  }
  SSF_THROW("mismatch data type");
  return 0;
}
int CompactDataType::get_short(short* val, int c) {
  if(compact_rec_get_offset < compact_rec_add_offset) {
   if(compact_record[compact_rec_get_offset] == TYPE_INT16 &&
     compact_record[compact_rec_get_offset+1] >= c) {
    compact_record[compact_rec_get_offset+1] -= c;
    if(compact_record[compact_rec_get_offset+1] == 0)
    compact_rec_get_offset += 2;
    compact_retrieve_raw(c, sizeof(short), (unsigned char*)val);
    return c;
   }
  }
  SSF_THROW("mismatch data type");
  return 0;
}
int CompactDataType::get_unsigned_short(unsigned short* val, int c) {
  if(compact_rec_get_offset < compact_rec_add_offset) {
   if(compact_record[compact_rec_get_offset] == TYPE_UINT16 &&
     compact_record[compact_rec_get_offset+1] >= c) {
    compact_record[compact_rec_get_offset+1] -= c;
    if(compact_record[compact_rec_get_offset+1] == 0)
    compact_rec_get_offset += 2;
    compact_retrieve_raw(c, sizeof(unsigned short), (unsigned char*)val);
    return c;
   }
  }
  SSF_THROW("mismatch data type");
  return 0;
}
int CompactDataType::get_int(int* val, int c) {
  if(compact_rec_get_offset < compact_rec_add_offset &&
    compact_record[compact_rec_get_offset+1] >= c) {
   // there's no conversion between signed and unsigned integer types;
   // this is OK, since it is a programming issue, not a portability
   // issue.
   if((compact_record[compact_rec_get_offset] == TYPE_INT32 &&
       sizeof(int32) == sizeof(int)) ||
     (compact_record[compact_rec_get_offset] == TYPE_INT64 &&
       sizeof(int64) == sizeof(int))) {
    compact_record[compact_rec_get_offset+1] -= c;
    if(compact_record[compact_rec_get_offset+1] == 0)
    compact_rec_get_offset += 2;
    compact_retrieve_raw(c, sizeof(int), (unsigned char*)val);
    return c;
   } else if(compact_record[compact_rec_get_offset] == TYPE_INT32) {
    // we have a larger integer size than in the storage, which is OK
    compact_record[compact_rec_get_offset+1] -= c;
    if(compact_record[compact_rec_get_offset+1] == 0)
    compact_rec_get_offset += 2;
    for(int i=0; i<c; i++) {
     int32 tmp;
     compact_retrieve_raw(1, sizeof(int32), (unsigned char*)&tmp);
     val[i] = (int)tmp; // upward conversion here
    }
    return c;
   } else if(compact_record[compact_rec_get_offset] == TYPE_INT64) {
    // we have a smaller integer size than in the storage, which may become problematic
    compact_record[compact_rec_get_offset+1] -= c;
    if(compact_record[compact_rec_get_offset+1] == 0)
    compact_rec_get_offset += 2;
    for(int i=0; i<c; i++) {
     int64 tmp;
     compact_retrieve_raw(1, sizeof(int64), (unsigned char*)&tmp);
     val[i] = (int)tmp; // downward conversion here
     if((int64)val[i] != tmp)
       SSF_THROW("type conversion causing data loss");
    }
    return c;
   }
  }
  SSF_THROW("mismatch data type");
  return 0;
}
int CompactDataType::get_unsigned_int(unsigned int* val, int c) {
  if(compact_rec_get_offset < compact_rec_add_offset &&
    compact_record[compact_rec_get_offset+1] >= c) {
   // there's no conversion between signed and unsigned integer types;
   // this is OK, since it is a programming issue, not a portability
   // issue.
   if((compact_record[compact_rec_get_offset] == TYPE_UINT32 &&
       sizeof(uint32) == sizeof(unsigned int)) ||
     (compact_record[compact_rec_get_offset] == TYPE_UINT64 &&
       sizeof(uint64) == sizeof(unsigned int))) {
    compact_record[compact_rec_get_offset+1] -= c;
    if(compact_record[compact_rec_get_offset+1] == 0)
    compact_rec_get_offset += 2;
    compact_retrieve_raw(c, sizeof(unsigned int), (unsigned char*)val);
    return c;
   } else if(compact_record[compact_rec_get_offset] == TYPE_UINT32) {
    // we have a larger integer size than in the storage, which is OK
    compact_record[compact_rec_get_offset+1] -= c;
    if(compact_record[compact_rec_get_offset+1] == 0)
    compact_rec_get_offset += 2;
    for(int i=0; i<c; i++) {
     uint32 tmp;
     compact_retrieve_raw(1, sizeof(uint32), (unsigned char*)&tmp);
     val[i] = (unsigned int)tmp; // upward conversion here
    }
    return c;
   } else if(compact_record[compact_rec_get_offset] == TYPE_UINT64) {
    // we have a smaller integer size than in the storage, which may become problematic
    compact_record[compact_rec_get_offset+1] -= c;
    if(compact_record[compact_rec_get_offset+1] == 0)
    compact_rec_get_offset += 2;
    for(int i=0; i<c; i++) {
     uint64 tmp;
     compact_retrieve_raw(1, sizeof(uint64), (unsigned char*)&tmp);
     val[i] = (unsigned int)tmp; // downward conversion here
     if((uint64)val[i] != tmp)
       SSF_THROW("type conversion causing data loss");
    }
    return c;
   }
  }
  SSF_THROW("mismatch data type");
  return 0;
}
int CompactDataType::get_long(long* val, int c) {
  if(compact_rec_get_offset < compact_rec_add_offset &&
     compact_record[compact_rec_get_offset+1] >= c) {
    // there's no conversion between signed and unsigned integer
    // types; this is OK, since it is a programming issue, not a
    // portability issue.
    if((compact_record[compact_rec_get_offset] == TYPE_INT32 &&
	sizeof(int32) == sizeof(long)) ||
       (compact_record[compact_rec_get_offset] == TYPE_INT64 &&
	sizeof(int64) == sizeof(long))) {
      compact_record[compact_rec_get_offset+1] -= c;
      if(compact_record[compact_rec_get_offset+1] == 0)
	compact_rec_get_offset += 2;
      compact_retrieve_raw(c, sizeof(long), (unsigned char*)val);
      return c;
    } else if(compact_record[compact_rec_get_offset] == TYPE_INT32) {
      // we have a larger integer size than in the storage, which is OK
      compact_record[compact_rec_get_offset+1] -= c;
      if(compact_record[compact_rec_get_offset+1] == 0)
	compact_rec_get_offset += 2;
      for(int i=0; i<c; i++) {
	int32 tmp;
	compact_retrieve_raw(1, sizeof(int32), (unsigned char*)&tmp);
	val[i] = (long)tmp; // upward conversion here
      }
      return c;
    } else if(compact_record[compact_rec_get_offset] == TYPE_INT64) {
      // we have a smaller integer size than in the storage, which may become problematic
      compact_record[compact_rec_get_offset+1] -= c;
      if(compact_record[compact_rec_get_offset+1] == 0)
	compact_rec_get_offset += 2;
      for(int i=0; i<c; i++) {
	int64 tmp;
	compact_retrieve_raw(1, sizeof(int64), (unsigned char*)&tmp);
	val[i] = (long)tmp; // downward conversion here
	if((int64)val[i] != tmp)
	  SSF_THROW("type conversion causing data loss");
      }
      return c;
    }
  }
  SSF_THROW("mismatch data type");
  return 0;
}
int CompactDataType::get_unsigned_long(unsigned long* val, int c) {
  if(compact_rec_get_offset < compact_rec_add_offset &&
    compact_record[compact_rec_get_offset+1] >= c) {
    // there's no conversion between signed and unsigned integer
    // types; this is OK, since it is a programming issue, not a
    // portability issue.
    if((compact_record[compact_rec_get_offset] == TYPE_UINT32 &&
	sizeof(uint32) == sizeof(unsigned long)) ||
       (compact_record[compact_rec_get_offset] == TYPE_UINT64 &&
	sizeof(uint64) == sizeof(unsigned long))) {
      compact_record[compact_rec_get_offset+1] -= c;
      if(compact_record[compact_rec_get_offset+1] == 0)
	compact_rec_get_offset += 2;
      compact_retrieve_raw(c, sizeof(unsigned long), (unsigned char*)val);
      return c;
    } else if(compact_record[compact_rec_get_offset] == TYPE_UINT32) {
      // we have a larger integer size than in the storage, which is OK
      compact_record[compact_rec_get_offset+1] -= c;
      if(compact_record[compact_rec_get_offset+1] == 0)
	compact_rec_get_offset += 2;
      for(int i=0; i<c; i++) {
	uint32 tmp;
	compact_retrieve_raw(1, sizeof(uint32), (unsigned char*)&tmp);
	val[i] = (unsigned long)tmp; // upward conversion here
      }
      return c;
    } else if(compact_record[compact_rec_get_offset] == TYPE_UINT64) {
      // we have a smaller integer size than in the storage, which may become problematic
      compact_record[compact_rec_get_offset+1] -= c;
      if(compact_record[compact_rec_get_offset+1] == 0)
	compact_rec_get_offset += 2;
      for(int i=0; i<c; i++) {
	uint64 tmp;
	compact_retrieve_raw(1, sizeof(uint64), (unsigned char*)&tmp);
	val[i] = (unsigned long)tmp; // downward conversion here
	if((uint64)val[i] != tmp)
	  SSF_THROW("type conversion causing data loss");
      }
      return c;
    }
  }
  SSF_THROW("mismatch data type");
  return 0;
}
#ifdef HAVE_LONG_LONG_INT
int CompactDataType::get_long_long(long long* val, int c) {
  if(compact_rec_get_offset < compact_rec_add_offset) {
   if(compact_record[compact_rec_get_offset] == TYPE_INT64 &&
     compact_record[compact_rec_get_offset+1] >= c) {
    compact_record[compact_rec_get_offset+1] -= c;
    if(compact_record[compact_rec_get_offset+1] == 0)
    compact_rec_get_offset += 2;
    compact_retrieve_raw(c, sizeof(long long), (unsigned char*)val);
    return c;
   }
  }
  SSF_THROW("mismatch data type");
  return 0;
}
int CompactDataType::get_unsigned_long_long(unsigned long long* val, int c) {
  if(compact_rec_get_offset < compact_rec_add_offset) {
   if(compact_record[compact_rec_get_offset] == TYPE_UINT64 &&
     compact_record[compact_rec_get_offset+1] >= c) {
    compact_record[compact_rec_get_offset+1] -= c;
    if(compact_record[compact_rec_get_offset+1] == 0)
    compact_rec_get_offset += 2;
    compact_retrieve_raw(c, sizeof(unsigned long long), (unsigned char*)val);
    return c;
   }
  }
  SSF_THROW("mismatch data type");
  return 0;
}
#endif
int CompactDataType::get_virtual_time(VirtualTime* val, int c) {
  if(compact_rec_get_offset < compact_rec_add_offset) {
    if(compact_record[compact_rec_get_offset] == TYPE_VIRTUAL_TIME &&
       compact_record[compact_rec_get_offset+1] >= c) {
      compact_record[compact_rec_get_offset+1] -= c;
      if(compact_record[compact_rec_get_offset+1] == 0)
	compact_rec_get_offset += 2;
      compact_retrieve_raw(c, sizeof(VirtualTime), (unsigned char*)val);
      return c;
    }
  }
  SSF_THROW("mismatch data type");
  return 0;
}
char* CompactDataType::get_string() {
  if(compact_rec_get_offset < compact_rec_add_offset) {
    if(compact_record[compact_rec_get_offset] == TYPE_STRING) {
      int len = compact_record[compact_rec_get_offset+1];
      assert(len > 0);
      char* strval = new char[len]; assert(strval);
      compact_rec_get_offset += 2;
      compact_retrieve_raw(len, sizeof(char), (unsigned char*)strval);
      assert(strval[len-1] == 0);
      return strval;
    }
  }
  SSF_THROW("mismatch data type");
  return 0;
}

void CompactDataType::serialize(uint8 data, char* buf, int bufsiz, int* pos) 
{
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("serialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(uint8));
  if(*pos+(int)sizeof(uint8) > bufsiz) SSF_THROW("serialize buffer overflow");
  *(uint8*)(buf+*pos) = data; *pos += sizeof(uint8);
}
void CompactDataType::serialize(uint16 data, char* buf, int bufsiz, int* pos) 
{
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("serialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(uint16));
  if(*pos+(int)sizeof(uint16) > bufsiz) SSF_THROW("serialize buffer overflow");
  *(uint16*)(buf+*pos) = SSF_BYTE_ORDER_16(data); *pos += sizeof(uint16);
}
void CompactDataType::serialize(uint32 data, char* buf, int bufsiz, int* pos) 
{
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("serialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(uint32));
  if(*pos+(int)sizeof(uint32) > bufsiz) SSF_THROW("serialize buffer overflow");
  *(uint32*)(buf+*pos) = SSF_BYTE_ORDER_32(data); *pos += sizeof(uint32);
}
void CompactDataType::serialize(uint64 data, char* buf, int bufsiz, int* pos) 
{
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("serialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(uint64));
  if(*pos+(int)sizeof(uint64) > bufsiz) SSF_THROW("serialize buffer overflow");
  *(uint64*)(buf+*pos) = SSF_BYTE_ORDER_64(data); *pos += sizeof(uint64);
}
void CompactDataType::serialize(int8 data, char* buf, int bufsiz, int* pos) 
{
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("serialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(int8));
  if(*pos+(int)sizeof(int8) > bufsiz) SSF_THROW("serialize buffer overflow");
  *(int8*)(buf+*pos) = data; *pos += sizeof(int8);
}
void CompactDataType::serialize(int16 data, char* buf, int bufsiz, int* pos) 
{
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("serialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(int16));
  if(*pos+(int)sizeof(int16) > bufsiz) SSF_THROW("serialize buffer overflow");
  *(int16*)(buf+*pos) = SSF_BYTE_ORDER_16(data); *pos += sizeof(int16);
}
void CompactDataType::serialize(int32 data, char* buf, int bufsiz, int* pos) 
{
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("serialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(int32));
  if(*pos+(int)sizeof(int32) > bufsiz) SSF_THROW("serialize buffer overflow");
  *(int32*)(buf+*pos) = SSF_BYTE_ORDER_32(data); *pos += sizeof(int32);
}
void CompactDataType::serialize(int64 data, char* buf, int bufsiz, int* pos) 
{
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("serialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(int64));
  if(*pos+(int)sizeof(int64) > bufsiz) SSF_THROW("serialize buffer overflow");
  *(int64*)(buf+*pos) = SSF_BYTE_ORDER_64(data); *pos += sizeof(int64);
}
void CompactDataType::serialize(float data, char* buf, int bufsiz, int* pos) {
  // assuming IEEE 754 compliant; check should be done at configuration/compilation time
  assert(sizeof(int32) == sizeof(float)); 
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("serialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(int32));
  if(*pos+(int)sizeof(int32) > bufsiz) SSF_THROW("serialize buffer overflow");
  union { float fdata; int32 idata; } tmp;
  tmp.fdata = data; 
  *(int32*)(buf+*pos) = SSF_BYTE_ORDER_32(tmp.idata); *pos += sizeof(int32);
}
void CompactDataType::serialize(double data, char* buf, int bufsiz, int* pos) {
  // assuming IEEE 754 compliant; check should be done at configuration/compilation time
  assert(sizeof(uint64) == sizeof(double)); 
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("serialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(int64));
  if(*pos+(int)sizeof(int64) > bufsiz) SSF_THROW("serialize buffer overflow");
  union { double ddata; int64 idata; } tmp;
  tmp.ddata = data; 
  *(int64*)(buf+*pos) = SSF_BYTE_ORDER_64(tmp.idata); *pos += sizeof(int64);
}

void CompactDataType::deserialize(uint8& data, char* buf, int bufsiz, int* pos) {
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("deserialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(uint8));
  if(*pos+(int)sizeof(uint8) > bufsiz) SSF_THROW("deserialize buffer overflow");
  data = *(uint8*)(buf+*pos); pos += sizeof(uint8);
}
void CompactDataType::deserialize(uint16& data, char* buf, int bufsiz, int* pos) {
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("deserialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(uint16));
  if(*pos+(int)sizeof(uint16) > bufsiz) SSF_THROW("deserialize buffer overflow");
  data = SSF_BYTE_ORDER_16(*(uint16*)(buf+*pos)); *pos += sizeof(uint16);
}
void CompactDataType::deserialize(uint32& data, char* buf, int bufsiz, int* pos) {
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("deserialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(uint32));
  if(*pos+(int)sizeof(uint32) > bufsiz) SSF_THROW("deserialize buffer overflow");
  data = SSF_BYTE_ORDER_32(*(uint32*)(buf+*pos)); *pos += sizeof(uint32);
}
void CompactDataType::deserialize(uint64& data, char* buf, int bufsiz, int* pos) {
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("deserialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(uint64));
  if(*pos+(int)sizeof(uint64) > bufsiz) SSF_THROW("deserialize buffer overflow");
  data = SSF_BYTE_ORDER_64(*(uint64*)(buf+*pos)); *pos += sizeof(uint64);
}
void CompactDataType::deserialize(int8& data, char* buf, int bufsiz, int* pos) {
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("deserialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(int8));
  if(*pos+(int)sizeof(int8) > bufsiz) SSF_THROW("deserialize buffer overflow");
  data = *(int8*)(buf+*pos); pos += sizeof(int8);
}
void CompactDataType::deserialize(int16& data, char* buf, int bufsiz, int* pos) {
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("deserialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(int16));
  if(*pos+(int)sizeof(int16) > bufsiz) SSF_THROW("deserialize buffer overflow");
  data = SSF_BYTE_ORDER_16(*(int16*)(buf+*pos)); *pos += sizeof(int16);
}
void CompactDataType::deserialize(int32& data, char* buf, int bufsiz, int* pos) {
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("deserialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(int32));
  if(*pos+(int)sizeof(int32) > bufsiz) SSF_THROW("deserialize buffer overflow");
  data = SSF_BYTE_ORDER_32(*(int32*)(buf+*pos)); *pos += sizeof(int32);
}
void CompactDataType::deserialize(int64& data, char* buf, int bufsiz, int* pos) {
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("deserialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(int64));
  if(*pos+(int)sizeof(uint64) > bufsiz) SSF_THROW("deserialize buffer overflow");
  data = SSF_BYTE_ORDER_64(*(int64*)(buf+*pos)); *pos += sizeof(int64);
}
void CompactDataType::deserialize(float& data, char* buf, int bufsiz, int* pos) {
  // assuming IEEE 754 compliant; check should be done at configuration/compilation time
  assert(sizeof(int32) == sizeof(float));
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("deserialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(float));
  if(*pos+(int)sizeof(int32) > bufsiz) SSF_THROW("deserialize buffer overflow");
  int32 tmp = SSF_BYTE_ORDER_32(*(int32*)(buf+*pos)); 
  *pos += sizeof(int32);
  memcpy(&data, &tmp, sizeof(int32));
}
void CompactDataType::deserialize(double& data, char* buf, int bufsiz, int* pos) {
  // assuming IEEE 754 compliant; check should be done at configuration/compilation time
  assert(sizeof(int64) == sizeof(double));
  int x = 0; if(!pos) pos = &x;
  if(!buf) SSF_THROW("deserialize empty buffer");
  *pos = compact_make_alignment(*pos, sizeof(double));
  if(*pos+(int)sizeof(int64) > bufsiz) SSF_THROW("deserialize buffer overflow");
  int64 tmp = SSF_BYTE_ORDER_64(*(int64*)(buf+*pos)); 
  *pos += sizeof(int64);
  memcpy(&data, &tmp, sizeof(int64));
}

void CompactDataType::compact_reset() {
  if(compact_record) {delete[] compact_record; compact_record = 0;}
  if(compact_data) {delete[] compact_data; compact_data = 0;}
  compact_rec_capacity = compact_rec_add_offset = compact_rec_get_offset = 0;
  compact_capacity = compact_add_offset = compact_get_offset = 0;
}

void CompactDataType::compact_add_record(int type, int size) {
  if(type != TYPE_STRING && // each string must maintain its own record
     compact_rec_add_offset > 0 && compact_record[compact_rec_add_offset-2] == type) {
    compact_record[compact_rec_add_offset-1] += size;
  } else {
    if(compact_rec_add_offset >= compact_rec_capacity) {
      int newsz = compact_rec_capacity ? 2*compact_rec_capacity : 16;
      int* newbuf = new int[newsz]; assert(newbuf);
      if(compact_record) {
	memcpy(newbuf, compact_record, compact_rec_add_offset*sizeof(int));
	delete[] compact_record;
      }
      compact_record = newbuf;
      compact_rec_capacity = newsz;
    }
    compact_record[compact_rec_add_offset] = type;
    compact_record[compact_rec_add_offset+1] = size;
    compact_rec_add_offset += 2;
  }
}

void CompactDataType::compact_append_raw(int c, int size, unsigned char* buf) {
  compact_add_offset = compact_make_alignment(compact_add_offset, size);
  int newsz = compact_capacity;
  while(size*c+compact_add_offset > newsz) {
    newsz = newsz ? 2*newsz : 32;
    if(newsz < size*c+compact_add_offset) continue;
    unsigned char* newbuf = new unsigned char[newsz];
    assert(newbuf);
    if(compact_data) {
      memcpy(newbuf, compact_data, compact_capacity*sizeof(unsigned char));
      delete[] compact_data;
    }
    compact_data = newbuf;
    compact_capacity = newsz;
    break;
  }
#ifdef SSF_NO_ENDIAN_FORMATTING
  memcpy(&compact_data[compact_add_offset], buf, size*c);
#else
  if(size == sizeof(uint8)) {
    memcpy(&compact_data[compact_add_offset], buf, size*c);
  } else {
    for(int i=0; i<c; i+=size) {
      if(size == sizeof(uint16))
	*(uint16*)&compact_data[compact_add_offset+i] =
	  SSF_BYTE_ORDER_16(*(uint16*)&buf[i]);
      else if(size == sizeof(uint32))
	*(uint32*)&compact_data[compact_add_offset+i] =
	  SSF_BYTE_ORDER_32(*(uint32*)&buf[i]);
      else if(size == sizeof(uint64))
	*(uint64*)&compact_data[compact_add_offset+i] =
	  SSF_BYTE_ORDER_64(*(uint64*)&buf[i]);
      else assert(0);
    }
  }
#endif /*SSF_NO_ENDIAN_FORMATTING*/
  compact_add_offset += size*c;
}

void CompactDataType::compact_retrieve_raw(int c, int size, unsigned char* buf) {
  compact_get_offset = compact_make_alignment(compact_get_offset, size);
  assert(size*c+compact_get_offset <= compact_capacity);
#ifdef SSF_NO_ENDIAN_FORMATTING
  memcpy(buf, &compact_data[compact_get_offset], size*c);
#else
  if(size == sizeof(uint8)) {
    memcpy(buf, &compact_data[compact_get_offset], size*c);
  } else {
    for(int i=0; i<c; i+=size) {
      if(size == sizeof(uint16))
	*(uint16*)&buf[i] =
	  SSF_BYTE_ORDER_16(*(uint16*)&compact_data[compact_get_offset+i]);
      else if(size == sizeof(uint32))
	*(uint32*)&buf[i] =
	  SSF_BYTE_ORDER_32(*(uint32*)&compact_data[compact_get_offset+i]);
      else if(size == sizeof(uint64))
	*(uint64*)&buf[i] =
	  SSF_BYTE_ORDER_64(*(uint64*)&compact_data[compact_get_offset+i]);
      else assert(0);
    }
  }
#endif /*SSF_NO_ENDIAN_FORMATTING*/
  compact_get_offset += size*c;
}

#ifdef HAVE_MPI_H
int CompactDataType::pack(char* buffer, int bufsiz) 
{
  int pos = 0;
  ssf_mpi_pack(&compact_rec_add_offset, 1, MPI_INT, buffer, bufsiz, &pos, MPI_COMM_WORLD);
  if(compact_rec_add_offset > 0) {
    ssf_mpi_pack(compact_record, compact_rec_add_offset, MPI_INT, 
		 buffer, bufsiz, &pos, MPI_COMM_WORLD);
    ssf_mpi_pack(&compact_add_offset, 1, MPI_INT, 
		 buffer, bufsiz, &pos, MPI_COMM_WORLD);
    ssf_mpi_pack(compact_data, compact_add_offset, MPI_UNSIGNED_CHAR, 
		 buffer, bufsiz, &pos, MPI_COMM_WORLD);
  }
  return pos;
}

int CompactDataType::pack_and_delete(char* buffer, int bufsiz) 
{
  int ret = pack(buffer, bufsiz);
  delete this;
  return ret;
}

void CompactDataType::unpack(char* buffer, int bufsiz) 
{
  int pos = 0;
  compact_reset();
  ssf_mpi_unpack(buffer, bufsiz, &pos, &compact_rec_add_offset, 
		 1, MPI_INT, MPI_COMM_WORLD);
  if(compact_rec_add_offset > 0) {
    compact_rec_capacity = compact_rec_add_offset;
    compact_record = new int[compact_rec_capacity]; assert(compact_record);
    ssf_mpi_unpack(buffer, bufsiz, &pos, compact_record, 
		   compact_rec_add_offset, MPI_INT, MPI_COMM_WORLD);
    ssf_mpi_unpack(buffer, bufsiz, &pos, &compact_add_offset, 
		   1, MPI_INT, MPI_COMM_WORLD);
    compact_capacity = compact_add_offset;
    compact_data = new unsigned char[compact_capacity]; assert(compact_data);
    ssf_mpi_unpack(buffer, bufsiz, &pos, compact_data, 
		   compact_add_offset, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
  }
  if(pos != bufsiz) SSF_THROW("unmatched compact data type");
}
#endif /*HAVE_MPI_H*/

}; /*namespace minissf*/

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
