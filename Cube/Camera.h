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

 /**
  *  @file Camera.h
  *  @date October 24, 2018
  *  @author Jeremiah van Oosten
  *
  *  @brief DirectX 12 Camera class.
  */

#include <DirectXMath.h>

enum class Space
{
   Local,
   World,
};

class Camera
{
public:
   Camera();
   virtual ~Camera();

   void XM_CALLCONV set_LookAt(DirectX::FXMVECTOR eye, DirectX::FXMVECTOR target, DirectX::FXMVECTOR up);
   DirectX::XMMATRIX get_ViewMatrix() const;
   DirectX::XMMATRIX get_InverseViewMatrix() const;

   void set_Projection(float fovy, float aspect, float zNear, float zFar);
   DirectX::XMMATRIX get_ProjectionMatrix() const;
   DirectX::XMMATRIX get_InverseProjectionMatrix() const;

   void set_FoV(float fovy);
   float get_FoV() const;

   void XM_CALLCONV set_Translation(DirectX::FXMVECTOR translation);
   DirectX::FXMVECTOR get_Translation() const;

   void XM_CALLCONV set_Rotation(DirectX::FXMVECTOR rotation);
   DirectX::FXMVECTOR get_Rotation() const;

   void XM_CALLCONV Translate(DirectX::FXMVECTOR translation, Space space = Space::Local);
   void Rotate(DirectX::FXMVECTOR quaternion);

protected:
   virtual void UpdateViewMatrix() const;
   virtual void UpdateInverseViewMatrix() const;
   virtual void UpdateProjectionMatrix() const;
   virtual void UpdateInverseProjectionMatrix() const;

   // This data must be aligned otherwise the SSE intrinsics fail
   // and throw exceptions.
   __declspec(align(16)) struct AlignedData
   {
      DirectX::XMVECTOR m_Translation;
      DirectX::XMVECTOR m_Rotation;
      DirectX::XMMATRIX m_ViewMatrix;
      DirectX::XMMATRIX m_InverseViewMatrix;
      DirectX::XMMATRIX m_ProjectionMatrix;
      DirectX::XMMATRIX m_InverseProjectionMatrix;
   };
   AlignedData* p_Data;

   float m_vFoV;
   float m_AspectRatio;
   float m_zNear;
   float m_zFar;

   mutable bool m_ViewDirty;
   mutable bool m_InverseViewDirty;
   mutable bool m_ProjectionDirty;
   mutable bool m_InverseProjectionDirty;
};

