#include <iostream>
#include <fstream>
#include <list>
#include <unistd.h>
#include "memory_alloc.h"


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

        // if there's space left in free chunk, split it
        if (bestFitDiff > 0) {
            bestFitChunk->size = bestFitDiff;
            bestFitChunk->space = static_cast<char*>(bestFitChunk->space) + chunk_size;
        } else {
            // remove chunk from free list since its fully used
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

    // create allocation for new space and add to allocated list
    Allocation allocatedChunk = {chunk_size, newSpace};
    allocatedChunks.push_back(allocatedChunk);

    // saftey return
    return allocatedChunk.space;
}

int main(int argc, char** argv) { 
    // check number of args
    if (argc != 2) {
        std::cout << "Incorrect number of args\n" << std::endl;
        return -1;
    }

    // open and check data file
    std::ifstream inputFile(argv[2]);
    if (!inputFile.is_open()) {
        std::cout << "Failed to open data file.\n" << std::endl;
        return -1;
    }

    // reading and running
    std::string line;
    while (std::getline(inputFile, line)) {
        size_t colonPos = line.find(':');
        // finding colon to check if we are alloc or dealloc
        if (colonPos != std::string::npos) {
            std::string temp_chunk_size = line.substr(colonPos + 1, 3);
            size_t chunk_size = std::stoul(temp_chunk_size);
            alloc(chunk_size);
        }
        else {
            Allocation lastAllocatedChunk = allocatedChunks.back();
            allocatedChunks.pop_back();
            dealloc(lastAllocatedChunk.space);
        }
    }

    inputFile.close();
    return 1;
}