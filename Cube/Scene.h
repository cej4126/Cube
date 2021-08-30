#pragma once

#include <DirectXCollision.h>

#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <string>

class aiMaterial;
class aiMesh;
class aiNode;
class aiScene;

class CommandList;
class Device;
class SceneNode;
class Mesh;
class Material;
class Visitor;

class Scene
{
public:
   Scene() = default;
   ~Scene() = default;

   void SetRootNode(std::shared_ptr<SceneNode> node)
   {
      m_RootNode = node;
   }

   std::shared_ptr<SceneNode> GetRootNode() const
   {
      return m_RootNode;
   }

   DirectX::BoundingBox GetAABB() const;

   virtual void Accept(Visitor& visitor);

protected:
   friend class CommandList;

   bool LoadSceneFromFile(CommandList& commandList,
      const std::wstring& fileName,
      const std::function<bool(float)>& loadingProgress);

   bool LoadSceneFromString(CommandList& commandList,
      const std::string& sceneStr,
      const std::string& format);

private:
   void ImportScene(CommandList& commandList, const aiScene& scene, std::filesystem::path parentPath);
   void ImportMaterial(CommandList& commandList, const aiMaterial& material, std::filesystem::path parentPath);
   void ImportMesh(CommandList& commandList, const aiMesh& mesh);
   std::shared_ptr<SceneNode> ImportSceneNode(CommandList& commandList, std::shared_ptr<SceneNode> parent, const aiNode* aiNode);

   std::map<std::string, std::shared_ptr<Material>> m_MaterialMap;
   std::vector<std::shared_ptr<Material>> m_Materials;
   std::vector <std::shared_ptr<Mesh>> m_Meshes;

   std::shared_ptr<SceneNode> m_RootNode;
   std::wstring m_SceneFile;
};

