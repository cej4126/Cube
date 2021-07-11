#include "Device.h"
#include "CommandQueue.h"

class MakeDevice : public Device
{
public:
   MakeDevice(std::shared_ptr<Adapter> adapter) :
      Device(adapter)
   {}

   virtual ~MakeDevice() {}
};

class MakeCommandQueue : public CommandQueue
{
public:
   MakeCommandQueue(Device& device, D3D12_COMMAND_LIST_TYPE type)
      : CommandQueue(device, type)
   {}

   virtual ~MakeCommandQueue() {}
};

void Device::enableDebugLayer()
{
   ComPtr<ID3D12Debug> debugInterface;
   ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
   debugInterface->EnableDebugLayer();
}

std::shared_ptr<Device> Device::Create(std::shared_ptr<Adapter> adapter)
{
    return std::make_shared<MakeDevice>(adapter);
}

CommandQueue& Device::getCommandQueue(D3D12_COMMAND_LIST_TYPE type)
{
   CommandQueue* commandQueue = nullptr;
   switch (type)
   {
      case D3D12_COMMAND_LIST_TYPE_DIRECT:
         commandQueue = m_directCommandQueue.get();
         break;
      case D3D12_COMMAND_LIST_TYPE_COMPUTE:
         commandQueue = m_computeCommandQueue.get();
         break;
      case D3D12_COMMAND_LIST_TYPE_COPY:
         commandQueue = m_copyCommandQueue.get();
         break;
      default:
         assert(false && "Invalid command queue type.");
   }

   return *commandQueue;
}

Device::Device(std::shared_ptr<Adapter> adapter) :
   m_adapter(adapter)
{
   if (!m_adapter)
   {
      m_adapter = Adapter::Create();
      assert(m_adapter);
   }

   auto dxgiAdapter = m_adapter->getDXGIAdapter();

   ThrowIfFailed(D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));

   ComPtr<ID3D12InfoQueue> pInfoQueue;
   if (SUCCEEDED(m_device.As(&pInfoQueue)))
   {
      pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
      pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
      pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

      D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
      D3D12_MESSAGE_ID denyIds[] =
      {
         D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
         D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
         D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
      };

      D3D12_INFO_QUEUE_FILTER newFilter = {};
      newFilter.DenyList.NumSeverities = _countof(severities);
      newFilter.DenyList.pSeverityList = severities;
      newFilter.DenyList.NumIDs = _countof(denyIds);
      newFilter.DenyList.pIDList = denyIds;

      ThrowIfFailed(pInfoQueue->PushStorageFilter(&newFilter));
   }

   m_directCommandQueue = std::make_unique<MakeCommandQueue>(*this, D3D12_COMMAND_LIST_TYPE_DIRECT);
   m_computeCommandQueue = std::make_unique<MakeCommandQueue>(*this, D3D12_COMMAND_LIST_TYPE_COMPUTE);
   m_copyCommandQueue = std::make_unique<MakeCommandQueue>(*this, D3D12_COMMAND_LIST_TYPE_COPY);
}
