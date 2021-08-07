#include "RenderTarget.h"
#include "Texture.h"

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
