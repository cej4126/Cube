#include <map>
#include "AppWorks.h"
#include "Window.h"
#include "objbase.h"
//#include <windows.h>
#include <strsafe.h>

static AppWorks* g_appWorks = nullptr;
constexpr wchar_t WINDOW_CLASS_NAME[] = L"RenderWindowClass";

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

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

using WindowMap = std::map<HWND, std::weak_ptr<Window>>;

static std::map<HWND, std::weak_ptr<Window>> gs_WindowMap;
static std::map<std::wstring, std::weak_ptr<Window>> gs_WindowMapByName;

static std::mutex gs_WindowHandlesMutex;

struct MakeWindow : public Window
{
   MakeWindow(HWND hWnd, const std::wstring& windowName, int clientWidth, int clientHeight) :
      Window(hWnd, windowName, clientWidth, clientHeight)
   {}
};


AppWorks::AppWorks(HINSTANCE hInst) :
   m_hInstance(hInst),
   m_bIsRunning(false),
   m_bRequestQuit(false)
{
   // Windows 10 Creators update adds Per Monitor V2 DPI awareness context.
   // Using this awareness context allows the client area of the window
   // to achieve 100% scaling while still allowing non-client window content to
   // be rendered in a DPI sensitive fashion.
   // @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setthreaddpiawarenesscontext
   SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

   // Init the gainput lib
   m_KeyboardDevice = m_InputManager.CreateDevice<gainput::InputDeviceKeyboard>();
   m_MouseDevice = m_InputManager.CreateDevice<gainput::InputDeviceMouse>();
   for (unsigned i = 0; i < gainput::MaxPadCount; i++)
   {
      m_GamepadDevice[i] = m_InputManager.CreateDevice<gainput::InputDevicePad>(i);
   }

   m_InputManager.SetDisplaySize(1, 1);

   // Initializes the COM library for use by the calling thread, sets the thread's concurrency model, and creates a new
   // apartment for the thread if one is required.
   // This must be called at least once for each thread that uses the COM library.
   // @see https://docs.microsoft.com/en-us/windows/win32/api/objbase/nf-objbase-coinitialize
   HRESULT hr = CoInitialize(NULL);
   if (FAILED(hr))
   {
      throw;
   }


   WNDCLASSEXW wndClass = { 0 };

   wndClass.cbSize = sizeof(WNDCLASSEX);
   wndClass.style = CS_HREDRAW | CS_VREDRAW;
   wndClass.lpfnWndProc = &WndProc;
   //wndClass.lpfnWndProc = testhandle;
   wndClass.hInstance = m_hInstance;
   wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
   wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
   wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
   wndClass.lpszMenuName = nullptr;
   wndClass.lpszClassName = WINDOW_CLASS_NAME;
   wndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

   if (!RegisterClassExW(&wndClass))
   {
      MessageBoxA(NULL, "Unable to register the window class.", "Error", MB_OK | MB_ICONERROR);
   }

}

AppWorks::~AppWorks()
{
   gs_WindowMap.clear();
   gs_WindowMapByName.clear();
}

AppWorks& AppWorks::Create(HINSTANCE hInst)
{
   if (!g_appWorks)
   {
      g_appWorks = new AppWorks(hInst);
   }
   return *g_appWorks;
}

void AppWorks::Destroy()
{
   if (g_appWorks)
   {
      delete g_appWorks;
      g_appWorks = nullptr;
   }
}

AppWorks& AppWorks::Get()
{
   assert(g_appWorks != nullptr);
   return *g_appWorks;
}


gainput::DeviceId AppWorks::GetKeyboardId() const
{
   return m_KeyboardDevice;
}

gainput::DeviceId AppWorks::GetMouseId() const
{
   return m_MouseDevice;
}

gainput::DeviceId AppWorks::GetPadId(unsigned index /*= 0 */) const
{
   assert(index >= 0 && index < gainput::MaxPadCount);
   return m_GamepadDevice[index];
}

std::shared_ptr<gainput::InputMap> AppWorks::CreateInputMap(const char* name)
{
   return std::make_shared<gainput::InputMap>(m_InputManager, name);
}

int32_t AppWorks::Run()
{
   assert(!m_bIsRunning);
   m_bIsRunning = true;

   MSG msg = {};
   while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) && msg.message != WM_QUIT)
   {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);

      m_InputManager.HandleMessage(msg);

      if (m_bRequestQuit)
      {
         ::PostQuitMessage(0);
         m_bRequestQuit = false;
      }
   }
   m_bIsRunning = false;

   return static_cast<uint32_t>(msg.wParam);
}

