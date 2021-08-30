#pragma once

/*
 *  Copyright(c) 2019 Jeremiah van Oosten
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

 /**
  *  @file SceneNode.h
  *  @date May 13, 2019
  *  @author Jeremiah van Oosten
  *
  *  @brief A node in a scene graph.
  */

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Mesh;
class CommandList;
class Visitor;

class SceneNode : public std::enable_shared_from_this<SceneNode>
{
public:
   explicit SceneNode(const DirectX::XMMATRIX& localTransform = DirectX::XMMatrixIdentity());
   virtual ~SceneNode();

   const std::string& GetName() const;
   void SetName(const std::string& name);

   DirectX::XMMATRIX GetLocalTransform() const;
   void SetLocalTransform(const DirectX::XMMATRIX& localTransform);
   DirectX::XMMATRIX GetInverseTransform() const;

   DirectX::XMMATRIX GetWorldTransform() const;

   DirectX::XMMATRIX GetInverseWorldTransform() const;

   void AddChild(std::shared_ptr<SceneNode> childNode);
   void RemoveChild(std::shared_ptr<SceneNode> childNode);
   void SetParent(std::shared_ptr<SceneNode> parentNode);

   size_t AddMesh(std::shared_ptr<Mesh> mesh);
   void RemoveMesh(std::shared_ptr<Mesh> mesh);

   std::shared_ptr<Mesh> GetMesh(size_t index = 0);

   const DirectX::BoundingBox& GetAABB() const;

   void Accept(Visitor& visitor);

protected:
   DirectX::XMMATRIX GetParentWorldTransform() const;

private:
   std::string m_Name;

   struct alignas(16) AlignedData
   {
      DirectX::XMMATRIX m_LocalTransform;
      DirectX::XMMATRIX m_InversrTransform;
   } * m_AlignedData;

   std::weak_ptr<SceneNode> m_ParentNode;
   std::vector<std::shared_ptr<SceneNode>> m_Children;
   std::multimap<std::string, std::shared_ptr<SceneNode>> m_ChildrenByName;
   std::vector<std::shared_ptr<Mesh>> m_Meshes;

   DirectX::BoundingBox m_AABB;
};

