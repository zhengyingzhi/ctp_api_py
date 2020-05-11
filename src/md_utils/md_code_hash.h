#ifndef _MD_CODE_HASH_H_
#define _MD_CODE_HASH_H_

#include <stdbool.h>
#include <stdint.h>

#define MD_HASHID_INVALID           (uint64_t)-1
#define MD_HASHID_WILDCARD          0


/// return a hash value of the instrument id
bool md_code_get_hashid(uint64_t* phashid, const char* exchange, const char* instrument, uint32_t len);


#endif//_MD_CODE_HASH_H_
