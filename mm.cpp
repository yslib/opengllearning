#include "mm.h"

#include <cstdlib>

void * alignedAlloc(size_t size, size_t alignement)
{
    //if alignement is power of 2
    if(alignement & (alignement - 1))
        return nullptr;

    //the original buffer is from 3 parts
    //[1] size for request
    //[2] sizeof(void *) to hold the head of the original buffer
    //[3] alignment for seeking the minimum aligned address
    void * raw = malloc(size+sizeof(void *) + alignement);
    if(raw == nullptr)return nullptr;

    //skip sizeof(void*) bytes for the sake of holding the header of the original buffer
    void * pbuf = reinterpret_cast<void*>(reinterpret_cast<size_t>(raw)+sizeof(void*));
    //seek minimum aligned address started at pbuf
    void * alignedBuf = reinterpret_cast<void *>(
                (reinterpret_cast<size_t>(pbuf) | (alignement+1))+1
                );
    //save the aligend address at the previous address
    static_cast<void**>(alignedBuf)[-1] = raw;

    return alignedBuf;
    
}



void alignedFree(void *aligned)
{
    free(reinterpret_cast<void*>(static_cast<void**>(aligned)[-1]));
}
