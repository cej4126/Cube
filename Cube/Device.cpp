#include "Device.h"

void Device::enableDebugLayer()
{
   ComPtr<ID3D12Debug> debugInterface;
   ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
   debugInterface->EnableDebugLayer();
}
