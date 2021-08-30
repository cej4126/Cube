#include "AppWorks.h"

#include "Tutorial3.h"

#include <dxgi1_3.h>
#include <dxgidebug.h>
#include <d3d12.h>
#include <shlwapi.h>  // for CommandLineToArgvW
#include <d3dcompiler.h>
#include <DirectXColors.h>
#include "Adapter.h"
#include "CommandList.h"
#include "CommandQueue.h"
#include "Device.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Window.h"
#include "SwapChain.h"
#include "d3dx12.h"
#include "SceneVisitor.h"
#include "Material.h"
#include "Scene.h"
#include "Texture.h"
#include "SwapChain.h"
#include "RenderTarget.h"
#include "GUI.h"
#include "RootSignature.h"
#include "imgui.h"
#include "Events.h"

using namespace DirectX;

struct Mat
{
   XMMATRIX ModeMatrix;
   XMMATRIX ModelViewMatrix;
   XMMATRIX InverseTransposeModelViewMatrix;
   XMMATRIX ModelViewProjectionMatrix;
};

struct LightProperties
{
   uint32_t NumPointLights;
   uint32_t NumSpotLights;
};

enum RootParameters
{
   MatricesCB,        // b0
   MaterialCB,        // b0, space1
   LightPropertiesCB, // b1
   PointLights,       // t0
   SpotLights,        // t1
   Textures,          // t2
   NumRootParameters
};


// Builds a look-at (world) matrix from a point, up and direction vectors.
XMMATRIX XM_CALLCONV LookAtMatrix(FXMVECTOR Position, FXMVECTOR Direction, FXMVECTOR Up)
{
   assert(!XMVector3Equal(Direction, XMVectorZero()));
   assert(!XMVector3IsInfinite(Direction));
   assert(!XMVector3Equal(Up, XMVectorZero()));
   assert(!XMVector3IsInfinite(Up));

   XMVECTOR R2 = XMVector3Normalize(Direction);

   XMVECTOR R0 = XMVector3Cross(Up, R2);
   R0 = XMVector3Normalize(R0);

   XMVECTOR R1 = XMVector3Cross(R2, R0);

   XMMATRIX M(R0, R1, R2, Position);

   return M;
}


Tutorial3::Tutorial3(const std::wstring& name, uint32_t width, uint32_t height, bool vSync) :
   m_Width(width),
   m_Height(height),
   m_VSync(vSync),
   m_ScissorRect(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX)),
   m_Viewport(CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height))),
   m_Shift(false),
   m_AnimateLights(false),
   m_Forward(0.0f),
   m_Backward(0.0f),
   m_Left(0.0f),
   m_Right(0.0f),
   m_Up(0.0f),
   m_Down(0.0f),
   m_Pitch(0.0f),
   m_Yaw(0.0f)
{
   m_Window = AppWorks::Get().CreateWindow(name, width, height);

   m_Window->Update += UpdateEvent::slot(&Tutorial3::OnUpdate, this);
   m_Window->KeyPressed += KeyboardEvent::slot(&Tutorial3::OnKeyPressed, this);
   m_Window->KeyReleased += KeyboardEvent::slot(&Tutorial3::OnKeyReleased, this);
   m_Window->MouseMoved += MouseMotionEvent::slot(&Tutorial3::OnMouseMoved, this);
   m_Window->MouseWheel += MouseWheelEvent::slot(&Tutorial3::OnMouseWheel, this);
   m_Window->Resize += ResizeEvent::slot(&Tutorial3::OnResize, this);

   XMVECTOR cameraPos = XMVectorSet(0, 5, -20, 1);
   XMVECTOR cameraTarget = XMVectorSet(0, 5, 0, 1);
   XMVECTOR cameraUp = XMVectorSet(0, 1, 0, 0);

   m_Camera.set_LookAt(cameraPos, cameraTarget, cameraUp);

   m_pAlignedCameraData = (CameraData*)_aligned_malloc(sizeof(CameraData), 16);
   m_pAlignedCameraData->m_InitialCamPos = m_Camera.get_Translation();
   m_pAlignedCameraData->m_InitialCamRot = m_Camera.get_Rotation();
}

Tutorial3::~Tutorial3()
{
   _aligned_free(m_pAlignedCameraData);
}

