#include "Window.h"

Window::Window(HWND hWnd, const std::wstring& windowName, int clientWidth, int clientHeight) :
   m_hWnd(hWnd),
   m_name(windowName),
   m_title(windowName),
   m_width(clientWidth),
   m_height(clientHeight),
   m_PreviousMouseX(0),
   m_PreviousMouseY(0),
   m_IsFullScreen(false),
   m_IsMinimized(false),
   m_IsMaximized(false),
   m_bInClientRect(false),
   m_bHasKeyboardFocus(false)
{
   m_DPIScaling = ::GetDpiForWindow(hWnd) / 96.0f;
}

Window::~Window()
{
   ::DestroyWindow(m_hWnd);
}

HWND Window::GetWindowHandle() const
{
    return m_hWnd;
}

void Window::Show()
{
   ::ShowWindow(m_hWnd, SW_SHOW);
}

void Window::Hide()
{
   ::ShowWindow(m_hWnd, SW_HIDE);
}

void Window::OnUpdate(UpdateEventArgs& e)
{
   m_Timer.Tick();

   e.DeltaTime = m_Timer.ElapsedSeconds();
   e.TotalTime = m_Timer.TotalSeconds();

   Update(e);
}

void Window::OnClose(WindowCloseEventArgs& e)
{
   Close(e);
}

void Window::OnResize(ResizeEventArgs& e)
{
   m_ClientWidth = e.Width;
   m_ClientHeight = e.Height;

   if ((m_IsMinimized || m_IsMaximized) && e.State == WindowState::Restored)
   {
      m_IsMaximized = false;
      m_IsMinimized = false;
      OnRestored(e);
   }

   if (!m_IsMinimized && e.State == WindowState::Minimized)
   {
      m_IsMaximized = true;
      m_IsMinimized = false;
      OnMinimized(e);
   }

   if (!m_IsMaximized && e.State == WindowState::Maximized)
   {
      m_IsMaximized = false;
      m_IsMinimized = true;
      OnMaximized(e);
   }
   Resize(e);
}

void Window::OnMinimized(ResizeEventArgs& e)
{
   Minimized(e);
}

void Window::OnMaximized(ResizeEventArgs& e)
{
   Maximized(e);
}

void Window::OnRestored(ResizeEventArgs& e)
{
   Restored(e);
}

// The DPI scaling of the window has changed.
void Window::OnDPIScaleChanged(DPIScaleEventArgs& e)
{
   m_DPIScaling = e.DPIScale;
   DPIScaleChanged(e);
}

// A keyboard key was pressed
void Window::OnKeyPressed(KeyEventArgs& e)
{
   KeyPressed(e);
}

// A keyboard key was released
void Window::OnKeyReleased(KeyEventArgs& e)
{
   KeyReleased(e);
}

// Window gained keyboard focus
void Window::OnKeyboardFocus(EventArgs& e)
{
   m_bHasKeyboardFocus = true;
   KeyboardFocus(e);
}

// Window lost keyboard focus
void Window::OnKeyboardBlur(EventArgs& e)
{
   m_bHasKeyboardFocus = false;
   KeyboardBlur(e);
}

// The mouse was moved
void Window::OnMouseMoved(MouseMotionEventArgs& e)
{
   if (!m_bInClientRect)
   {
      m_PreviousMouseX = e.X;
      m_PreviousMouseY = e.Y;
      m_bInClientRect = true;
      // Mouse re-entered the client area.
      OnMouseEnter(e);
   }

   e.RelX = e.X - m_PreviousMouseX;
   e.RelY = e.Y - m_PreviousMouseY;

   m_PreviousMouseX = e.X;
   m_PreviousMouseY = e.Y;

   MouseMoved(e);
}

// A button on the mouse was pressed
void Window::OnMouseButtonPressed(MouseButtonEventArgs& e)
{
   MouseButtonPressed(e);
}

// A button on the mouse was released
void Window::OnMouseButtonReleased(MouseButtonEventArgs& e)
{
   MouseButtonReleased(e);
}

void Window::OnMouseWheel(MouseWheelEventArgs& e)
{
   MouseWheel(e);
}

void Window::OnMouseEnter(MouseMotionEventArgs& e)
{
   // Track mouse leave events.
   TRACKMOUSEEVENT trackMouseEvent = {};
   trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
   trackMouseEvent.hwndTrack = m_hWnd;
   trackMouseEvent.dwFlags = TME_LEAVE;
   TrackMouseEvent(&trackMouseEvent);

   m_bInClientRect = true;
   MouseEnter(e);
}

void Window::OnMouseLeave(EventArgs& e)
{
   m_bInClientRect = false;
   MouseLeave(e);
}

// The window has received mouse focus
void Window::OnMouseFocus(EventArgs& e)
{
   MouseFocus(e);
}

// The window has lost mouse focus
void Window::OnMouseBlur(EventArgs& e)
{
   MouseBlur(e);
}

float Window::GetDPIScaling() const
{
   return m_DPIScaling;
}

void Window::ToggleFullScreen()
{
   SetFullScreen(!m_IsFullScreen);
}

void Window::SetFullScreen(bool fullScreen)
{
   if (m_IsFullScreen != fullScreen)
   {
      m_IsFullScreen = fullScreen;
      if (m_IsFullScreen)
      {
         ::GetWindowRect(m_hWnd, &m_WindowRect);

         UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
         ::SetWindowLongW(m_hWnd, GWL_STYLE, windowStyle);

         HMONITOR hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
         MONITORINFOEX monitorInfo = {};
         monitorInfo.cbSize = sizeof(MONITORINFOEX);
         ::GetMonitorInfo(hMonitor, &monitorInfo);

         ::SetWindowPos(m_hWnd, HWND_TOP,
            monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
            monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
            monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
            SWP_FRAMECHANGED | SWP_NOACTIVATE);
         ::ShowWindow(m_hWnd, SW_MAXIMIZE);
      }
      else
      {
         ::SetWindowLongW(m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
         ::SetWindowPos(m_hWnd, HWND_NOTOPMOST,
            m_WindowRect.left, m_WindowRect.top,
            m_WindowRect.right - m_WindowRect.left,
            m_WindowRect.bottom - m_WindowRect.top,
            SWP_FRAMECHANGED | SWP_NOACTIVATE);
         ::ShowWindow(m_hWnd, SW_DENORMAL);
      }
   }
}

void Window::SetWindowTitle(const std::wstring& windowTitle)
{
   m_title = windowTitle;
   ::SetWindowTextW(m_hWnd, m_title.c_str());
}
