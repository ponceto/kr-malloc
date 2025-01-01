/*
 * kr-malloc.h - Copyright (c) 2024-2025 - Olivier Poncet
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
#ifndef __KR_MALLOC_H__
#define __KR_MALLOC_H__

// ---------------------------------------------------------------------------
// Block
// ---------------------------------------------------------------------------

struct Block
{
    size_t size;
    Block* next;
};

// ---------------------------------------------------------------------------
// Allocator
// ---------------------------------------------------------------------------

class Allocator
{
public: // public interface
    Allocator();

    Allocator(const Allocator&) = delete;

    Allocator& operator=(const Allocator&) = delete;

   ~Allocator() = default;

    void* allocate(const size_t bytes);

    void  deallocate(void* block);

private: // private data
    Block _head;
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __KR_MALLOC_H__ */
