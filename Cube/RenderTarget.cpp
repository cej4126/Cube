#include "RenderTarget.h"
#include "Texture.h"

using namespace DirectX;

RenderTarget::RenderTarget() :
   m_Textures(AttachmentPoint::NumAttachmentPoints),
   m_Size(0, 0)
{
}

void RenderTarget::AttachTexture(AttachmentPoint attachmentPoint, std::shared_ptr<Texture> texture)
{
   m_Textures[attachmentPoint] = texture;
   if (texture && texture->GetD3D12Resource())
   {
      auto desc = texture->GetD3D12ResourceDesc();

      m_Size.x = static_cast<uint32_t>(desc.Width);
      m_Size.y = static_cast<uint32_t>(desc.Height);
   }
}

std::shared_ptr<Texture> RenderTarget::GetTexture(AttachmentPoint attachmentPoint) const
{
   return m_Textures[attachmentPoint];
}

void RenderTarget::Resize(DirectX::XMUINT2 size)
{
   m_Size = size;
   for (auto texture : m_Textures)
   {
      if (texture)
      {
         texture->Resize(m_Size.x, m_Size.y);
      }
   }
}

void RenderTarget::Resize(uint32_t width, uint32_t height)
{
   Resize(XMUINT2(width, height));
}

DirectX::XMUINT2 RenderTarget::GetSize() const
{
   return m_Size;
}

uint32_t RenderTarget::GetWidth() const
{
   return m_Size.x;
}

uint32_t RenderTarget::GetHeight() const
{
   return m_Size.y;
}

D3D12_VIEWPORT RenderTarget::GetViewport(DirectX::XMFLOAT2 scale, DirectX::XMFLOAT2 bias, float minDepth, float maxDepth)
{
   UINT64 width = 0;
   UINT height = 0;

   for (int i = AttachmentPoint::Color0; i < AttachmentPoint::Color7; i++)
   {
      auto texture = m_Textures[i];
      if (texture)
      {
         auto desc = texture->GetD3D12ResourceDesc();
         if (width < desc.Width)
         {
            width = desc.Width;
         }
         if (height < desc.Height)
         {
            height = desc.Height;
         }
      }
   }

   D3D12_VIEWPORT viewport =
   {
      (width * bias.x),
      (height * bias.y),
      (width * scale.x),
      (height * scale.y),
      minDepth, maxDepth
   };

   return viewport;
}

const std::vector<std::shared_ptr<Texture>>& RenderTarget::GetTextures() const
{
   return m_Textures;
}

D3D12_RT_FORMAT_ARRAY RenderTarget::GetRenderTargetFormats() const
{
   D3D12_RT_FORMAT_ARRAY rtvFormats = {};
   for (int i = AttachmentPoint::Color0; i < AttachmentPoint::Color7; i++)
   {
      auto texture = m_Textures[i];
      if (texture)
      {
         rtvFormats.RTFormats[rtvFormats.NumRenderTargets++] = texture->GetD3D12ResourceDesc().Format;
      }
   }
   return rtvFormats;
}

DXGI_FORMAT RenderTarget::GetDepthStencilFormat() const
{
   DXGI_FORMAT dvsFormat = DXGI_FORMAT_UNKNOWN;
   auto depthStencilTexture = m_Textures[AttachmentPoint::DepthStencil];
   if (depthStencilTexture)
   {
      dvsFormat = depthStencilTexture->GetD3D12ResourceDesc().Format;
   }
   return dvsFormat;
}

DXGI_SAMPLE_DESC RenderTarget::GetSampleDesc() const
{
   DXGI_SAMPLE_DESC sampleDesc = { 1, 0 };
   for (int i = AttachmentPoint::Color0; i < AttachmentPoint::Color7; i++)
   {
      auto texture = m_Textures[i];
      if (texture)
      {
         sampleDesc = texture->GetD3D12ResourceDesc().SampleDesc;
         break;
      }
   }
   return sampleDesc;
}
