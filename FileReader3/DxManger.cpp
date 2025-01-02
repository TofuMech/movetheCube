#include "DXManager.h"
#include <iostream>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "Controls.h"

const char* vertexShaderSource = R"(
cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

struct VS_INPUT
{
    float4 Pos : POSITION;
    float4 Color : COLOR;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Color = input.Color;
    return output;
}
)";

const char* pixelShaderSource = R"(
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

float4 PS(PS_INPUT input) : SV_Target
{
    return input.Color;
}
)";

DXManager::DXManager() :
    m_device(nullptr),
    m_deviceContext(nullptr),
    m_swapChain(nullptr),
    m_renderTargetView(nullptr),
    m_depthStencilView(nullptr),
    m_depthStencilBuffer(nullptr),
    m_vertexShader(nullptr),
    m_pixelShader(nullptr),
    m_vertexLayout(nullptr),
    m_vertexBuffer(nullptr),
    m_indexBuffer(nullptr),
    m_constantBuffer(nullptr),
    m_worldMatrix(XMMatrixIdentity()),
    m_viewMatrix(XMMatrixIdentity()),
    m_projectionMatrix(XMMatrixIdentity()) {
}

DXManager::~DXManager() {
    Cleanup();
}

void DXManager::OnKeyDown(WPARAM key) {
    m_character.OnKeyDown(key);
}

void DXManager::OnKeyUp(WPARAM key) {
    m_character.OnKeyUp(key);
}

bool DXManager::Initialize(HWND hwnd) {
    if (!InitDevice(hwnd)) {
        std::cerr << "Failed to initialize device." << std::endl;
        return false;
    }
    if (!InitShaders()) {
        std::cerr << "Failed to initialize shaders." << std::endl;
        return false;
    }
    if (!InitGeometry()) {
        std::cerr << "Failed to initialize geometry." << std::endl;
        return false;
    }

    XMVECTOR Eye = XMVectorSet(0.0f, 2.0f, -10.0f, 0.0f);  
    XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    m_viewMatrix = XMMatrixLookAtLH(Eye, At, Up);

    RECT rc;
    GetClientRect(hwnd, &rc);
    float width = static_cast<float>(rc.right - rc.left);
    float height = static_cast<float>(rc.bottom - rc.top);
    m_projectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / height, 0.01f, 100.0f);

    return true;
}

void DXManager::Update(float deltaTime) {
    m_character.Update(deltaTime);

    XMFLOAT3 position = m_character.GetPosition();
    float rotation = m_character.GetRotation();  

    XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(rotation));

    XMMATRIX translationMatrix = XMMatrixTranslation(position.x, position.y, position.z);

    m_worldMatrix = rotationMatrix * translationMatrix;
}

void DXManager::Render() {
    float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f }; 
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    struct MatrixBuffer {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

    MatrixBuffer cb;
    cb.world = XMMatrixTranspose(m_worldMatrix);
    cb.view = XMMatrixTranspose(m_viewMatrix);
    cb.projection = XMMatrixTranspose(m_projectionMatrix);

    m_deviceContext->UpdateSubresource(m_constantBuffer, 0, nullptr, &cb, 0, 0);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    m_deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
    m_deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
    m_deviceContext->IASetInputLayout(m_vertexLayout);

    m_deviceContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);

    m_deviceContext->DrawIndexed(36, 0, 0); // 36 indices for a cube (6 faces * 2 triangles * 3 vertices)

    m_swapChain->Present(0, 0);
}