uint32_t Tutorial3::Run()
{
   LoadContent();
   m_Window->Show();

   uint32_t retCode = AppWorks::Get().Run();

   UnloadContent();

   return retCode;
}

bool Tutorial3::LoadContent()
{
   m_Device = Device::Create();

   m_SwapChain = m_Device->CreateSwapChain(m_Window->GetWindowHandle(), DXGI_FORMAT_R8G8B8A8_UNORM);
   m_SwapChain->SetVSync(m_VSync);

   m_GUI = m_Device->CreateGUI(m_Window->GetWindowHandle(), m_SwapChain->GetRenderTarget());

   AppWorks::Get().WndProcHandler += WndProcEvent::slot(&GUI::WndProcHandler, m_GUI);

   auto& commandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
   auto commandList = commandQueue.GetCommandList();

   // Create some geometry to render.
   m_Cube = commandList->CreateCube();
   m_Sphere = commandList->CreateSphere();
   m_Cone = commandList->CreateCone();
   m_Torus = commandList->CreateTorus();
   m_Plane = commandList->CreatePlane();

   // Load some textures
   m_DefaultTexture = commandList->LoadTextureFromFile(L"Assets/Textures/DefaultWhite.bmp", true);
   m_DirectXTexture = commandList->LoadTextureFromFile(L"Assets/Textures/Directx9.png", true);
   m_EarthTexture = commandList->LoadTextureFromFile(L"Assets/Textures/earth.dds", true);
   m_MonaLisaTexture = commandList->LoadTextureFromFile(L"Assets/Textures/Mona_Lisa.jpg", true);

   commandQueue.ExecuteCommandList(commandList);

   ComPtr<ID3DBlob> vertexShaderBlob;
   ThrowIfFailed(D3DReadFileToBlob(L"VertexShader.cso", &vertexShaderBlob));
   ComPtr<ID3DBlob> pixelShaderBlob;
   ThrowIfFailed(D3DReadFileToBlob(L"PixelShader.cso", &pixelShaderBlob));
   ComPtr<ID3DBlob> unlitPixeShaderlBlob;
   ThrowIfFailed(D3DReadFileToBlob(L"UnlitPixelShader.cso", &unlitPixeShaderlBlob));

   D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
      D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
      D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
      D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

   CD3DX12_DESCRIPTOR_RANGE1 descriptorRage(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);

   CD3DX12_ROOT_PARAMETER1 rootParameters[RootParameters::NumRootParameters];
   rootParameters[RootParameters::MatricesCB].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
      D3D12_SHADER_VISIBILITY_VERTEX);
   rootParameters[RootParameters::MaterialCB].InitAsConstantBufferView(0, 1, D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
      D3D12_SHADER_VISIBILITY_PIXEL);
   rootParameters[RootParameters::LightPropertiesCB].InitAsConstants(sizeof(LightProperties) / 4, 1, 0,
      D3D12_SHADER_VISIBILITY_PIXEL);
   rootParameters[RootParameters::PointLights].InitAsShaderResourceView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
      D3D12_SHADER_VISIBILITY_PIXEL);
   rootParameters[RootParameters::SpotLights].InitAsShaderResourceView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
      D3D12_SHADER_VISIBILITY_PIXEL);
   rootParameters[RootParameters::Textures].InitAsDescriptorTable(1, &descriptorRage, D3D12_SHADER_VISIBILITY_PIXEL);

   CD3DX12_STATIC_SAMPLER_DESC linearRepeatSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
   //CD3DX12_STATIC_SAMPLER_DESC anisotropicSampler(0, D3D12_FILTER_ANISOTROPIC);

   CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
   rootSignatureDescription.Init_1_1(RootParameters::NumRootParameters, rootParameters, 1, &linearRepeatSampler,
      rootSignatureFlags);

   m_RootSignature = m_Device->CreateRootSignature(rootSignatureDescription.Desc_1_1);
   // Setup the pipeline state.
   struct PipelineStateStream
   {
      CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE        pRootSignature;
      CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT          InputLayout;
      CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY    PrimitiveTopologyType;
      CD3DX12_PIPELINE_STATE_STREAM_VS                    VS;
      CD3DX12_PIPELINE_STATE_STREAM_PS                    PS;
      CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT  DSVFormat;
      CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
      CD3DX12_PIPELINE_STATE_STREAM_SAMPLE_DESC           SampleDesc;
   } pipelineStateStream;

   // Create a color buffer with sRGB for gamma correction.
   DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
   DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT;

   // Check the best multisample quality level that can be used for the given back buffer format.
   DXGI_SAMPLE_DESC sampleDesc = m_Device->GetMultisampleQualityLevels(backBufferFormat);

   D3D12_RT_FORMAT_ARRAY rtvFormats = {};
   rtvFormats.NumRenderTargets = 1;
   rtvFormats.RTFormats[0] = backBufferFormat;

   pipelineStateStream.pRootSignature = m_RootSignature->GetD3D12RootSignature().Get();
   pipelineStateStream.InputLayout = VertexPositionNormalTangentBitangentTexture::InputLayout;
   pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
   pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
   pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
   pipelineStateStream.DSVFormat = depthBufferFormat;
   pipelineStateStream.RTVFormats = rtvFormats;
   pipelineStateStream.SampleDesc = sampleDesc;

   m_PipelineState = m_Device->CreatePipelineStateObject(pipelineStateStream);

   // For the unlit PSO, only the pixel shader is different.
   pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(unlitPixeShaderlBlob.Get());

   m_UnlitPipelineState = m_Device->CreatePipelineStateObject(pipelineStateStream);

   // Create an off-screen render target with a single color buffer and a depth buffer.
   auto colorDesc = CD3DX12_RESOURCE_DESC::Tex2D(backBufferFormat, m_Width, m_Height, 1, 1, sampleDesc.Count,
      sampleDesc.Quality, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
   D3D12_CLEAR_VALUE colorClearValue;
   colorClearValue.Format = colorDesc.Format;
   colorClearValue.Color[0] = 0.4f;
   colorClearValue.Color[1] = 0.6f;
   colorClearValue.Color[2] = 0.9f;
   colorClearValue.Color[3] = 1.0f;

   auto colorTexture = m_Device->CreateTexture(colorDesc, &colorClearValue);
   colorTexture->SetName(L"Color Render Target");

   // Create a depth buffer.
   auto depthDesc = CD3DX12_RESOURCE_DESC::Tex2D(depthBufferFormat, m_Width, m_Height, 1, 1, sampleDesc.Count,
      sampleDesc.Quality, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
   D3D12_CLEAR_VALUE depthClearValue;
   depthClearValue.Format = depthDesc.Format;
   depthClearValue.DepthStencil = { 1.0f, 0 };

   auto depthTexture = m_Device->CreateTexture(depthDesc, &depthClearValue);
   depthTexture->SetName(L"Depth Render Target");

   // Attach the textures to the render target.
   m_RenderTarget.AttachTexture(AttachmentPoint::Color0, colorTexture);
   m_RenderTarget.AttachTexture(AttachmentPoint::DepthStencil, depthTexture);

   commandQueue.Flush();  // Wait for loading operations to complete before rendering the first frame.

   return true;
}

void Tutorial3::UnloadContent()
{
   m_Cube.reset();
   m_Sphere.reset();
   m_Cone.reset();
   m_Torus.reset();
   m_Plane.reset();

   m_DefaultTexture.reset();
   m_DirectXTexture.reset();
   m_EarthTexture.reset();
   m_MonaLisaTexture.reset();

   m_RenderTarget.Reset();

   m_RootSignature.reset();
   m_PipelineState.reset();

   m_GUI.reset();
   m_SwapChain.reset();
   m_Device.reset();
}

void Tutorial3::OnUpdate(UpdateEventArgs& e)
{
   static uint64_t frameCount = 0;
   static double totalTime = 0.0;

   totalTime += e.DeltaTime;
   frameCount++;

   if (totalTime > 1.0)
   {
      double fps = frameCount / totalTime;
      wchar_t buffer[256];
      ::swprintf_s(buffer, L"Textures [FFS: %f]", fps);
      m_Window->SetWindowTitle(buffer);
      frameCount = 0;
      totalTime = 0.0;
   }

   m_SwapChain->WaitForSwapChain();

   // Camera
   float speedMultipler = (m_Shift ? 16.0f : 4.0f);
   XMVECTOR cameraTranslate = XMVectorSet(m_Right - m_Left, 0.0f, m_Forward - m_Backward, 1.0f) *
      speedMultipler * static_cast<float>(e.DeltaTime);
   XMVECTOR cameraPan = XMVectorSet(0.0f, m_Up - m_Down, 0.0f, 1.0f) * 
      speedMultipler * static_cast<float>(e.DeltaTime);
   m_Camera.Translate(cameraTranslate, Space::Local);
   m_Camera.Translate(cameraPan, Space::Local);

   XMVECTOR careraRotation = XMQuaternionRotationRollPitchYaw(
      XMConvertToRadians(m_Pitch), XMConvertToRadians(m_Yaw), 0.0f);
   m_Camera.set_Rotation(careraRotation);

   XMMATRIX viewMatrix = m_Camera.get_ViewMatrix();

   const int numPointLights = 4;
   const int numSpotLights = 4;
   static const XMVECTORF32 LightColors[] =
   {
      Colors::White, Colors::Orange, Colors::Yellow, Colors::Green,
      Colors::Blue, Colors::Indigo, Colors::Violet, Colors::White
   };

   static float lightAnimTime = 0.0f;
   if (m_AnimateLights)
   {
      lightAnimTime += static_cast<float>(e.DeltaTime) * 0.5f * XM_PI;
   }

   const float radius = 8.0f;
   const float offset = 2.0f * XM_PI / numPointLights;
   const float offset2 = offset + (offset / 2.0f);
   
   m_PointLights.resize(numPointLights);
   for (int i = 0; i < numPointLights; ++i)
   {
      PointLight& l = m_PointLights[i];

      l.PositionWS =
      {
         static_cast<float>(std::sin(lightAnimTime + offset * i)) * radius,
         9.0f,
         static_cast<float>(std::cos(lightAnimTime + offset * i)) * radius,
         1.0f
      };

      XMVECTOR positionWS = XMLoadFloat4(&l.PositionWS);
      XMVECTOR positionVS = XMVector3TransformCoord(positionWS, viewMatrix);
      XMStoreFloat4(&l.PositionVS, positionVS);

      l.Color = XMFLOAT4(LightColors[i]);
      l.ConstantAttenuation = 1.0f;
      l.LinearAttenuation = 0.08f;
      l.QuadraticAttenuation = 0.0f;
   }

   m_SpotLights.resize(numSpotLights);
   for (int i = 0; i < numSpotLights; i++)
   {
      SpotLight& l = m_SpotLights[i];

      l.PositionWS =
      {
         static_cast<float>(std::sin(lightAnimTime + offset * i + offset2)) * radius,
         9.0f,
         static_cast<float>(std::cos(lightAnimTime + offset * i + offset2)) * radius,
         1.0f
      };

      XMVECTOR positionWS = XMLoadFloat4(&l.PositionWS);
      XMVECTOR positionVS = XMVector3TransformCoord(positionWS, viewMatrix);
      XMStoreFloat4(&l.PositionVS, positionVS);

      XMVECTOR directionWS = XMVector3Normalize(XMVectorSetW(XMVectorNegate(positionWS), 0));
      XMVECTOR directionVS = XMVector3Normalize(XMVector3TransformNormal(directionWS, viewMatrix));
      XMStoreFloat4(&l.DirectionWS, directionWS);
      XMStoreFloat4(&l.DirectionVS, directionVS);

      l.Color = XMFLOAT4(LightColors[numPointLights + i]);
      l.SpotAngle = XMConvertToRadians(45.0f);
      l.ConstantAttenuation = 1.0f;
      l.LinearAttenuation = 0.08f;
      l.QuadraticAttenuation = 0.0f;
   }

   OnRender();
}

void Tutorial3::OnResize(ResizeEventArgs& e)
{
   m_Width = std::max(1, e.Width);
   m_Height = std::max(1, e.Height);

   m_SwapChain->Resize(m_Width, m_Height);

   float aspectRatio = m_Width / (float)m_Height;
   m_Camera.set_Projection(45.0f, aspectRatio, 0.1f, 100.0f);

   m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(m_Width), static_cast<float>(m_Height));

   m_RenderTarget.Resize(m_Width, m_Height);
}

