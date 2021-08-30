#include "Material.h"
#include "Texture.h"

static MaterialProperties* NewMaterialProperties(const MaterialProperties& props)
{
   MaterialProperties* materialProperties = (MaterialProperties*)_aligned_malloc(sizeof(MaterialProperties), 16);
   *materialProperties = props;
   return materialProperties;
}

static void DeleteMaterialProperties(MaterialProperties* p)
{
   _aligned_free(p);
}

Material::Material(const MaterialProperties& materialProperties) :
   m_MaterialProperties(NewMaterialProperties(materialProperties), &DeleteMaterialProperties)
{}

Material::Material(const Material& copy) :
   m_MaterialProperties(NewMaterialProperties(*copy.m_MaterialProperties), &DeleteMaterialProperties),
   m_Textures(copy.m_Textures)
{
}

const MaterialProperties Material::Zero =
{
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    0.0f,
    { 0.0f, 0.0f, 0.0f, 1.0f }
};

const MaterialProperties Material::White =
{
    { 1.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
    128.0f,
    { 0.1f, 0.1f, 0.1f, 1.0f }
};