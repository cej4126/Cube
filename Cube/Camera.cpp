#include "Camera.h"

using namespace DirectX;

Camera::Camera() :
m_ViewDirty(true),
m_InverseViewDirty(true),
m_ProjectionDirty(true),
m_InverseProjectionDirty(true),
m_vFoV(45.0f),
m_AspectRatio(1.0f),
m_zNear(0.1f),
m_zFar(100.0f)
{
   p_Data = (AlignedData*) _aligned_malloc(sizeof(AlignedData), 16);
   p_Data->m_Translation = XMVectorZero();
   p_Data->m_Rotation = XMQuaternionIdentity();
}

Camera::~Camera()
{
   _aligned_free(p_Data);
}

void XM_CALLCONV Camera::set_LookAt(DirectX::FXMVECTOR eye, DirectX::FXMVECTOR target, DirectX::FXMVECTOR up)
{
   p_Data->m_ViewMatrix = XMMatrixLookAtLH(eye, target, up);
   p_Data->m_Translation = eye;
   p_Data->m_Rotation = XMQuaternionRotationMatrix(XMMatrixTranspose(p_Data->m_ViewMatrix));

   m_InverseViewDirty = true;
   m_ViewDirty = true;
}

DirectX::XMMATRIX Camera::get_ViewMatrix() const
{
   if (m_ViewDirty)
   {
      UpdateViewMatrix();
   }
   return p_Data->m_ViewMatrix;
}

DirectX::XMMATRIX Camera::get_InverseViewMatrix() const
{
   if (m_InverseViewDirty)
   {
      UpdateInverseViewMatrix();
   }
   return p_Data->m_InverseViewMatrix;
}

void Camera::set_Projection(float fovy, float aspect, float zNear, float zFar)
{
   m_vFoV = fovy;
   m_AspectRatio = aspect;
   m_zNear = zNear;
   m_zFar = zFar;

   m_ProjectionDirty = true;
   m_InverseProjectionDirty = true;
}

DirectX::XMMATRIX Camera::get_ProjectionMatrix() const
{
   if (m_ProjectionDirty)
   {
      UpdateProjectionMatrix();
   }
   return p_Data->m_ProjectionMatrix;
}

DirectX::XMMATRIX Camera::get_InverseProjectionMatrix() const
{
   if (m_InverseProjectionDirty)
   {
      UpdateInverseProjectionMatrix();
   }
   return p_Data->m_InverseProjectionMatrix;
}

void Camera::set_FoV(float fovy)
{
   if (m_vFoV != fovy)
   {
      m_vFoV = fovy;
      m_ProjectionDirty = true;
      m_InverseProjectionDirty = true;
   }
}

float Camera::get_FoV() const
{
   return m_vFoV;
}

void XM_CALLCONV Camera::set_Translation(DirectX::FXMVECTOR translation)
{
   p_Data->m_Translation = translation;
   m_ViewDirty = true;
}

DirectX::FXMVECTOR Camera::get_Translation() const
{
   return p_Data->m_Translation;
}

void XM_CALLCONV Camera::set_Rotation(DirectX::FXMVECTOR rotation)
{
   p_Data->m_Rotation = rotation;
}

DirectX::FXMVECTOR Camera::get_Rotation() const
{
   return p_Data->m_Rotation;
}

void XM_CALLCONV Camera::Translate(DirectX::FXMVECTOR translation, Space space)
{
   switch (space)
   {
      case Space::Local:
         p_Data->m_Translation += XMVector3Rotate(translation, p_Data->m_Rotation);
         break;
      case Space::World:
         p_Data->m_Translation = translation;
         break;
      default:
         break;
   }
   p_Data->m_Translation = XMVectorSetW(p_Data->m_Translation, 1.0f);

   m_ViewDirty = true;
   m_InverseViewDirty = true;
}

void Camera::Rotate(DirectX::FXMVECTOR quaternion)
{
   p_Data->m_Rotation = XMQuaternionMultiply(quaternion, p_Data->m_Rotation);

   m_ViewDirty = true;
   m_InverseViewDirty = true;

}

void Camera::UpdateViewMatrix() const
{
   XMMATRIX rotationMatrix = XMMatrixTranspose(XMMatrixRotationQuaternion(p_Data->m_Rotation));
   XMMATRIX translationMatrix = XMMatrixTranslationFromVector(-(p_Data->m_Translation));

   p_Data->m_ViewMatrix = translationMatrix * rotationMatrix;

   m_InverseViewDirty = true;
   m_ViewDirty = false;
}

void Camera::UpdateInverseViewMatrix() const
{
   if (m_ViewDirty)
   {
      UpdateViewMatrix();
   }

   p_Data->m_InverseViewMatrix = XMMatrixInverse(nullptr, p_Data->m_ViewMatrix);
   m_InverseViewDirty = false;
}

void Camera::UpdateProjectionMatrix() const
{
   p_Data->m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_vFoV), m_AspectRatio, m_zNear, m_zFar);

   m_ProjectionDirty = false;
   m_InverseProjectionDirty = true;
}

void Camera::UpdateInverseProjectionMatrix() const
{
   if (m_ProjectionDirty)
   {
      UpdateProjectionMatrix();
   }

   p_Data->m_InverseProjectionMatrix = XMMatrixInverse(nullptr, p_Data->m_ProjectionMatrix);
   m_InverseProjectionDirty = false;
}
