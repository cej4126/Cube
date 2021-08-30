#pragma once

#include <DirectXMath.h>
#include <map>
#include <memory>

class Texture;

struct alignas(16) MaterialProperties
{
   MaterialProperties(
      const DirectX::XMFLOAT4 diffuse = { 1.0f, 1.0f, 1.0f, 1.0f },
      const DirectX::XMFLOAT4 specular = { 1.0f, 1.0f, 1.0f, 1.0f },
      const float specularPower = 128.0f,
      const DirectX::XMFLOAT4 ambient = { 0.0f, 0.0f, 0.0f, 1.0f },
      const DirectX::XMFLOAT4 emissive = { 0.0f, 0.0f, 0.0f, 1.0f },
      const DirectX::XMFLOAT4 reflectance = { 0.0f, 0.0f, 0.0f, 0.0f },
      const float opacity = 1.0f,
      const float indexOfRefrection = 0.0f,
      const float bumpIntensity = 1.0f,
      const float alphaThreshold = 0.1f) :
      Diffuse(diffuse),
      Specular(specular),
      Emissive(emissive),
      Ambient(ambient),
      Reflectance(reflectance),
      Opacity(opacity),
      SpecularPower(specularPower),
      IndexOfRefraction(indexOfRefrection),
      BumpIntensity(bumpIntensity),
      HasAmbientTexture(false),
      HasEmissiveTexture(false),
      HasDiffuseTexture(false),
      HasSpecularTexture(false),
      HasSpecularPowerTexture(false),
      HasNormalTexture(false),
      HasBumpTexture(false),
      HasOpacityTexture(false)
   {}

   DirectX::XMFLOAT4 Diffuse;
   DirectX::XMFLOAT4 Specular;
   DirectX::XMFLOAT4 Emissive;
   DirectX::XMFLOAT4 Ambient;
   DirectX::XMFLOAT4 Reflectance;
   float Opacity;
   float SpecularPower;
   float IndexOfRefraction;
   float BumpIntensity;
   uint32_t HasAmbientTexture;
   uint32_t HasEmissiveTexture;
   uint32_t HasDiffuseTexture;
   uint32_t HasSpecularTexture;

   uint32_t HasSpecularPowerTexture;
   uint32_t HasNormalTexture;
   uint32_t HasBumpTexture;
   uint32_t HasOpacityTexture;
};

class Material
{
public:
   enum class TextureType
   {
      Ambient,
      Emissive,
      Diffuse,
      Specular,
      SpecularPower,
      Normal,
      Bump,
      Opacity,
      NumTypes,
   };

   Material(const MaterialProperties& materialProperties = MaterialProperties());
   Material(const Material& copy);
   ~Material() = default;

   static const MaterialProperties Zero;
   static const MaterialProperties White;

private:
   std::unique_ptr <MaterialProperties, void(*)(MaterialProperties*)> m_MaterialProperties;
   std::map<TextureType, std::shared_ptr<Texture>> m_Textures;
};