void XM_CALLCONV ComputeMatrices(FXMMATRIX model, CXMMATRIX view, CXMMATRIX viewProjection, Mat& mat)
{
   mat.ModeMatrix = model;
   mat.ModelViewMatrix = model * view;
   mat.InverseTransposeModelViewMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, mat.ModelViewMatrix));
   mat.ModelViewProjectionMatrix = model * viewProjection;
}

void Tutorial3::OnRender()
{
   auto& commandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
   auto commandList = commandQueue.GetCommandList();

   SceneVisitor visitor(*commandList);

   {
      FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
      commandList->ClearTexture(m_RenderTarget.GetTexture(AttachmentPoint::Color0), clearColor);
      commandList->ClearDepthStencilTexture(m_RenderTarget.GetTexture(AttachmentPoint::DepthStencil), D3D12_CLEAR_FLAG_DEPTH);
   }

   commandList->SetPipelineState(m_PipelineState);
   commandList->SetGraphicsRootSignature(m_RootSignature);

   // Upload lights
   LightProperties lightProgs;
   lightProgs.NumPointLights = static_cast<uint32_t>(m_PointLights.size());
   lightProgs.NumSpotLights = static_cast<uint32_t>(m_SpotLights.size());
   
   commandList->SetGraphics32BitConstants(RootParameters::LightPropertiesCB, lightProgs);
   commandList->SetGraphicsDynamicStructuredBuffer(RootParameters::PointLights, m_PointLights);
   commandList->SetGraphicsDynamicStructuredBuffer(RootParameters::SpotLights, m_SpotLights);

   commandList->SetViewport(m_Viewport);
   commandList->SetScissorRect(m_ScissorRect);

   commandList->SetRenderTarget(m_RenderTarget);

   XMMATRIX viewMatrix = m_Camera.get_ViewMatrix();
   XMMATRIX viewProjectionMatrix = viewMatrix * m_Camera.get_ProjectionMatrix();

   // Sphere --------------
   XMMATRIX translationMatrix = XMMatrixTranslation(-4.0f, 2.0f, -4.0f);
   XMMATRIX rotationMatrix = XMMatrixIdentity();
   XMMATRIX scaleMatrix = XMMatrixScaling(4.0f, 4.0f, 4.0f);
   XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

   Mat matrices;
   ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

   commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
   commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, Material::White);
   commandList->SetShaderResourceView(RootParameters::Textures, 0, m_EarthTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

   m_Sphere->Accept(visitor);

   // Cube --------------
   translationMatrix = XMMatrixTranslation(4.0f, 4.0f, 4.0f);
   rotationMatrix = XMMatrixRotationY(45.0f);
   scaleMatrix = XMMatrixScaling(4.0f, 4.0f, 4.0f);
   worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

   ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

   commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
   commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, Material::White);
   commandList->SetShaderResourceView(RootParameters::Textures, 0, m_MonaLisaTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

   m_Cube->Accept(visitor);


   commandList->SetShaderResourceView(RootParameters::Textures, 0, m_DefaultTexture,
      D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

   commandList->SetPipelineState(m_UnlitPipelineState);

   MaterialProperties lightMaterial = Material::Zero;

   for (const auto& l : m_PointLights)
   {
      lightMaterial.Emissive = l.Color;
      XMVECTOR lightPos = XMLoadFloat4(&l.PositionWS);
      worldMatrix = XMMatrixTranslationFromVector(lightPos);
      ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

      commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
      commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, lightMaterial);

      m_Sphere->Accept(visitor);
   }

   for (const auto& l : m_SpotLights)
   {
      lightMaterial.Emissive = l.Color;
      XMVECTOR lightPos = XMLoadFloat4(&l.PositionWS);
      XMVECTOR lightDir = XMLoadFloat4(&l.DirectionWS);
      XMVECTOR up = XMVectorSet(0, 1, 0, 0);

      // Rotate the cone so it is facing the Z axis.
      rotationMatrix = XMMatrixRotationX(XMConvertToRadians(-90.0f));
      worldMatrix = rotationMatrix * LookAtMatrix(lightPos, lightDir, up);

      ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

      commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
      commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, lightMaterial);

      m_Cone->Accept(visitor);
   }

   auto& swapChainRT = m_SwapChain->GetRenderTarget();
   auto swapChainBackBuffer = swapChainRT.GetTexture(AttachmentPoint::Color0);
   auto msaaRenderTarget = m_RenderTarget.GetTexture(AttachmentPoint::Color0);

   commandList->ResolveSubresource(swapChainBackBuffer, msaaRenderTarget);

   OnGUI(commandList, swapChainRT);

   commandQueue.ExecuteCommandList(commandList);

   m_SwapChain->Present();
}

