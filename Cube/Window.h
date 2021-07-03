#pragma once

#include <windows.h>
#include <memory>
#include <string>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

class Window
{
public:

protected:
   Window(HWND hWnd, const std::wstring& name, int width, int height);
   virtual ~Window();

private:
   HWND m_hWnd;
   uint32_t m_width;
   uint32_t m_height;
   std::wstring m_name;
   std::wstring m_title;

   float m_dpiScaling;
   //HighResolutionTimer m_timer;
};

