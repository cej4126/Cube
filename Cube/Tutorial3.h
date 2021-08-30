#pragma once
#include <memory>   // For std::unique_ptr and std::smart_ptr
#include <string>   // For std::wstring
#include <vector>   // For std::vector
#include "Camera.h"
#include "light.h"
#include "Events.h"
#include "RenderTarget.h"

class GUI;
class Device;
class Window;
class SwapChain;
class RootSignature;
class Scene;
class RenderTarget;
class CommandList;
class Texture;
class PipelineStateObject;

class Tutorial3
{
public:
   Tutorial3(const std::wstring& name, uint32_t width, uint32_t height, bool vSync = false);
   virtual ~Tutorial3();
   uint32_t Run();

   bool LoadContent();
   void UnloadContent();

protected:
   void OnUpdate(UpdateEventArgs& e);

   void OnRender();
   void OnGUI(const std::shared_ptr<CommandList>& commandList, const RenderTarget& renderTarget);

   void OnKeyPressed(KeyEventArgs& e);
   virtual void OnKeyReleased(KeyEventArgs& e);
   virtual void OnMouseMoved(MouseMotionEventArgs& e);
   void OnMouseWheel(MouseWheelEventArgs& e);
   void OnResize(ResizeEventArgs& e);

private:
   std::shared_ptr<Window> m_Window;

   std::shared_ptr<Device> m_Device;
   std::shared_ptr<SwapChain> m_SwapChain;
   std::shared_ptr<GUI> m_GUI;

   std::shared_ptr<Scene> m_Cube;
   std::shared_ptr<Scene> m_Sphere;
   std::shared_ptr<Scene> m_Cone;
   std::shared_ptr<Scene> m_Torus;
   std::shared_ptr<Scene> m_Plane;

   std::shared_ptr<Texture> m_DefaultTexture;
   std::shared_ptr<Texture> m_DirectXTexture;
   std::shared_ptr<Texture> m_EarthTexture;
   std::shared_ptr<Texture> m_MonaLisaTexture;

   RenderTarget m_RenderTarget;

   std::shared_ptr<RootSignature> m_RootSignature;
   std::shared_ptr <PipelineStateObject> m_PipelineState;
   std::shared_ptr <PipelineStateObject> m_UnlitPipelineState;

   D3D12_RECT m_ScissorRect;
   D3D12_VIEWPORT m_Viewport;

   // Camera
   float m_Forward;
   float m_Backward;
   float m_Left;
   float m_Right;
   float m_Up;
   float m_Down;

   float m_Pitch;
   float m_Yaw;
   Camera m_Camera;
   struct alignas(16) CameraData
   {
      DirectX::XMVECTOR m_InitialCamPos;
      DirectX::XMVECTOR m_InitialCamRot;
   };
   CameraData* m_pAlignedCameraData;

   int m_Width;
   int m_Height;
   bool m_VSync;

   bool m_Shift;
   bool m_AnimateLights;

   std::vector<PointLight> m_PointLights;
   std::vector<SpotLight> m_SpotLights;
};


