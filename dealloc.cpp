#include <iostream>
#include <fstream>
#include <list>
#include <unistd.h>
#include "memory_alloc.h"

void dealloc(void* chunk) {
    // search for chunk in allocated list
    for (auto i = allocatedChunks.begin(); i != allocatedChunks.end(); ++i) {
        if (i->space == chunk) {
            // found allocated chunk, remove from allocated list
            allocatedChunks.erase(i);

            // add deallocated chunk to free list
            freeChunks.push_back(*i);

            return;
        }
    }

    // if chunk was not found in allocated list, obviously error
    std::cerr << "Deallocation failed. The chunk was not previously allocated.\n" << std::endl;
}
