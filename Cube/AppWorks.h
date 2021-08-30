#pragma once

/*
 *  Copyright(c) 2020 Jeremiah van Oosten
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
  *  @file Application.h
  *  @date September 29, 2020
  *  @author Jeremiah van Oosten
  *
  *  @brief The application class is used to create windows for our application.
  */

#include "Events.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using WndProcEvent = Delegate<LRESULT(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)>;

#include <gainput/gainput.h>


// I want to use a function with this name but it conflicts with the Windows
// macro defined in the Windows header files.
#ifdef CreateWindow
#undef CreateWindow
#endif

#include <cstdint>      // for uint32_t
#include <limits>       // for std::numeric_limits
#include <memory>       // for std::shared_ptr
#include <mutex>        // for std::mutex
#include <string>       // for std::wstring
#include <thread>       // for std::thread
#include <type_traits>  // for std::enable_if

class Window;

class AppWorks
{
public:
   static AppWorks& Create(HINSTANCE hInst);

   std::shared_ptr<Window> CreateWindow(const std::wstring& windowName, int clientWidth, int clientHeight);

   static void Destroy();

   static AppWorks& Get();


   /**
 * Get the keyboard device ID.
 */
   gainput::DeviceId GetKeyboardId() const;

   /**
    * Get the mouse device ID.
    */
   gainput::DeviceId GetMouseId() const;

   /**
    * Get a gamepad device ID.
    *
    * @param index The index of the connected pad [0 ... gainput::MaxPadCount)
    */
   gainput::DeviceId GetPadId(unsigned index = 0) const;

   /**
    * Get an input device.
    *
    * @param InputDevice the Type of device to retrieve. (Must be derived from
    * gainput::InputDevice)
    */
   template<class T>
   T* GetDevice(gainput::DeviceId deviceId) const
   {
      static_assert(std::is_base_of_v<gainput::InputDevice, T>);
      return static_cast<T*>(m_InputManager.GetDevice(deviceId));
   }

   /**
    * Create a gainput input map.
    *
    * @param [name] (Optional) name of the input map.
    * @see http://gainput.johanneskuhlmann.de/api/classgainput_1_1InputMap.html
    */
   std::shared_ptr<gainput::InputMap> CreateInputMap(const char* name = nullptr);

   void Stop();
   int32_t Run();

   void SetDisplaySize(int width, int height);
   void ProcessInput();
   WndProcEvent WndProcHandler;

   Event Exit;

protected:
   friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

   AppWorks(HINSTANCE hInst);
   virtual ~AppWorks();
   virtual LRESULT OnWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
   virtual void OnExit(EventArgs& e);

private:
   void ErrorExit(LPTSTR lpszFunction);

   HINSTANCE m_hInstance;
   std::atomic_bool m_bIsRunning;
   std::atomic_bool m_bRequestQuit;

   //gainput
   gainput::InputManager m_InputManager;
   gainput::DeviceId     m_KeyboardDevice;
   gainput::DeviceId     m_MouseDevice;
   gainput::DeviceId     m_GamepadDevice[gainput::MaxPadCount];

};

