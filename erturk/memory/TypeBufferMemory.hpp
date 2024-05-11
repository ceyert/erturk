#ifndef ERTURK_TYPEBUFFER_MEMORY_H
#define ERTURK_TYPEBUFFER_MEMORY_H

#include <cstddef>
#include <cstdint>

namespace erturk::type_buffer_memory
{

template <class T, class T_Iter>
inline constexpr T_Iter emplace_type_buffers_copy(T_Iter emplaced_src_begin, const T_Iter emplaced_src_end,
                                                  T_Iter type_buffer_dest_begin)
{
    T_Iter curr_type_buffer = type_buffer_dest_begin;
    try
    {
        while (emplaced_src_begin != emplaced_src_end)
        {
            // Unlike copy(*dest_first = *src_first), curr_type_buffer can not be dereferenced since it's type
            // buffer(char buffer)
            ::new (static_cast<void*>(&*curr_type_buffer))
                T{*emplaced_src_begin};  // Emplace T copy constructor into address

            ++emplaced_src_begin;  // increase to next T memory layout
            ++curr_type_buffer;    // increase to next T memory layout
        }
        return curr_type_buffer;
    }
    catch (...)
    {
        while (curr_type_buffer != type_buffer_dest_begin)
        {
            --curr_type_buffer;      // rollback constructed type buffer to begin
            curr_type_buffer->~T();  // call already constructed type buffer
        }
        throw;
    }
}

template <class T, class T_Iter, class Size>
inline constexpr T_Iter emplace_type_buffers_copy_n(T_Iter emplaced_src_begin, Size size, T_Iter type_buffer_dest_begin)
{
    T_Iter curr_type_buffer = type_buffer_dest_begin;
    try
    {
        while (size > 0)
        {
            // Unlike regular copy(*dest_first = *src_first), curr_type_buffer can not be dereferenced since it's type
            // buffer(uninitialized storage)
            ::new (static_cast<void*>(&*curr_type_buffer))
                T{*emplaced_src_begin};  // Emplace T with copy constructor from emplaced T into address

            ++emplaced_src_begin;  // increase to next T memory layout
            ++curr_type_buffer;    // increase to next T memory layout
            size--;
        }
        return curr_type_buffer;
    }
    catch (...)
    {
        while (curr_type_buffer != type_buffer_dest_begin)
        {
            --curr_type_buffer;      // rollback constructed type buffer to begin
            curr_type_buffer->~T();  // call already constructed type buffer
        }
        throw;
    }
}

template <class T, class T_Iter>
void emplace_type_buffers(T_Iter type_buffer_dest_begin, T_Iter type_buffer_dest_end, const T& value)
{
    T_Iter curr_type_buffer = type_buffer_dest_begin;
    try
    {
        while (curr_type_buffer != type_buffer_dest_end)
        {
            // Unlike copy(*dest_first = *src_first), curr_type_buffer can not be dereferenced since it's type
            // buffer(char buffer)
            ::new (static_cast<void*>(&*curr_type_buffer)) T{value};  // Emplace T copy constructor into address

            ++curr_type_buffer;  // increase to next T memory layout
        }
        return curr_type_buffer;
    }
    catch (...)
    {
        while (curr_type_buffer != type_buffer_dest_begin)
        {
            --curr_type_buffer;      // rollback constructed type buffer to begin
            curr_type_buffer->~T();  // call already constructed type buffer
        }
        throw;
    }
}

template <class T, class T_Iter, class... Args>
void emplace_type_buffers(T_Iter type_buffer_dest_begin, T_Iter type_buffer_dest_end, Args... args)
{
    T_Iter curr_type_buffer = type_buffer_dest_begin;
    try
    {
        while (curr_type_buffer != type_buffer_dest_end)
        {
            // Unlike copy(*dest_first = *src_first), curr_type_buffer can not be dereferenced since it's type
            // buffer(char buffer)
            ::new (static_cast<void*>(&*curr_type_buffer)) T{std::forward<Args>(args)...};  // Emplace T into address

            ++curr_type_buffer;  // increase to next T memory layout
        }
        return curr_type_buffer;
    }
    catch (...)
    {
        while (curr_type_buffer != type_buffer_dest_begin)
        {
            --curr_type_buffer;      // rollback constructed type buffer to begin
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
        addr->~U();
    }
}

}  // namespace erturk::type_buffer_memory

#endif  // ERTURK_TYPEBUFFER_MEMORY_H
