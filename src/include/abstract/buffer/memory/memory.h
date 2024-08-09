/* Author: Snivyer
*  Create on: 2027/7/19
*  Description:
   Memory Class, learning from adios2 adios_memory.h
*/

#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <thread>
#include <algorithm>


namespace SDS {

template <class T>
void insertToBuffer(std::vector<char> &buffer, const T *source,
                    const size_t elements) noexcept;

template <class T>
void copyToBuffer(std::vector<char> &buffer, size_t &position, const T *source,
                  const size_t len) noexcept;

template <class T>
void copyToBufferThreads(std::vector<char> &buffer, size_t &position,
                         const T *source, const size_t elements,
                         const unsigned int threads) noexcept;


template <class T>
void reverseCopyFromBuffer(const std::vector<char> &buffer,
                                  size_t &position, T *destination,
                                  const size_t elements) noexcept;
template <class T>
void copyFromBuffer(const std::vector<char> &buffer, size_t &position,
                    T *destination, size_t elements) noexcept;


}
