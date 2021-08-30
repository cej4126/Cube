#pragma once
#include <DirectXCollision.h>
#include <DirectXMath.h>

#include <d3d12.h>
#include <map>
#include <memory>

class CommandList;
class IndexBuffer;
class Material;
class VertexBuffer;
class Visitor;

class Mesh
{
public:
   Mesh();
   ~Mesh() = default;

   void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology);
   D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const;

   void SetVertexBuffer(uint32_t slotID, const std::shared_ptr<VertexBuffer>& vertexBuffer);
   std::shared_ptr<VertexBuffer> GetVertexBuffer(uint32_t slotID) const;
   const std::map<uint32_t, std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const
   {
      return m_VertexBuffers;
   }

   void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);
   std::shared_ptr<IndexBuffer> GetIndexBuffer();

   size_t GetIndexCount() const;
   size_t GetVertexCount() const;

   void SetMaterial(std::shared_ptr<Material> material);
   std::shared_ptr<Material> GetMaterial() const;

   void SetAABB(const DirectX::BoundingBox& aabb);
   const DirectX::BoundingBox& GetAABB() const;

   void Draw(CommandList& commandList, uint32_t instanceCount = 1, uint32_t startInstance = 0);

   void Accept(Visitor& visitor);

private:
   std::map<uint32_t, std::shared_ptr<VertexBuffer>> m_VertexBuffers;
   std::shared_ptr<IndexBuffer> m_IndexBuffer;
   std::shared_ptr<Material> m_Material;
   D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;
   DirectX::BoundingBox m_AABB;
};
