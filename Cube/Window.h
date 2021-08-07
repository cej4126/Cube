#pragma once

#include <windows.h>
#include <memory>
#include <string>
#include "Events.h"
#include "HighResolutionTimer.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

class Window
{
public:
   HWND GetWindowHandle() const;
   float GetDPIScaling() const;

   void Show();
   void Hide();

   //bool IsFullscreen() const;
   void SetFullScreen(bool fullScreen);
   void ToggleFullScreen();
   void SetWindowTitle(const std::wstring& windowTitle);

   UpdateEvent Update;
   DPIScaleEvent DPIScaleChanged;
   WindowCloseEvent Close;
   ResizeEvent Resize;
   ResizeEvent Minimized;
   ResizeEvent Maximized;
   ResizeEvent Restored;
   KeyboardEvent KeyPressed;
   KeyboardEvent KeyReleased;
   Event KeyboardFocus;
   Event KeyboardBlur;
   MouseMotionEvent MouseMoved;
   MouseMotionEvent MouseEnter;
   MouseButtonEvent MouseButtonPressed;
   MouseButtonEvent MouseButtonReleased;
   MouseWheelEvent MouseWheel;
   Event MouseLeave;
   Event MouseFocus;
   Event MouseBlur;

protected:
   friend class AppWorks;
   friend LRESULT CALLBACK::WndProc(HWND, UINT, WPARAM, LPARAM);

   Window(HWND hWnd, const std::wstring& windowName, int clientWidth, int clientHeight);
   virtual ~Window();

   virtual void OnUpdate(UpdateEventArgs& e);
   virtual void OnDPIScaleChanged(DPIScaleEventArgs& e);
   virtual void OnClose(WindowCloseEventArgs& e);
   virtual void OnResize(ResizeEventArgs& e);
   virtual void OnMinimized(ResizeEventArgs& e);
   virtual void OnMaximized(ResizeEventArgs& e);
   virtual void OnRestored(ResizeEventArgs& e);
   // A keyboard key was pressed.
   virtual void OnKeyPressed(KeyEventArgs& e);
   // A keyboard key was released
   virtual void OnKeyReleased(KeyEventArgs& e);
   // Window gained keyboard focus
   virtual void OnKeyboardFocus(EventArgs& e);
   // Window lost keyboard focus
   virtual void OnKeyboardBlur(EventArgs& e);

   // The mouse was moved
   virtual void OnMouseMoved(MouseMotionEventArgs& e);
   // A button on the mouse was pressed
   virtual void OnMouseButtonPressed(MouseButtonEventArgs& e);
   // A button on the mouse was released
   virtual void OnMouseButtonReleased(MouseButtonEventArgs& e);
   // The mouse wheel was moved.
   virtual void OnMouseWheel(MouseWheelEventArgs& e);

   // The mouse entered the client area.
   virtual void OnMouseEnter(MouseMotionEventArgs& e);
   // The mouse left the client are of the window.
   virtual void OnMouseLeave(EventArgs& e);
   // The application window has received mouse focus
   virtual void OnMouseFocus(EventArgs& e);
   // The application window has lost mouse focus
   virtual void OnMouseBlur(EventArgs& e);

private:
   HWND m_hWnd;
   uint32_t m_width;
   uint32_t m_height;
   std::wstring m_name;
   std::wstring m_title;

   uint32_t m_ClientWidth;
   uint32_t m_ClientHeight;

   RECT m_WindowRect;

   int32_t m_PreviousMouseX;
   int32_t m_PreviousMouseY;

   float m_DPIScaling;
   bool m_IsMinimized;
   bool m_IsMaximized;
   bool m_bHasKeyboardFocus;
   bool m_bInClientRect;
   bool m_IsFullScreen;
   HighResolutionTimer m_Timer;
};

