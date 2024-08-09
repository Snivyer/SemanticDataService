#include "abstract/buffer/memory/memory.h"


namespace SDS {

template <class T>
void insertToBuffer(std::vector<char> &buffer, const T *source,
                    const size_t elements) noexcept {

    const char *src = reinterpret_cast<const char *>(source);
    buffer.insert(buffer.end(), src, src + elements * sizeof(T));
}

template <class T>
void copyToBuffer(std::vector<char> &buffer, size_t &position, const T *source,
                  const size_t len) noexcept {
    const char *src = reinterpret_cast<const char *>(source);
    std::copy(src, src + len, buffer.begin() + position);
    position += len;
}

template void copyToBuffer<unsigned char> (std::vector<char> &buffer, size_t &position, const unsigned char *source,
                  const size_t len) noexcept;

template void copyToBuffer<short> (std::vector<char> &buffer, size_t &position, const  short *source,
                  const size_t len) noexcept;

template void copyToBuffer<int> (std::vector<char> &buffer, size_t &position, const  int *source,
                  const size_t len) noexcept;

template void copyToBuffer<char> (std::vector<char> &buffer, size_t &position, const  char *source,
                  const size_t len) noexcept;

template void copyToBuffer<float> (std::vector<char> &buffer, size_t &position, const  float *source,
                  const size_t len) noexcept;

template void copyToBuffer<double> (std::vector<char> &buffer, size_t &position, const double *source,
                  const size_t len) noexcept;

template <class T>
void copyToBufferThreads(std::vector<char> &buffer, size_t &position,
                         const T *source, const size_t elements,
                         const unsigned int threads) noexcept {
    if (elements == 0) {
        return;
    }

    if (threads == 1 || threads > elements) {
        CopyToBuffer(buffer, position, source, elements);
        return;
    }

    const size_t stride = elements / threads;    // elements per thread
    const size_t remainder = elements % threads; // remainder if not aligned
    const size_t last = stride + remainder;

    std::vector<std::thread> copyThreads;
    copyThreads.reserve(threads);

    const char *src = reinterpret_cast<const char *>(source);
    for (unsigned int t = 0; t < threads; ++t) {
        const size_t bufferStart = position + stride * t * sizeof(T);
        const size_t srcStart = stride * t * sizeof(T);

        // last thread takes stride + remainder 
        if (t == threads - 1) {
            copyThreads.push_back(std::thread(std::memcpy, &buffer[bufferStart],
                                              &src[srcStart],
                                              last * sizeof(T)));
        }
        else {
            copyThreads.push_back(std::thread(std::memcpy, &buffer[bufferStart],
                                              &src[srcStart],
                                              stride * sizeof(T)));
        }
    }

    for (auto &copyThread : copyThreads) {
        copyThread.join();
    }
    position += elements * sizeof(T);
}


template <class T>
void reverseCopyFromBuffer(const std::vector<char> &buffer,
                                  size_t &position, T *destination,
                                  const size_t elements) noexcept {
    std::reverse_copy(buffer.begin() + position,
                      buffer.begin() + position + sizeof(T) * elements,
                      reinterpret_cast<char *>(destination));
    position += elements * sizeof(T);
}

template <class T>
void copyFromBuffer(const std::vector<char> &buffer, size_t &position,
                    T *destination, size_t elements) noexcept {
    std::copy(buffer.begin() + position,
              buffer.begin() + position + sizeof(T) * elements,
              reinterpret_cast<char *>(destination));
    position += elements * sizeof(T);
}




}