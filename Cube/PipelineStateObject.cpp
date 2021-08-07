#include "DirectX12Helper.h"
#include "PipelineStateObject.h"
#include "Device.h"

PipelineStateObject::PipelineStateObject(Device& device, const D3D12_PIPELINE_STATE_STREAM_DESC& desc) :
   m_Device(device)
{
   auto d3d12Device = m_Device.GetD3D12Device();

   ThrowIfFailed(d3d12Device->CreatePipelineState(&desc, IID_PPV_ARGS(&m_pipelineState)));
}
