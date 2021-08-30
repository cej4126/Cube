#include "Scene.h"
#include "Visitor.h"
#include "SceneNode.h"

DirectX::BoundingBox Scene::GetAABB() const
{
   return DirectX::BoundingBox();
}

void Scene::Accept(Visitor& visitor)
{
   visitor.Visit(*this);
   if (m_RootNode)
   {
      m_RootNode->Accept(visitor);
   }
}

bool Scene::LoadSceneFromFile(CommandList& commandList, const std::wstring& fileName, const std::function<bool(float)>& loadingProgress)
{
   return false;
}

bool Scene::LoadSceneFromString(CommandList& commandList, const std::string& sceneStr, const std::string& format)
{
   return false;
}

void Scene::ImportScene(CommandList& commandList, const aiScene& scene, std::filesystem::path parentPath)
{
}

void Scene::ImportMaterial(CommandList& commandList, const aiMaterial& material, std::filesystem::path parentPath)
{
}

void Scene::ImportMesh(CommandList& commandList, const aiMesh& mesh)
{
}

std::shared_ptr<SceneNode> Scene::ImportSceneNode(CommandList& commandList, std::shared_ptr<SceneNode> parent, const aiNode* aiNode)
{
   return std::shared_ptr<SceneNode>();
}
