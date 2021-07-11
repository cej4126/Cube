#include "DirectX12Helper.h"
#include "Adapter.h"

class MakeAdapter : public Adapter
{
public:
   MakeAdapter(Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter) :
      Adapter(dxgiAdapter)
   {}

   virtual ~MakeAdapter() {}
};

std::shared_ptr<Adapter> Adapter::Create(
   DXGI_GPU_PREFERENCE gpuPreference,
   bool useWarp)
{
   std::shared_ptr<Adapter> adapter = nullptr;

   ComPtr<IDXGIFactory6> dxgiFactory6;
   ComPtr<IDXGIAdapter> dxgiAdapter;
   ComPtr<IDXGIAdapter4> dxgiAdapter4;

   UINT createFactoryFlags = 0;
#if defined(_DEBUG)
   createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

   ThrowIfFailed(::CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory6)));

   if (useWarp)
   {
      ThrowIfFailed(dxgiFactory6->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter)));
      ThrowIfFailed(dxgiAdapter.As(&dxgiAdapter4));
   }
   else
   {
      for (UINT i = 0; dxgiFactory6->EnumAdapterByGpuPreference(i, gpuPreference, IID_PPV_ARGS(&dxgiAdapter)) != DXGI_ERROR_NOT_FOUND; ++i)
      {
         if (SUCCEEDED(D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0,
            __uuidof(ID3D12Device), nullptr)))
         {
            ThrowIfFailed(dxgiAdapter.As(&dxgiAdapter4));
            break;
         }
      }
   }
   
   if (dxgiAdapter4)
   {
      adapter = std::make_shared<MakeAdapter>(dxgiAdapter4);
   }

   return adapter;
}

Adapter::Adapter(Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter) :
   m_dxgiAdapter(dxgiAdapter),
   m_desc{0}
{
   if (m_dxgiAdapter)
   {
      ThrowIfFailed(m_dxgiAdapter->GetDesc3(&m_desc));
   }
}

Microsoft::WRL::ComPtr<IDXGIAdapter4> Adapter::getDXGIAdapter() const
{
   return m_dxgiAdapter;
}