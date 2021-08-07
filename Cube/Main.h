#pragma once

#include <DirectXMath.h>
#include "Window.h"
#include "AppWorks.h"
#include "Device.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "SwapChain.h"
#include "PipelineStateObject.h"
#include "RootSignature.h"
using namespace DirectX;

std::shared_ptr<Device> m_Device = nullptr;
std::shared_ptr<VertexBuffer> m_VertexBuffer = nullptr;
std::shared_ptr<IndexBuffer> m_IndexBuffer = nullptr;
std::shared_ptr<Window> m_AppWindow = nullptr;
std::shared_ptr<SwapChain> m_SwapChain = nullptr;
std::shared_ptr<Texture> m_DepthTexture = nullptr;
std::shared_ptr<PipelineStateObject> m_PipelineStateObject = nullptr;
std::shared_ptr< RootSignature> m_RootSignature = nullptr;

struct VertexPosColor
{
   XMFLOAT3 Position;
   XMFLOAT3 Color;
};

static VertexPosColor m_vertices[8] =
{
   { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)}, // 0
   { XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)}, // 1
   { XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f)}, // 2
   { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)}, // 3
   { XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)}, // 4
   { XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f)}, // 5
   { XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)}, // 6
   { XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f)}, // 7
};

static WORD m_Indicies[36] =
{ 0, 1, 2,
   0, 2, 3,
   4, 6, 5,
   4, 7, 6,
   4, 5, 1,
   4, 1, 0,
   3, 2, 6,
   3, 6, 7,
   1, 5, 6,
   1, 6, 2,
   4, 0, 3,
   4, 3, 7
};

float m_fieldOfView = 45.0f;