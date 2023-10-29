#include <iostream>
#include <fstream>
#include <list>
#include <unistd.h>
#include "memory_alloc.h"

std::list<Allocation*> allocatedChunks;
std::list<Allocation*> freeChunks;

void* alloc(std::size_t chunk_size) {
    // search for chunk in the free list that can satisfy allocation request
    for (Allocation* i : freeChunks) {
        if (i->size >= chunk_size) {
            // found large enough chunk
            Allocation allocatedChunk = {chunk_size, i->space};
            
            // add allocated chunk to allocated list
            allocatedChunks.push_back(&allocatedChunk);

            // adjust free chunk's size and space
            i->size -= chunk_size;
            i->space = static_cast<char*>(i->space) + chunk_size;

            return allocatedChunk.space;
        }
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
    allocatedChunks.push_back(&allocatedChunk);

    return allocatedChunk.space;
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