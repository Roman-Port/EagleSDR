#pragma once

#include <cstdio>
#include <string>
#include <mutex>
#include <condition_variable>
#include <cstring>

template <typename T>
class RaptorCircularBuffer {

public:
	RaptorCircularBuffer(size_t sizeOfElement, int count) : waiter_lock(lock) {
        bufferPtr = (T*)malloc(sizeOfElement * count);
        bufferSize = count;
        free = count;
        writeIndex = 0;
        readIndex = 0;
	}

	int write(T* ptr, int count) {
        //Lock and write
        lock.lock();
        int written = 0;
        while (count > 0 && free > 0)
        {
            //Calculate how much we can write
            int writable = std::min(count, std::min(free, bufferSize - writeIndex));

            //Copy
            memcpy(bufferPtr + writeIndex, ptr, writable * sizeof(T));

            //Update state
            count -= writable;
            ptr += writable;
            writeIndex = (writeIndex + writable) % bufferSize;
            free -= writable;
            written += writable;
        }

        //Unlock
        lock.unlock();

        //Send an event notifying anyone waiting that it's time to read
        waiter.notify_one();

        return written;
	}

	int read(T* ptr, int count) {
        //Wait and lock
        while (free == bufferSize)
            waiter.wait(waiter_lock);

        //Read
        int read = 0;
        while (count > 0 && (bufferSize - free) > 0)
        {
            //Calculate how much we can read
            int readable = std::min(count, std::min(bufferSize - free, bufferSize - readIndex));

            //Copy
            memcpy(ptr, bufferPtr + readIndex, readable * sizeof(T));

            //Update state
            count -= readable;
            ptr += readable;
            readIndex = (readIndex + readable) % bufferSize;
            free += readable;
            read += readable;
        }

        //Unlock
        lock.unlock();
        return read;
	}

    ~RaptorCircularBuffer() {
        std::free(bufferPtr);
    }

private:
	T* bufferPtr;
    int bufferSize;
	int readIndex;
	int writeIndex;
	int free;
    std::mutex lock;
    std::unique_lock<std::mutex> waiter_lock;
    std::condition_variable waiter;

};