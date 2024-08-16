//
// Created by yarlkot on 13.08.24.
//

#pragma once

#include "Parser.hpp"

class ArenaAllocator {
public:
    static ArenaAllocator &Instance() {
        static ArenaAllocator singleton = ArenaAllocator(1024 * 1024 * 4);
        return singleton;
    }


    template<typename T>
    inline T *alloc() {
        void *offset = m_offset;
        m_offset += sizeof(T);
        return static_cast<T *>(offset);
    }

    inline ArenaAllocator(const ArenaAllocator &a) = delete;

    inline ArenaAllocator operator=(const ArenaAllocator &a) = delete;


    inline ~ArenaAllocator() {
        free(m_buffer);

    }

private:
    size_t m_size;
    std::byte *m_buffer;
    std::byte *m_offset;

    inline explicit ArenaAllocator(size_t bytes) : m_size(bytes) {
        m_buffer = static_cast<std::byte *>(malloc(m_size));
        m_offset = m_buffer;
    }
};
