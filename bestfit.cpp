#include <iostream>
#include <fstream>
#include <list>
#include <unistd.h>
#include "memory_alloc.h"

std::list<Allocation*> allocatedChunks;
std::list<Allocation*> freeChunks;


void* alloc(std::size_t chunk_size) {
    std::size_t bestFitSize = std::size_t(-1);
    Allocation* bestFitChunk = nullptr;

    // search for best-fit chunk
    for (auto i = allocatedChunks.begin(); i != allocatedChunks.end(); ++i) {
    if ((*i)->space != nullptr && (*i)->size >= chunk_size) {
        std::size_t currentChunkSize = (*i)->size;
        if (currentChunkSize < bestFitSize) {
            bestFitSize = currentChunkSize;
            bestFitChunk = *i;
        }
    }
}

    if (bestFitChunk) {
        
        // found a best fit chunk
        Allocation allocatedChunk = {chunk_size, bestFitChunk->space};
        
        // Add the allocated chunk to the allocated list
        allocatedChunks.push_back(&allocatedChunk);

        // if there's space left in free chunk, split it
        if (bestFitSize > 0) {
            bestFitChunk->size = bestFitSize;
            bestFitChunk->space = static_cast<char*>(bestFitChunk->space) + chunk_size;
        } 
        else {
            // remove chunk from free list since its fully used
            freeChunks.erase(std::find(freeChunks.begin(), freeChunks.end(), *bestFitChunk));
        }

        return allocatedChunk.space;
        
    } 
    else {
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
        allocatedChunks.push_back(&allocatedChunk);

        return allocatedChunk.space;
    }

    return bestFitChunk ? bestFitChunk->space : nullptr;
}

int main(int argc, char** argv) { 
    // check number of args
    if (argc != 2) {
        std::cout << "Incorrect number of args\n" << std::endl;
        return -1;
    }

    // open and check data file
    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cout << "Failed to open data file.\n" << std::endl;
        return -1;
    }

    // reading and running
    std::string line;
    while (std::getline(inputFile, line)) {
        std::size_t colonPos = line.find(':');
        // finding colon to check if we are alloc or dealloc
        if (colonPos != std::string::npos) {
            std::string temp_chunk_size = line.substr(colonPos + 1, 4);
            std::size_t chunk_size = std::stoul(temp_chunk_size);
            alloc(chunk_size);
        }
        else {
            Allocation* lastAllocatedChunk = allocatedChunks.back();
            allocatedChunks.pop_back();
            dealloc(lastAllocatedChunk->space);
        }
    }

    inputFile.close();
    std::cout << "\nAllocated Chunks:" << std::endl;
    for (Allocation* chunk : freeChunks) {
        std::cout << "   " << chunk->space << std::endl;
        std::cout << "\tSize:" << chunk->size << std::endl;
    }

    std::cout << "\nFree Chunks:" << std::endl;
    for (Allocation* chunk : freeChunks) {
        std::cout << "   " << chunk->space << std::endl;
        std::cout << "\tSize:" << chunk->size << std::endl;
    }
    return 1;
}