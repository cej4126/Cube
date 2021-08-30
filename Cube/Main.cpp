#include "Main.h"
#include "Window.h"
#include "AppWorks.h"
#include <dxgi1_3.h>
#include <dxgidebug.h>
#include <d3d12.h>
#include <shlwapi.h>  // for CommandLineToArgvW
#include "Tutorial3.h"
#include "directX12Helper.h"
#include <wrl/client.h>

void ReportLiveObjects()
{
   IDXGIDebug1* dxgiDebug;
   ::DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug));
   dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_IGNORE_INTERNAL);
   dxgiDebug->Release();
}

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
   int retCode = 0;

#if defined( _DEBUG )
   Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
   ThrowIfFailed(::D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
   debugInterface->EnableDebugLayer();
#endif

   AppWorks::Create(hInstance);
   {
      std::unique_ptr<Tutorial3> demo = std::make_unique<Tutorial3>(L"Textures", 1920, 1080);
      retCode = demo->Run();
   }
   AppWorks::Destroy();

   atexit(ReportLiveObjects);

   return retCode;
};