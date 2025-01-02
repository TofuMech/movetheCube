#ifndef DXMANAGER_H
#define DXMANAGER_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "Character.h"
#include <string>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

class DXManager {
public:
    struct Vertex {
        XMFLOAT3 Pos;
        XMFLOAT4 Color;
    };
    struct MatrixBuffer {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

    DXManager();
    ~DXManager();

    bool Initialize(HWND hwnd);
    void Update(float deltaTime);
    void Render();
    void Cleanup();
    void OnKeyDown(WPARAM key);
    void OnKeyUp(WPARAM key);

private:
    bool InitDevice(HWND hwnd);
    bool InitShaders();
    bool InitGeometry();

    ID3D11Device* m_device;
    ID3D11DeviceContext* m_deviceContext;
    IDXGISwapChain* m_swapChain;
    ID3D11RenderTargetView* m_renderTargetView;
    ID3D11DepthStencilView* m_depthStencilView;
    ID3D11Texture2D* m_depthStencilBuffer;
    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11InputLayout* m_vertexLayout;
    ID3D11Buffer* m_vertexBuffer;
    ID3D11Buffer* m_indexBuffer;
    ID3D11Buffer* m_constantBuffer;

    XMMATRIX m_worldMatrix;
    XMMATRIX m_viewMatrix;
    XMMATRIX m_projectionMatrix;

    Character m_character;
};

#endif // DXMANAGER_H