#include "AppWorks.h"

static AppWorks* appWorks = nullptr;

AppWorks& AppWorks::Create(HINSTANCE hInst)
{
   if (!appWorks)
   {
      appWorks = new AppWorks(hInst);
   }
   return *appWorks;
}

void AppWorks::Destory()
{
   if (appWorks)
   {
      delete appWorks;
      appWorks = nullptr;
   }
}

AppWorks::AppWorks(HINSTANCE hInst) :
   m_hInstance(hInst),
   m_bIsRunning(false),
   m_bQuit(false)
{
   // Windows 10 Creators update adds Per Monitor V2 DPI awareness context.
   // Using this awareness context allows the client area of the window
   // to achieve 100% scaling while still allowing non-client window content to
   // be rendered in a DPI sensitive fashion.
   // @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setthreaddpiawarenesscontext
   SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

   // Init the gainput lib
   m_keyboard = m_input.CreateDevice<gainput::InputDeviceKeyboard>();
   m_mouse = m_input.CreateDevice<gainput::InputDeviceMouse>();
   for (unsigned i = 0; i < gainput::MaxPadCount; i++)
   {
      m_gamePad[i] = m_input.CreateDevice<gainput::InputDevicePad>(i);
   }


}

AppWorks::~AppWorks()
{
}
