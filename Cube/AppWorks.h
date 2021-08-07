#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdint>      // for uint32_t
#include <limits>       // for std::numeric_limits
#include <memory>       // for std::shared_ptr
#include <mutex>        // for std::mutex
#include <string>       // for std::wstring
#include <thread>       // for std::thread
#include <type_traits>  // for std::enable_if
#include "gainput/gainput.h"
#include "Events.h"


// I want to use a function with this name but it conflicts with the Windows
// macro defined in the Windows header files.
#ifdef CreateWindow
   #undef CreateWindow
#endif

class Window;

using WndProcEvent = Delegate<LRESULT(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)>;


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

