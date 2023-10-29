#include <iostream>
#include <fstream>
#include <list>
#include <unistd.h>
#include "memory_alloc.h"


std::list<Allocation> allocatedChunks;
std::list<Allocation> freeChunks;

void* alloc(std::size_t chunk_size) {
    // initialize best fit chunk as nullptr and best fit diff as large val
    Allocation* bestFitChunk = nullptr;
    std::size_t bestFitDiff = std::numeric_limits<std::size_t>::max();

    // search for best fit chunk in free list
    for (auto i = freeChunks.begin(); i != freeChunks.end(); ++i) {
        if (i->size >= chunk_size) {
            // calculate the difference between the chunk size and the requested size
            std::size_t difference = i->size - chunk_size;

            // if the difference is smaller than the best fit so far, update bestFitChunk
            if (difference < bestFitDiff) {
                bestFitChunk = &(*i);
                bestFitDiff = difference;
            }
        }
    }

    if (bestFitChunk != nullptr) {
        // found a best fit chunk
        Allocation allocatedChunk = { chunk_size, bestFitChunk->space };
        
        // Add the allocated chunk to the allocated list
        allocatedChunks.push_back(allocatedChunk);

        // If there's some space left in the free chunk, split it
        if (bestFitDiff > 0) {
            bestFitChunk->size = bestFitDiff;
            bestFitChunk->space = static_cast<char*>(bestFitChunk->space) + chunk_size;
        } else {
            // Remove the chunk from the free list since it's fully used
            freeChunks.erase(std::find(freeChunks.begin(), freeChunks.end(), *bestFitChunk));
        }

        return allocatedChunk.space;
    }

    // if no suitable chunk found, request more memory from OS using sbrk
    // https://stackoverflow.com/questions/62678780/what-is-the-compliant-way-to-handle-an-sbrk-failure
    // error checking if comparison sourced from link above
    void* newSpace = sbrk(chunk_size);
    if (newSpace == (void*) -1) {
        // sbrk failed to allocate memory
        std::cout << "Allocation failed." << std::endl;
        return nullptr;
    }

    // Create an allocation for the new space and add it to the allocated list
    Allocation allocatedChunk = { chunk_size, newSpace };
    allocatedChunks.push_back(allocatedChunk);

    return allocatedChunk.space;
}
