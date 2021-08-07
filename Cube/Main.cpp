#include "Main.h"
#include "Adapter.h"
#include "CommandList.h"
#include "CommandQueue.h"
#include "Device.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Window.h"
#include "AppWorks.h"
#include "SwapChain.h"
#include "d3dx12.h"
#include <shlwapi.h>  // for CommandLineToArgvW
#include <d3dcompiler.h>

using namespace DirectX;

void OnKeyPressed(KeyEventArgs& e);
void OnMouseWheel(MouseWheelEventArgs& e);
void OnResized(ResizeEventArgs& e);
void OnUpdate(UpdateEventArgs& e);
void OnWindowClose(WindowCloseEventArgs& e);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShopw)
{
   int retCode = 0;

#if defined( _DEBUG )
   Device::EnableDebugLayer();
#endif

   // Set the working directory to the path of the executable.
   WCHAR   path[MAX_PATH];
   HMODULE hModule = ::GetModuleHandleW(NULL);
   if (::GetModuleFileNameW(hModule, path, MAX_PATH) > 0)
   {
      ::PathRemoveFileSpecW(path);
      ::SetCurrentDirectoryW(path);
   }

   auto& gf = AppWorks::Create(hInstance);
   {
      // Create a logger for logging messages.
      //logger = gf.CreateLogger("Cube");

      // Create a GPU device using the default adapter selection.
      m_Device = Device::Create();

      auto description = m_Device->GetDescription();
      //logger->info(L"Device Created: {}", description);

      // Use a copy queue to copy GPU resources.
      auto& commandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
      auto  commandList = commandQueue.GetCommandList();

      // Load vertex data:
      m_VertexBuffer =
         commandList->CopyVertexBuffer(_countof(m_vertices), sizeof(VertexPosColor), m_vertices);

      // Load index data:
      m_IndexBuffer = commandList->CopyIndexBuffer(_countof(m_Indicies), DXGI_FORMAT_R16_UINT, m_Indicies);

      // Execute the command list to upload the resources to the GPU.
      commandQueue.ExecuteCommandList(commandList);

      // Create a window:
      m_AppWindow = gf.CreateWindow(L"Cube", 1920, 1080);

      // Create a swapchain for the window
      m_SwapChain = m_Device->CreateSwapChain(m_AppWindow->GetWindowHandle());
      m_SwapChain->SetVSync(false);

      // Register events.
      m_AppWindow->KeyPressed += &OnKeyPressed;
      m_AppWindow->MouseWheel += &OnMouseWheel;
      m_AppWindow->Resize += &OnResized;
      m_AppWindow->Update += &OnUpdate;
      m_AppWindow->Close += &OnWindowClose;

      // Create the vertex input layout
      D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
          { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
          { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
      };

      // Create the root signature.
      // Allow input layout and deny unnecessary access to certain pipeline stages.
      D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
         D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
         D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
         D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
         D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

      // A single 32-bit constant root parameter that is used by the vertex shader.
      CD3DX12_ROOT_PARAMETER1 rootParameters[1];
      rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

      CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription(_countof(rootParameters), rootParameters, 0,
         nullptr, rootSignatureFlags);

      m_RootSignature = m_Device->CreateRootSignature(rootSignatureDescription.Desc_1_1);

      // Load shaders
      // Load the vertex shader.
      ComPtr<ID3DBlob> vertexShaderBlob;
      ThrowIfFailed(D3DReadFileToBlob(L"VertexShader.cso", &vertexShaderBlob));

      // Load the pixel shader.
      ComPtr<ID3DBlob> pixelShaderBlob;
      ThrowIfFailed(D3DReadFileToBlob(L"PixelShader.cso", &pixelShaderBlob));

      // Create the pipeline state object.
      struct PipelineStateStream
      {
         CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE        pRootSignature;
         CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT          InputLayout;
         CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY    PrimitiveTopologyType;
         CD3DX12_PIPELINE_STATE_STREAM_VS                    VS;
         CD3DX12_PIPELINE_STATE_STREAM_PS                    PS;
         CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT  DSVFormat;
         CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
      } pipelineStateStream;

      pipelineStateStream.pRootSignature = m_RootSignature->GetD3D12RootSignature().Get();
      pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
      pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
      pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
      pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
      pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
      pipelineStateStream.RTVFormats = m_SwapChain->GetRenderTarget().GetRenderTargetFormats();

      m_PipelineStateObject = m_Device->CreatePipelineStateObject(pipelineStateStream);

      // Make sure the index/vertex buffers are loaded before rendering the first frame.
      commandQueue.Flush();

      m_AppWindow->Show();

      // Start the game loop.
      retCode = AppWorks::Get().Run();

      // Release globals.
      m_IndexBuffer.reset();
      m_VertexBuffer.reset();
      m_PipelineStateObject.reset();
      m_RootSignature.reset();
      m_DepthTexture.reset();
      m_Device.reset();
      m_SwapChain.reset();
      m_AppWindow.reset();
   }
   // Destroy game framework resource.
   AppWorks::Destroy();

   //atexit(&Device::ReportLiveObjects);

   return retCode;
}

void OnKeyPressed(KeyEventArgs& e)
{
   switch (e.Key)
   {
      case KeyCode::V:
         m_SwapChain->ToggleVSync();
         break;
      case KeyCode::Escape:
         AppWorks::Get().Stop();
         break;
      case KeyCode::Enter:
         if (e.Alt)
         break;
         [[fallthrough]];
      case KeyCode::F11:
         m_AppWindow->ToggleFullScreen();
         break;
   }
}

void OnMouseWheel(MouseWheelEventArgs& e)
{
   m_fieldOfView -= e.WheelDelta;
   m_fieldOfView = std::clamp(m_fieldOfView, 12.0f, 90.0f);
}

void OnResized(ResizeEventArgs& e)
{
   AppWorks::Get().SetDisplaySize(e.Width, e.Height);
   m_Device->Flush();

   m_SwapChain->Resize(e.Width, e.Height);

   auto depthTextureDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, e.Width, e.Height);
   depthTextureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

   D3D12_CLEAR_VALUE optimizedClearValue = {};
   optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
   optimizedClearValue.DepthStencil = { 1.0f, 0 };

   m_DepthTexture = m_Device->CreateTexture(depthTextureDesc, &optimizedClearValue);
}