bool DXManager::InitGeometry() {
    Vertex vertices[] = {
        { XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },  
        { XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },

        { XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }
    };

    WORD indices[] = {
        0, 1, 2, 0, 2, 3,    // front face (+Z)
        4, 6, 5, 4, 7, 6,    // back face (-Z)
        3, 2, 6, 3, 6, 7,    // left face (-X)
        0, 5, 1, 0, 4, 5,    // right face (+X)
        1, 5, 6, 1, 6, 2,    // top face
        0, 3, 7, 0, 7, 4     // bottom face
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA initData = { 0 };
    initData.pSysMem = vertices;

    if (FAILED(m_device->CreateBuffer(&bd, &initData, &m_vertexBuffer))) {
        std::cerr << "CreateBuffer vertex buffer failed." << std::endl;
        return false;
    }

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(indices);
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    initData.pSysMem = indices;

    if (FAILED(m_device->CreateBuffer(&bd, &initData, &m_indexBuffer))) {
        std::cerr << "CreateBuffer index buffer failed." << std::endl;
        return false;
    }

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(XMMATRIX) * 3; // world, view, projection matrices
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    if (FAILED(m_device->CreateBuffer(&bd, nullptr, &m_constantBuffer))) {
        std::cerr << "CreateBuffer constant buffer failed." << std::endl;
        return false;
    }

    return true;
}

bool DXManager::InitDevice(HWND hwnd) {
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 800;
    sd.BufferDesc.Height = 600;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    if (FAILED(D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &featureLevel, 1,
        D3D11_SDK_VERSION, &sd, &m_swapChain, &m_device, nullptr, &m_deviceContext))) {
        std::cerr << "D3D11CreateDeviceAndSwapChain failed." << std::endl;
        return false;
    }

    ID3D11Texture2D* pBackBuffer = nullptr;
    if (FAILED(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer))) {
        std::cerr << "GetBuffer failed." << std::endl;
        return false;
    }

    if (FAILED(m_device->CreateRenderTargetView(pBackBuffer, nullptr, &m_renderTargetView))) {
        std::cerr << "CreateRenderTargetView failed." << std::endl;
        pBackBuffer->Release();
        return false;
    }
    pBackBuffer->Release();

    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = 800;
    descDepth.Height = 600;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    if (FAILED(m_device->CreateTexture2D(&descDepth, nullptr, &m_depthStencilBuffer))) {
        std::cerr << "CreateTexture2D depth stencil buffer failed." << std::endl;
        return false;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    if (FAILED(m_device->CreateDepthStencilView(m_depthStencilBuffer, &descDSV, &m_depthStencilView))) {
        std::cerr << "CreateDepthStencilView failed." << std::endl;
        return false;
    }

    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    D3D11_VIEWPORT vp = {};
    vp.Width = 800.0f;
    vp.Height = 600.0f;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    m_deviceContext->RSSetViewports(1, &vp);

    return true;
}

bool DXManager::InitShaders() {
    ID3DBlob* vsBlob = nullptr;
    if (FAILED(D3DCompile(vertexShaderSource, strlen(vertexShaderSource), nullptr, nullptr, nullptr,
        "VS", "vs_4_0", 0, 0, &vsBlob, nullptr))) {
        std::cerr << "D3DCompile vertex shader failed." << std::endl;
        return false;
    }

    if (FAILED(m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vertexShader))) {
        std::cerr << "CreateVertexShader failed." << std::endl;
        vsBlob->Release();
        return false;
    }

    ID3DBlob* psBlob = nullptr;
    if (FAILED(D3DCompile(pixelShaderSource, strlen(pixelShaderSource), nullptr, nullptr, nullptr,
        "PS", "ps_4_0", 0, 0, &psBlob, nullptr))) {
        std::cerr << "D3DCompile pixel shader failed." << std::endl;
        return false;
    }

    if (FAILED(m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader))) {
        std::cerr << "CreatePixelShader failed." << std::endl;
        psBlob->Release();
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    if (FAILED(m_device->CreateInputLayout(layout, numElements, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_vertexLayout))) {
        std::cerr << "CreateInputLayout failed." << std::endl;
        vsBlob->Release();
        return false;
    }
    vsBlob->Release();

    return true;
}

void DXManager::Cleanup() {
    if (m_deviceContext) m_deviceContext->ClearState();
    if (m_constantBuffer) m_constantBuffer->Release();
    if (m_indexBuffer) m_indexBuffer->Release();
    if (m_vertexBuffer) m_vertexBuffer->Release();
    if (m_vertexLayout) m_vertexLayout->Release();
    if (m_pixelShader) m_pixelShader->Release();
    if (m_vertexShader) m_vertexShader->Release();
    if (m_depthStencilView) m_depthStencilView->Release();
    if (m_depthStencilBuffer) m_depthStencilBuffer->Release();
    if (m_renderTargetView) m_renderTargetView->Release();
    if (m_swapChain) m_swapChain->Release();
    if (m_deviceContext) m_deviceContext->Release();
    if (m_device) m_device->Release();
}