void AppWorks::SetDisplaySize(int width, int height)
{
   m_InputManager.SetDisplaySize(width, height);
}

void AppWorks::ProcessInput()
{
   m_InputManager.Update();
}

void AppWorks::Stop()
{
   m_bRequestQuit = true;
}

std::shared_ptr<Window> AppWorks::CreateWindow(const std::wstring& windowName, int clientWidth, int clientHeight)
{
   int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
   int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

   RECT windowRect = { 0, 0, static_cast<LONG>(clientWidth), static_cast<LONG>(clientHeight) };

   ::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

   uint32_t width = windowRect.right - windowRect.left;
   uint32_t height = windowRect.bottom - windowRect.top;

   int windowX = std::max<int>(0, (screenWidth - (int)width) / 2);
   int windowY = std::max<int>(0, (screenHeight - (int)height) / 2);
   HWND hWindow = ::CreateWindowExW(NULL, WINDOW_CLASS_NAME, windowName.c_str(), WS_OVERLAPPEDWINDOW, windowX,
      windowY, width, height, NULL, NULL, m_hInstance, NULL);

   if (!hWindow)
   {
      ErrorExit(TEXT("CreateWindowExW - "));
      assert(false);
      return nullptr;
   }

   auto pWindow = std::make_shared<MakeWindow>(hWindow, windowName, clientWidth, clientHeight);

   gs_WindowMap.insert(std::map<HWND, std::weak_ptr<Window>>::value_type(hWindow, pWindow));
   gs_WindowMapByName.insert(std::map<std::wstring, std::weak_ptr<Window>>::value_type(windowName, pWindow));

   return pWindow;
}

LRESULT AppWorks::OnWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   auto res = WndProcHandler(hWnd, msg, wParam, lParam);
   return res ? *res : 0;
}

void AppWorks::OnExit(EventArgs& e)
{
   // Invoke the Exit event.
   Exit(e);
}

// Convert the message ID into a MouseButton ID
static MouseButton DecodeMouseButton(UINT messageID)
{
   MouseButton mouseButton = MouseButton::None;
   switch (messageID)
   {
      case WM_LBUTTONDOWN:
      case WM_LBUTTONUP:
      case WM_LBUTTONDBLCLK:
      {
         mouseButton = MouseButton::Left;
      }
      break;
      case WM_RBUTTONDOWN:
      case WM_RBUTTONUP:
      case WM_RBUTTONDBLCLK:
      {
         mouseButton = MouseButton::Right;
      }
      break;
      case WM_MBUTTONDOWN:
      case WM_MBUTTONUP:
      case WM_MBUTTONDBLCLK:
      {
         mouseButton = MouseButton::Middle;
      }
      break;
   }

   return mouseButton;
}

// Convert the message ID into a ButtonState.
static ButtonState DecodeButtonState(UINT messageID)
{
   ButtonState buttonState = ButtonState::Pressed;

   switch (messageID)
   {
      case WM_LBUTTONUP:
      case WM_RBUTTONUP:
      case WM_MBUTTONUP:
      case WM_XBUTTONUP:
         buttonState = ButtonState::Released;
         break;
      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN:
      case WM_MBUTTONDOWN:
      case WM_XBUTTONDOWN:
         buttonState = ButtonState::Pressed;
         break;
   }

   return buttonState;
}

