#include "hash.h"

uint64_t inline hash (const char * key)
{
    uint64_t h = 525201411107845655ull;
    for (;*key;++key) {
        printf("iteration\n");
        h ^= *key;
        h *= 0x5bd1e9955bd1e995;
        h ^= h >> 47;
    }
    return h;
}
