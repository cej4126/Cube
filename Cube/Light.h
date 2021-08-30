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
  *  @file Light.h
  *  @date October 24, 2018
  *  @author Jeremiah van Oosten
  *
  *  @brief Light structures that use HLSL constant buffer padding rules.
  */

#include <DirectXMath.h>

// WS - World Space, VS View Space

struct PointLight
{
   PointLight() :
      PositionWS(0.0f, 0.0f, 0.0f, 1.0f),
      PositionVS(0.0f, 0.0f, 0.0f, 1.0f),
      Color(1.0f, 1.0f, 1.0f, 1.0f),
      ConstantAttenuation(1.0f),
      LinearAttenuation(0.0f),
      QuadraticAttenuation(0.0f),
      Padding(0.0f)
   {}

   DirectX::XMFLOAT4 PositionWS;
   DirectX::XMFLOAT4 PositionVS;
   DirectX::XMFLOAT4 Color;
   float ConstantAttenuation;
   float LinearAttenuation;
   float QuadraticAttenuation;
   float Padding;
};

struct SpotLight
{
   SpotLight() :
      PositionWS(0.0f, 0.0f, 0.0f, 1.0f),
      PositionVS(0.0f, 0.0f, 0.0f, 1.0f),
      DirectionWS(0.0f, 0.0f, 1.0f, 0.0f),
      DirectionVS(0.0f, 0.0f, 1.0f, 0.0f),
      Color(1.0f, 1.0f, 1.0f, 1.0f),
      SpotAngle(DirectX::XM_PIDIV2),
      ConstantAttenuation(1.0f),
      LinearAttenuation(0.0f),
      QuadraticAttenuation(0.0f)
   {}

   DirectX::XMFLOAT4 PositionWS;
   DirectX::XMFLOAT4 PositionVS;
   DirectX::XMFLOAT4 DirectionWS;
   DirectX::XMFLOAT4 DirectionVS;
   DirectX::XMFLOAT4 Color;
   float SpotAngle;
   float ConstantAttenuation;
   float LinearAttenuation;
   float QuadraticAttenuation;
};

struct DirectionLight
{
   DirectionLight() :
      DirectionWS(0.0f, 0.0f, 1.0f, 0.0f),
      DirectionVS(0.0f, 0.0f, 1.0f, 0.0f),
      Color(1.0f, 1.0f, 1.0f, 1.0f)
   {}

   DirectX::XMFLOAT4 DirectionWS;
   DirectX::XMFLOAT4 DirectionVS;
   DirectX::XMFLOAT4 Color;
};