void Tutorial3::OnGUI(const std::shared_ptr<CommandList>& commandList, const RenderTarget& renderTarget)
{
   m_GUI->NewFrame();

   m_GUI->Render(commandList, renderTarget);
}

static bool g_AllowFullscreenToggle = true;

void Tutorial3::OnKeyPressed(KeyEventArgs& e)
{
   if (!ImGui::GetIO().WantCaptureKeyboard)
   {
      switch (e.Key)
      {
         case KeyCode::Escape:
            AppWorks::Get().Stop();
            break;
         case KeyCode::Enter:
            if (e.Alt)
            {
         case KeyCode::F11:
            if (g_AllowFullscreenToggle)
            {
               m_Window->ToggleFullScreen();
               g_AllowFullscreenToggle = false;
            }
            break;
            }
         case KeyCode::V:
            m_SwapChain->ToggleVSync();
            break;
         case KeyCode::R:
            // Reset camera transform
            m_Camera.set_Translation(m_pAlignedCameraData->m_InitialCamPos);
            m_Camera.set_Rotation(m_pAlignedCameraData->m_InitialCamRot);
            m_Pitch = 0.0f;
            m_Yaw = 0.0f;
            break;
         case KeyCode::Up:
         case KeyCode::W:
            m_Forward = 1.0f;
            break;
         case KeyCode::Left:
         case KeyCode::A:
            m_Left = 1.0f;
            break;
         case KeyCode::Down:
         case KeyCode::S:
            m_Backward = 1.0f;
            break;
         case KeyCode::Right:
         case KeyCode::D:
            m_Right = 1.0f;
            break;
         case KeyCode::Q:
            m_Down = 1.0f;
            break;
         case KeyCode::E:
            m_Up = 1.0f;
            break;
         case KeyCode::Space:
            m_AnimateLights = !m_AnimateLights;
            break;
         case KeyCode::ShiftKey:
            m_Shift = true;
            break;
      }
   }
}

