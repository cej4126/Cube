#pragma once

/*
 *  Copyright(c) 2018 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */
#include <DirectXMath.h>
#include <d3d12.h>
#include <cstdint>
#include <memory>  // for std::shared_ptr
#include <vector>

class Texture;

enum AttachmentPoint
{
   Color0,
   Color1,
   Color2,
   Color3,
   Color4,
   Color5,
   Color6,
   Color7,
   DepthStencil,
   NumAttachmentPoints
};

class RenderTarget
{
public:
   RenderTarget();

   RenderTarget(const RenderTarget& copy) = default;
   RenderTarget(RenderTarget&& copy) = default;

   RenderTarget& operator=(const RenderTarget& other) = default;
   RenderTarget& operator=(RenderTarget&& other) = default;

   void AttachTexture(AttachmentPoint attachmentPoint, std::shared_ptr<Texture> texture);
   std::shared_ptr<Texture> GetTexture(AttachmentPoint attachmentPoint) const;

   void Resize(DirectX::XMUINT2 size);
   void Resize(uint32_t width, uint32_t height);
   DirectX::XMUINT2 GetSize() const;
   uint32_t GetWidth() const;
   uint32_t GetHeight() const;

   D3D12_VIEWPORT GetViewport(DirectX::XMFLOAT2 scale = { 1.0f, 1.0f },
      DirectX::XMFLOAT2 bias = { 0.0f, 0.0f },
      float minDepth = 0.0f, float maxDepth = 1.0f);

   const std::vector<std::shared_ptr<Texture>>& GetTextures() const;
   D3D12_RT_FORMAT_ARRAY GetRenderTargetFormats() const;
   DXGI_FORMAT GetDepthStencilFormat() const;

   DXGI_SAMPLE_DESC GetSampleDesc() const;

   void Reset()
   {
      m_Textures = std::vector<std::shared_ptr<Texture>>(AttachmentPoint::NumAttachmentPoints);
   }

private:
   std::vector<std::shared_ptr<Texture>> m_Textures;
   DirectX::XMUINT2 m_Size;

};

