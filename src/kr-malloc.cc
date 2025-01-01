/*
 * kr-malloc.cc - Copyright (c) 2024-2025 - Olivier Poncet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cstdarg>
#include <ctime>
#include <unistd.h>
#include <sys/mman.h>
#include "kr-malloc.h"

// ---------------------------------------------------------------------------
// <anonymous>::debug
// ---------------------------------------------------------------------------

namespace {

static void debug(const char* format, ...)
{
#ifndef NDEBUG
    FILE* stream(stderr);

    va_list arguments;
    va_start(arguments, format);
    static_cast<void>(::vfprintf(stream, format, arguments));
    static_cast<void>(::fputc('\n', stream));
    va_end(arguments);
#endif
}

}

// ---------------------------------------------------------------------------
// <anonymous>::AllocatorTraits
// ---------------------------------------------------------------------------

namespace {

struct AllocatorTraits
{
    static size_t get_units(const size_t bytes)
    {
        size_t units = 0;

        if(bytes != 0) {
            units = (((sizeof(Block) + bytes - 1) / sizeof(Block)) + 1);
        }
        return units;
    }

    static void* get_memory(const size_t bytes)
    {
        debug("allocating memory for %ld bytes", bytes);
        constexpr int prot   = (PROT_READ | PROT_WRITE);
        constexpr int flags  = (MAP_PRIVATE | MAP_ANONYMOUS);
        void*         memory = ::mmap(nullptr, bytes, prot, flags, -1, 0);

        if(memory != MAP_FAILED) {
            return memory;
        }
        return nullptr;
    }

    static Block* get_blocks(const size_t count)
    {
        debug("requesting a new block for %ld units", count);
        const size_t bytes  = ((count * sizeof(Block)) + 16383) & ~16383;
        const size_t units  = ((bytes / sizeof(Block)));
        void*        memory = get_memory(bytes);

        if(memory != nullptr) {
            Block* block = reinterpret_cast<Block*>(memory);
            block->size = units;
            block->next = nullptr;
            return block;
        }
        return nullptr;
    }
};

}

// ---------------------------------------------------------------------------
// Allocator
// ---------------------------------------------------------------------------

Allocator::Allocator()
    : _head{0, &_head}
{
}

void* Allocator::allocate(const size_t bytes)
{
    const size_t units = AllocatorTraits::get_units(bytes);

    auto init_block = [&](Block* block) -> void*
    {
        block->size = units;
        block->next = nullptr;
        return static_cast<void*>(block + 1);
    };

    debug("allocating %ld bytes (%ld units)", bytes, units);
    if(units != 0) {
        Block* head = &_head;
        Block* prev = &_head;
        do {
            Block* node = prev->next;
            /* no free block found that fits the requirements */ {
                if(node == head) {
                    debug("no free block found that fits %ld units", units);
                    node = AllocatorTraits::get_blocks(units);
                    if(node != nullptr) {
                        prev->next = node;
                        node->next = head;
                    }
                    else {
                        break;
                    }
                }
            }
            /* current block fits perfectly, use it */ {
                if(node->size == units) {
                    debug("block %p (%ld units) fits perfectly, use it", node, node->size);
                    prev->next = node->next;
                    return init_block(node);
                }
            }
            /* current block fits roughly, split it */ {
                if(node->size >= units) {
                    debug("block %p (%ld units) fits roughly, split it", node, node->size);
                    node->size -= units;
                    return init_block(node + node->size);
                }
            }
            /* current free block does not fit */ {
                debug("block %p (%ld units) does not fit", node, node->size);
            }
            /* iterate to the next free block */ {
                prev = node;
            }
        } while(true);
    }
    return nullptr;
}

void Allocator::deallocate(void* block)
{
    if(block != nullptr) {
        Block* blck = reinterpret_cast<Block*>(block) - 1;
        Block* head = &_head;
        Block* prev = &_head;
        debug("deallocating %p (%ld units)", blck, blck->size);
        do {
            Block* node = prev->next;
            debug("current block is %p, %ld units", node, node->size);
            /* end of list reached */ {
                if(node == head) {
                    debug("append the freed block to the end of the list");
                    prev->next = blck;
                    blck->next = head;
                    break;
                }
            }
            /* merging the current block with block */ {
                if((node + node->size) == blck) {
                    debug("merging the current block with freed block");
                    node->size += blck->size;
                    blck->size  = 0;
                    blck->next  = nullptr;
                    break;
                }
            }
            /* merging the block with current block */ {
                if((blck + blck->size) == node) {
                    debug("merging the freed block with current block");
                    blck->size += node->size;
                    blck->next  = node->next;
                    prev->next  = blck;
                    break;
                }
            }
            /* iterate to next block */ {
                prev = node;
            }
        } while(true);
        /* dump the free list */ {
            Block* node = head;
            while((node = node->next) != head) {
                debug("=> free block %p (%ld units)", node, node->size);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// globals
// ---------------------------------------------------------------------------

Allocator allocator;

// ---------------------------------------------------------------------------
// kr_malloc
// ---------------------------------------------------------------------------

void* kr_malloc(size_t bytes)
{
    return allocator.allocate(bytes);
}

// ---------------------------------------------------------------------------
// kr_free
// ---------------------------------------------------------------------------

void kr_free(void* block)
{
    return allocator.deallocate(block);
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    void* pointers[8];

    auto setup = [&]() -> void
    {
        ::srand(::time(nullptr));
        for(auto& pointer : pointers) {
            pointer = nullptr;
        }
    };

    auto allocate = [&]() -> void
    {
        for(auto& pointer : pointers) {
            debug("-------- kr_malloc() --------");
            pointer = ::kr_malloc(8 + (::rand() % 16384));
            debug("");
        }
    };

    auto deallocate = [&]() -> void
    {
        for(auto& pointer : pointers) {
            debug("--------- kr_free() ---------");
            pointer = (::kr_free(pointer), nullptr);
            debug("");
        }
    };

    auto run = [&]() -> int
    {
        setup();
        allocate();
        deallocate();

        return EXIT_SUCCESS;
    };

    return run();
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