void OnUpdate(UpdateEventArgs& e)
{
   static uint64_t frameCount = 0;
   static double   totalTime = 0.0;

   totalTime += e.DeltaTime;
   frameCount++;

   if (totalTime > 1.0)
   {
      auto fps = frameCount / totalTime;
      frameCount = 0;
      totalTime -= 1.0;

      //logger->info("FPS: {:.7}", fps);

      wchar_t buffer[256];
      ::swprintf_s(buffer, L"Cube [FPS: %f]", fps);
      m_AppWindow->SetWindowTitle(buffer);
   }

   // Use the render target from the swapchain.
   auto renderTarget = m_SwapChain->GetRenderTarget();
   // Set the render target (with the depth texture).
   renderTarget.AttachTexture(AttachmentPoint::DepthStencil, m_DepthTexture);

   auto viewport = renderTarget.GetViewport();

   // Update the model matrix.
   float          angle = static_cast<float>(e.TotalTime * 90.0);
   const XMVECTOR rotationAxis = XMVectorSet(0, 1, 1, 0);
   XMMATRIX       modelMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));

   // Update the view matrix.
   const XMVECTOR eyePosition = XMVectorSet(0, 0, -10, 1);
   const XMVECTOR focusPoint = XMVectorSet(0, 0, 0, 1);
   const XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
   XMMATRIX       viewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);

   // Update the projection matrix.
   float    aspectRatio = viewport.Width / viewport.Height;
   XMMATRIX projectionMatrix =
      XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fieldOfView), aspectRatio, 0.1f, 100.0f);
   XMMATRIX mvpMatrix = XMMatrixMultiply(modelMatrix, viewMatrix);
   mvpMatrix = XMMatrixMultiply(mvpMatrix, projectionMatrix);

   auto& commandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
   auto  commandList = commandQueue.GetCommandList();

   // Set the pipeline state object
   commandList->SetPipelineState(m_PipelineStateObject);
   // Set the root signatures.
   commandList->SetGraphicsRootSignature(m_RootSignature);

   // Set root parameters
   commandList->SetGraphics32BitConstants(0, mvpMatrix);

   // Clear the color and depth-stencil textures.
   const FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
   commandList->ClearTexture(renderTarget.GetTexture(AttachmentPoint::Color0), clearColor);
   commandList->ClearDepthStencilTexture(m_DepthTexture, D3D12_CLEAR_FLAG_DEPTH);

   commandList->SetRenderTarget(renderTarget);
   commandList->SetViewport(renderTarget.GetViewport());
   commandList->SetScissorRect(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX));

   // Render the cube.
   commandList->SetVertexBuffer(0, m_VertexBuffer);
   commandList->SetIndexBuffer(m_IndexBuffer);
   commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
   commandList->DrawIndexed(m_IndexBuffer->GetNumIndicies());

   commandQueue.ExecuteCommandList(commandList);

   // Present the image to the window.
   m_SwapChain->Present();
}

void OnWindowClose(WindowCloseEventArgs& e)
{
   AppWorks::Get().Stop();
}
