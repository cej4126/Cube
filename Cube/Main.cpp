#include "Window.h"
#include "Device.h"

#include <shlwapi.h>  // for CommandLineToArgvW


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShopw)
{
   int returnCode = 0;

#if defined(_DEBUG)
   Device::enableDebugLayer();
#endif

   // Set the working directory
   WCHAR path[MAX_PATH];
   HMODULE hModule = ::GetModuleHandleW(NULL);
   if (::GetModuleFileNameW(hModule, path, MAX_PATH) > 0)
   {
      ::PathRemoveFileSpecW(path);
      ::SetCurrentDirectoryW(path);
   }


}