void Tutorial3::OnKeyReleased(KeyEventArgs& e)
{
   switch (e.Key)
   {
      case KeyCode::Enter:
         if (e.Alt)
         {
      case KeyCode::F11:
         g_AllowFullscreenToggle = true;
         }
         break;
      case KeyCode::Up:
      case KeyCode::W:
         m_Forward = 0.0f;
         break;
      case KeyCode::Left:
      case KeyCode::A:
         m_Left = 0.0f;
         break;
      case KeyCode::Down:
      case KeyCode::S:
         m_Backward = 0.0f;
         break;
      case KeyCode::Right:
      case KeyCode::D:
         m_Right = 0.0f;
         break;
      case KeyCode::Q:
         m_Down = 0.0f;
         break;
      case KeyCode::E:
         m_Up = 0.0f;
         break;
      case KeyCode::ShiftKey:
         m_Shift = false;
         break;
   }
}

void Tutorial3::OnMouseMoved(MouseMotionEventArgs& e)
{
   const float mouseSpeed = 0.1f;

   if (!ImGui::GetIO().WantCaptureMouse)
   {
      if (e.LeftButton)
      {
         m_Pitch -= e.RelY * mouseSpeed;

         m_Pitch = std::clamp(m_Pitch, -90.0f, 90.0f);

         m_Yaw -= e.RelX * mouseSpeed;
      }
   }
}

void Tutorial3::OnMouseWheel(MouseWheelEventArgs& e)
{
   if (!ImGui::GetIO().WantCaptureMouse)
   {
      auto fov = m_Camera.get_FoV();

      fov -= e.WheelDelta;
      fov = std::clamp(fov, 12.0f, 90.0f);

      m_Camera.set_FoV(fov);

      //m_Logger->info("FoV: {:.7}", fov);
   }
}

