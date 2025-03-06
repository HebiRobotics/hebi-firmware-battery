/** 
 * @file memory.cpp
 *
 * @brief This file contains overridden versions of C++ memory allocation
 * operators.
 *
 * new, new[], delete, delete[] are re-defined here to use the thread-safe
 * chibiOS allocators.
 *
 * @author Yigit Bilgen (mbilgen@andrew)
 */

#include <ch.hpp>
 
#include <stdlib.h>

void* operator new(size_t num) {
  return chHeapAlloc(NULL, num);
}

void operator delete(void* ptr) {
  if (ptr != NULL)
    chHeapFree(ptr);
}

void operator delete(void* ptr, unsigned int) {
  if (ptr != NULL)
    chHeapFree(ptr);
}

void* operator new [] (size_t num) {
  return chHeapAlloc(NULL, num);
}

void operator delete [] (void* ptr) {
  if (ptr != NULL)
    chHeapFree(ptr);
}

void operator delete [] (void* ptr, unsigned int) {
  if (ptr != NULL)
    chHeapFree(ptr);
}
