#include "Window.h"

Window::Window(HWND hWnd, const std::wstring& name, int width, int height) :
   m_hWnd(hWnd),
   m_name(name),
   m_title(name),
   m_width(width),
   m_height(height)
{
   m_dpiScaling = ::GetDpiForWindow(hWnd) / 96.0f;
}

Window::~Window()
{
   ::DestroyWindow(m_hWnd);
}