// Convert wParam of the WM_SIZE events to a WindowState.
static WindowState DecodeWindowState(WPARAM wParam)
{
   WindowState windowState = WindowState::Restored;

   switch (wParam)
   {
      case SIZE_RESTORED:
         windowState = WindowState::Restored;
         break;
      case SIZE_MINIMIZED:
         windowState = WindowState::Minimized;
         break;
      case SIZE_MAXIMIZED:
         windowState = WindowState::Maximized;
         break;
      default:
         break;
   }

   return windowState;
}
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   // Allow for external handling of window messages.
   if (AppWorks::Get().OnWndProc(hwnd, message, wParam, lParam))
   {
      return 1;
   }

   std::shared_ptr<Window> pWindow;
   {
      auto iter = gs_WindowMap.find(hwnd);
      if (iter != gs_WindowMap.end())
      {
         pWindow = iter->second.lock();
      }
   }

   if (pWindow)
   {
      switch (message)
      {
         case WM_DPICHANGED:
         {
            float             dpiScaling = HIWORD(wParam) / 96.0f;
            DPIScaleEventArgs dpiScaleEventArgs(dpiScaling);
            pWindow->OnDPIScaleChanged(dpiScaleEventArgs);
         }
         break;
         case WM_PAINT:
         {
            // Delta and total time will be filled in by the Window.
            UpdateEventArgs updateEventArgs(0.0, 0.0);
            pWindow->OnUpdate(updateEventArgs);
         }
         break;
         case WM_SYSKEYDOWN:
         case WM_KEYDOWN:
         {
            MSG charMsg;

            // Get the Unicode character (UTF-16)
            unsigned int c = 0;
            // For printable characters, the next message will be WM_CHAR.
            // This message contains the character code we need to send the
            // KeyPressed event. Inspired by the SDL 1.2 implementation.
            if (PeekMessage(&charMsg, hwnd, 0, 0, PM_NOREMOVE) && charMsg.message == WM_CHAR)
            {
               //                GetMessage( &charMsg, hwnd, 0, 0 );
               c = static_cast<unsigned int>(charMsg.wParam);
            }

            bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

            KeyCode      key = (KeyCode)wParam;
            KeyEventArgs keyEventArgs(key, c, KeyState::Pressed, control, shift, alt);
            pWindow->OnKeyPressed(keyEventArgs);
         }
         break;
         case WM_SYSKEYUP:
         case WM_KEYUP:
         {
            bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

            KeyCode      key = (KeyCode)wParam;
            unsigned int c = 0;
            unsigned int scanCode = (lParam & 0x00FF0000) >> 16;

            // Determine which key was released by converting the key code and
            // the scan code to a printable character (if possible). Inspired by
            // the SDL 1.2 implementation.
            unsigned char keyboardState[256];
            GetKeyboardState(keyboardState);
            wchar_t translatedCharacters[4];
            if (int result =
               ToUnicodeEx((UINT)wParam, scanCode, keyboardState, translatedCharacters, 4, 0, NULL) > 0)
            {
               c = translatedCharacters[0];
            }

            KeyEventArgs keyEventArgs(key, c, KeyState::Released, control, shift, alt);
            pWindow->OnKeyReleased(keyEventArgs);
         }
         break;
         // The default window procedure will play a system notification sound
         // when pressing the Alt+Enter keyboard combination if this message is
         // not handled.
         case WM_SYSCHAR:
            break;
         case WM_KILLFOCUS:
         {
            // Window lost keyboard focus.
            EventArgs eventArgs;
            pWindow->OnKeyboardBlur(eventArgs);
         }
         break;
         case WM_SETFOCUS:
         {
            EventArgs eventArgs;
            pWindow->OnKeyboardFocus(eventArgs);
         }
         break;
         case WM_MOUSEMOVE:
         {
            bool lButton = (wParam & MK_LBUTTON) != 0;
            bool rButton = (wParam & MK_RBUTTON) != 0;
            bool mButton = (wParam & MK_MBUTTON) != 0;
            bool shift = (wParam & MK_SHIFT) != 0;
            bool control = (wParam & MK_CONTROL) != 0;

            int x = ((int)(short)LOWORD(lParam));
            int y = ((int)(short)HIWORD(lParam));

            MouseMotionEventArgs mouseMotionEventArgs(lButton, mButton, rButton, control, shift, x, y);
            pWindow->OnMouseMoved(mouseMotionEventArgs);
         }
         break;
         case WM_LBUTTONDOWN:
         case WM_RBUTTONDOWN:
         case WM_MBUTTONDOWN:
         {
            bool lButton = (wParam & MK_LBUTTON) != 0;
            bool rButton = (wParam & MK_RBUTTON) != 0;
            bool mButton = (wParam & MK_MBUTTON) != 0;
            bool shift = (wParam & MK_SHIFT) != 0;
            bool control = (wParam & MK_CONTROL) != 0;

            int x = ((int)(short)LOWORD(lParam));
            int y = ((int)(short)HIWORD(lParam));

            // Capture mouse movement until the button is released.
            SetCapture(hwnd);

            MouseButtonEventArgs mouseButtonEventArgs(DecodeMouseButton(message), ButtonState::Pressed, lButton,
               mButton, rButton, control, shift, x, y);
            pWindow->OnMouseButtonPressed(mouseButtonEventArgs);
         }
         break;
         case WM_LBUTTONUP:
         case WM_RBUTTONUP:
         case WM_MBUTTONUP:
         {
            bool lButton = (wParam & MK_LBUTTON) != 0;
            bool rButton = (wParam & MK_RBUTTON) != 0;
            bool mButton = (wParam & MK_MBUTTON) != 0;
            bool shift = (wParam & MK_SHIFT) != 0;
            bool control = (wParam & MK_CONTROL) != 0;

            int x = ((int)(short)LOWORD(lParam));
            int y = ((int)(short)HIWORD(lParam));

            // Stop capturing the mouse.
            ReleaseCapture();

            MouseButtonEventArgs mouseButtonEventArgs(DecodeMouseButton(message), ButtonState::Released, lButton,
               mButton, rButton, control, shift, x, y);
            pWindow->OnMouseButtonReleased(mouseButtonEventArgs);
         }
         break;
         case WM_MOUSEWHEEL:
         {
            // The distance the mouse wheel is rotated.
            // A positive value indicates the wheel was rotated forwards (away
            //  the user). A negative value indicates the wheel was rotated
            //  backwards (toward the user).
            float zDelta = ((int)(short)HIWORD(wParam)) / (float)WHEEL_DELTA;
            short keyStates = (short)LOWORD(wParam);

            bool lButton = (keyStates & MK_LBUTTON) != 0;
            bool rButton = (keyStates & MK_RBUTTON) != 0;
            bool mButton = (keyStates & MK_MBUTTON) != 0;
            bool shift = (keyStates & MK_SHIFT) != 0;
            bool control = (keyStates & MK_CONTROL) != 0;

            int x = ((int)(short)LOWORD(lParam));
            int y = ((int)(short)HIWORD(lParam));

            // Convert the screen coordinates to client coordinates.
            POINT screenToClientPoint;
            screenToClientPoint.x = x;
            screenToClientPoint.y = y;
            ::ScreenToClient(hwnd, &screenToClientPoint);

            MouseWheelEventArgs mouseWheelEventArgs(zDelta, lButton, mButton, rButton, control, shift,
               (int)screenToClientPoint.x, (int)screenToClientPoint.y);
            pWindow->OnMouseWheel(mouseWheelEventArgs);
         }
         break;
         // NOTE: Not really sure if these next set of messages are working
         // correctly. Not really sure HOW to get them to work correctly.
         // TODO: Try to fix these if I need them ;)
         case WM_CAPTURECHANGED:
         {
            EventArgs mouseBlurEventArgs;
            pWindow->OnMouseBlur(mouseBlurEventArgs);
         }
         break;
         case WM_MOUSEACTIVATE:
         {
            EventArgs mouseFocusEventArgs;
            pWindow->OnMouseFocus(mouseFocusEventArgs);
         }
         break;
         case WM_MOUSELEAVE:
         {
            EventArgs mouseLeaveEventArgs;
            pWindow->OnMouseLeave(mouseLeaveEventArgs);
         }
         break;
         case WM_SIZE:
         {
            WindowState windowState = DecodeWindowState(wParam);

            int width = ((int)(short)LOWORD(lParam));
            int height = ((int)(short)HIWORD(lParam));

            ResizeEventArgs resizeEventArgs(width, height, windowState);
            pWindow->OnResize(resizeEventArgs);
         }
         break;
         case WM_CLOSE:
         {
            WindowCloseEventArgs windowCloseEventArgs;
            pWindow->OnClose(windowCloseEventArgs);

            // Check to see if the user canceled the close event.
            if (windowCloseEventArgs.ConfirmClose)
            {
               // DestroyWindow( hwnd );
               // Just hide the window.
               // Windows will be destroyed when the application quits.
               pWindow->Hide();
            }
         }
         break;
         case WM_DESTROY:
         {
            std::lock_guard<std::mutex> lock(gs_WindowHandlesMutex);
            WindowMap::iterator         iter = gs_WindowMap.find(hwnd);
            if (iter != gs_WindowMap.end())
            {
               gs_WindowMap.erase(iter);
            }
         }
         break;
         default:
            return ::DefWindowProcW(hwnd, message, wParam, lParam);
      }
   }
   else
   {
      switch (message)
      {
         case WM_CREATE:
            break;
         default:
            return ::DefWindowProcW(hwnd, message, wParam, lParam);
      }
   }

   return 0;
}

void AppWorks::ErrorExit(LPTSTR lpszFunction)
{
   // Retrieve the system error message for the last-error code

   LPVOID lpMsgBuf;
   LPVOID lpDisplayBuf;
   DWORD dw = GetLastError();

   FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      dw,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR)&lpMsgBuf,
      0, NULL);

   // Display the error message and exit the process

   lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
      (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
   StringCchPrintf((LPTSTR)lpDisplayBuf,
      LocalSize(lpDisplayBuf) / sizeof(TCHAR),
      TEXT("%s failed with error %d: %s"),
      lpszFunction, dw, lpMsgBuf);
   MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

   LocalFree(lpMsgBuf);
   LocalFree(lpDisplayBuf);
   ExitProcess(dw);
}
