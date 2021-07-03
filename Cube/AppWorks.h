#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdint>      // for uint32_t
#include <limits>       // for std::numeric_limits
#include <memory>       // for std::shared_ptr
#include <mutex>        // for std::mutex
#include <string>       // for std::wstring
#include <thread>       // for std::thread
#include "gainput/gainput.h"

class AppWorks
{
public:
   static AppWorks& Create(HINSTANCE hInst);

   static void Destory();

protected:
   AppWorks(HINSTANCE hInst);
   virtual ~AppWorks();

private:
   HINSTANCE m_hInstance;
   std::atomic_bool m_bIsRunning;
   std::atomic_bool m_bQuit;

   //gainput
   gainput::InputManager m_input;
   gainput::DeviceId     m_keyboard;
   gainput::DeviceId     m_mouse;
   gainput::DeviceId     m_gamePad[gainput::MaxPadCount];

};

