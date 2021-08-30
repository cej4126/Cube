#pragma once

/*
 *  Copyright(c) 2017 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

 /**
  *  @file Helpers.h
  *  @date August 28, 2017
  *  @author Jeremiah van Oosten
  *
  *  @brief Helper functions.
  */

#include <cstdint>
#include <codecvt>
#include <thread>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // For HRESULT
#include <comdef.h> // For _com_error class (used to decode HR result codes).

#include <wrl/client.h>
using namespace Microsoft::WRL;

#include <d3d12.h>

// From DXSampleHelper.h 
// Source: https://github.com/Microsoft/DirectX-Graphics-Samples

inline void ThrowIfFailed(HRESULT hr)
{
   if (FAILED(hr))
   {
      _com_error err(hr);
      OutputDebugString(err.ErrorMessage());

      throw std::exception(err.ErrorMessage());
   }
}

namespace Math
{
   /***************************************************************************
* These functions were taken from the MiniEngine.
* Source code available here:
* https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/Math/Common.h
* Retrieved: January 13, 2016
**************************************************************************/
   template <typename T>
   inline T AlignUpWithMask(T value, size_t mask)
   {
      return (T)(((size_t)value + mask) & ~mask);
   }

   template <typename T>
   inline T AlignUp(T value, size_t alignment)
   {
      return AlignUpWithMask(value, alignment - 1);
   }


   template <typename T>
   inline T DivideByMultiple(T value, size_t alignment)
   {
      return (T)((value + alignment - 1) / alignment);
   }
}


// Set the name of an std::thread.
// Useful for debugging.
const DWORD MS_VC_EXCEPTION = 0x406D1388;

// Set the name of a running thread (for debugging)
#pragma pack( push, 8 )
typedef struct tagTHREADNAME_INFO
{
   DWORD  dwType;      // Must be 0x1000.
   LPCSTR szName;      // Pointer to name (in user addr space).
   DWORD  dwThreadID;  // Thread ID (-1=caller thread).
   DWORD  dwFlags;     // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack( pop )

inline void SetThreadName(std::thread& thread, const char* threadName)
{
   THREADNAME_INFO info;
   info.dwType = 0x1000;
   info.szName = threadName;
   info.dwThreadID = ::GetThreadId(reinterpret_cast<HANDLE>(thread.native_handle()));
   info.dwFlags = 0;

   __try
   {
      ::RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
   }
   __except (EXCEPTION_EXECUTE_HANDLER)
   {
   }
}
