#ifndef ERTURK_TYPEBUFFER_MEMORY_H
#define ERTURK_TYPEBUFFER_MEMORY_H

#include <cstddef>
#include <cstdint>

namespace erturk::type_buffer_memory
{

// Emplace T copy constructor into address
template <class T, class T_Iterator>
inline constexpr T_Iterator emplace_type_buffers(const T_Iterator type_buffer_dest_begin,
                                                 const T_Iterator type_buffer_dest_end, const T& value)
{
    if (type_buffer_dest_begin == nullptr || type_buffer_dest_end == nullptr)
    {
        return nullptr;
    }
    T_Iterator curr_type_buffer = type_buffer_dest_begin;
    try
    {
        while (curr_type_buffer != type_buffer_dest_end)
        {
            // Unlike copy(*dest_first = *src_first), curr_type_buffer cannot be dereferenced since it's type
            // buffer(not instantiated)
            ::new (static_cast<void*>(&*curr_type_buffer)) T{value};  // Emplace T copy constructor into address

            curr_type_buffer++;  // increase to next T memory layout
        }
        return curr_type_buffer;
    }
    catch (...)
    {
        while (curr_type_buffer != type_buffer_dest_begin)
        {
            curr_type_buffer--;      // rollback constructed type buffer to begin
            curr_type_buffer->~T();  // call already constructed type buffer
        }
        throw;
    }
}

// Emplace T copy constructor into address
template <class T, class T_Iterator, class Size>
inline constexpr T_Iterator emplace_type_buffers_n(const T_Iterator type_buffer_dest_begin, const Size size,
                                                   const T& value)
{
    if (type_buffer_dest_begin == nullptr || size <= 0)
    {
        return nullptr;
    }
    T_Iterator curr_type_buffer = type_buffer_dest_begin;
    try
    {
        Size size_ = size;
        while (size_ > 0)
        {
            // Unlike copy(*dest_first = *src_first), curr_type_buffer cannot be dereferenced since it's type
            // buffer(not instantiated)
            ::new (static_cast<void*>(&*curr_type_buffer)) T{value};  // Emplace T copy constructor into address

            curr_type_buffer++;  // increase to next T memory layout
            size_++;
        }
        return curr_type_buffer;
    }
    catch (...)
    {
        while (curr_type_buffer != type_buffer_dest_begin)
        {
            curr_type_buffer--;      // rollback constructed type buffer to begin
            curr_type_buffer->~T();  // call already constructed type buffer
        }
        throw;
    }
}

// Emplace T any constructor into address
template <class T, class T_Iterator, class... Args>
inline constexpr T_Iterator emplace_type_buffers(const T_Iterator type_buffer_dest_begin,
                                                 const T_Iterator type_buffer_dest_end, Args&&... args)
{
    if (type_buffer_dest_begin == nullptr || type_buffer_dest_end == nullptr)
    {
        return nullptr;
    }
    T_Iterator curr_type_buffer = type_buffer_dest_begin;
    try
    {
        while (curr_type_buffer != type_buffer_dest_end)
        {
            // Unlike copy(*dest_first = *src_first), curr_type_buffer cannot be dereferenced since it's type
            // buffer(not instantiated)
            ::new (static_cast<void*>(&*curr_type_buffer))
                T{std::forward<Args>(args)...};  // Emplace T any constructor into address

            curr_type_buffer++;  // increase to next T memory layout
        }
        return curr_type_buffer;
    }
    catch (...)
    {
        while (curr_type_buffer != type_buffer_dest_begin)
        {
            curr_type_buffer--;      // rollback constructed type buffer to begin
            curr_type_buffer->~T();  // call already constructed type buffer
        }
        throw;
    }
}

// Emplace T constructor into address
template <class T, class T_Iterator, class Size, class... Args>
inline constexpr T_Iterator emplace_type_buffers_n(const T_Iterator type_buffer_dest_begin, const Size size,
                                                   Args&&... args)
{
    if (type_buffer_dest_begin == nullptr || size <= 0)
    {
        return nullptr;
    }
    T_Iterator curr_type_buffer = type_buffer_dest_begin;
    try
    {
        Size size_ = size;
        while (size_ > 0)
        {
            // Unlike copy(*dest_first = *src_first), curr_type_buffer cannot be dereferenced since it's type
            // buffer(not instantiated)
            ::new (static_cast<void*>(&*curr_type_buffer)) T{std::forward<Args>(args)...};  // Emplace T into address

            curr_type_buffer++;  // increase to next T memory layout
            size_++;
        }
        return curr_type_buffer;
    }
    catch (...)
    {
        while (curr_type_buffer != type_buffer_dest_begin)
        {
            curr_type_buffer--;      // rollback constructed type buffer to begin
            curr_type_buffer->~T();  // call already constructed type buffer
        }
        throw;
    }
}

// Emplace T copy constructor into address
template <class T, class T_Iterator>
inline constexpr T_Iterator emplace_type_buffers_copy(T_Iterator emplaced_src_begin, const T_Iterator emplaced_src_end,
                                                      const T_Iterator type_buffer_dest_begin)
{
    if (emplaced_src_begin == nullptr || emplaced_src_end == nullptr || type_buffer_dest_begin == nullptr)
    {
        return nullptr;
    }
    T_Iterator curr_type_buffer = type_buffer_dest_begin;
    try
    {
        while (emplaced_src_begin != emplaced_src_end)
        {
            // Unlike copy(*dest_first = *src_first), curr_type_buffer cannot be dereferenced since it's type
            // buffer(not instantiated)
            ::new (static_cast<void*>(&*curr_type_buffer))
                T{*emplaced_src_begin};  // Emplace T copy constructor into address

            emplaced_src_begin++;  // increase to next T memory layout
            curr_type_buffer++;    // increase to next T memory layout
        }
        return curr_type_buffer;
    }
    catch (...)
    {
        while (curr_type_buffer != type_buffer_dest_begin)
        {
            curr_type_buffer--;      // rollback constructed type buffer to begin
            curr_type_buffer->~T();  // call already constructed type buffer
        }
        throw;
    }
}

// Emplace T with copy constructor from placed T, into address
template <class T, class T_Iterator, class Size>
inline constexpr T_Iterator emplace_type_buffers_copy_n(T_Iterator emplaced_src_begin, const Size size,
                                                        const T_Iterator type_buffer_dest_begin)
{
    if (emplaced_src_begin == nullptr || size <= 0 || type_buffer_dest_begin == nullptr)
    {
        return nullptr;
    }
    T_Iterator curr_type_buffer = type_buffer_dest_begin;
    try
    {
        Size size_ = size;
        while (size_ > 0)
        {
            // Unlike copy(*dest_first = *src_first), curr_type_buffer cannot be dereferenced since it's type
            // buffer(not instantiated)
            ::new (static_cast<void*>(&*curr_type_buffer))
                T{*emplaced_src_begin};  // Emplace T with copy constructor from placed T, into address

            emplaced_src_begin++;  // increase to next T memory layout
            curr_type_buffer++;    // increase to next T memory layout
            size_--;
        }
        return curr_type_buffer;
    }
    catch (...)
    {
        while (curr_type_buffer != type_buffer_dest_begin)
        {
            curr_type_buffer--;      // rollback constructed type buffer to begin
            curr_type_buffer->~T();  // call already constructed type buffer
        }
        throw;
    }
}

template <typename T, typename... Args>
inline constexpr void construct_at(T* addr, Args&&... args)
{
    if (addr != nullptr)
    {
        new (addr) T{std::forward<Args>(args)...};
    }
}

template <typename T>
inline constexpr void destruct_at(T* addr)
{
    if (addr != nullptr)
    {
        addr->~T();
    }
}

}  // namespace erturk::type_buffer_memory

#endif  // ERTURK_TYPEBUFFER_MEMORY_H
