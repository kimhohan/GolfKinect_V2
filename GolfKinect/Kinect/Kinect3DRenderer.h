#pragma once
#include "../Game.h"
#include "FrameManager.h"
#include "LightHelper.h"
#include "Direct3DCamera.h"
#include "VertexIndexCollector.h"

// MMD
#include "trackball.h"
#include "pmd_reader.h"
#include "vmd_reader.h"
#include "mmd_scene.h"
#include "mmd_math.h"

struct cbLightPerObject
{
    XMMATRIX mWorld;
    XMMATRIX mWorldInvTranspose;
    XMMATRIX mWorldViewProj;
    Material mMaterial;
    //ID3D11ShaderResourceView* m_pDiffuseMapSRV = nullptr;
};

struct cbLightPerFrame
{
    LightHelper mMaterial;
    XMFLOAT3 mEyePosW;
    float mFadeAmount;
};

using namespace mmd;

class Kinect3DRenderer
{
private:
    PMDModel * model = NULL;
    //VMDAnimation *anim = NULL;
    MMDScene *scene = NULL;

    RenderTargets * m_ptRenderTargets = nullptr;
    D3DHelper * m_pD3DHelper = nullptr;
    FrameManager * mpFrameManager = nullptr;

    ID3D11InputLayout * mpInputLayout = nullptr;
    ID3D11InputLayout * mpIALight = nullptr;

    // Shader
    ID3D11VertexShader * mpVS = nullptr;
    ID3D11PixelShader * mpPS = nullptr;
    ID3DBlob * mpVSBuffer = nullptr;
    ID3DBlob * mpPSBuffer = nullptr;

    ID3D11VertexShader * mpVSBasicLight = nullptr;
    ID3D11PixelShader * mpPSBasicLight = nullptr;
    ID3DBlob * mpVSBasicLightBuffer = nullptr;
    ID3DBlob * mpPSBasicLightBuffer = nullptr;

    // Vertex, Index
    ID3D11Buffer * mpJointVertexBuffer = nullptr;
    ID3D11Buffer * mpJointIndexBuffer = nullptr;
    ID3D11Buffer * mpPosNormalJointVertexBuffer = nullptr;
    ID3D11Buffer * mpPosNormalJointIndexBuffer = nullptr;

    // cbuffer
    ID3D11Buffer * mpcbWorldViewProj = nullptr;
    ID3D11Buffer * mpcbLight = nullptr;
    ID3D11Buffer * mpcbLightPerFrame = nullptr;
    ID3D11Buffer * mpcbTexture = nullptr;

    cbWorldViewProj mWorldViewProj;


    // camera
    Direct3DCamera * mpDirect3DCamera = nullptr;

    POINT mClickedMousePos;

    int mTargetWidth;
    int mTargetHeight;

    VertexIndexCollector<Vertex> * mpVertexIndexCollector;
    VertexIndexCollector<PosNormalTex> * mpPosNormalCollector;
    const vector<Offset> * mpOffset;
    const vector<Offset> * mpPosNormalTexOffset;

    const vector<KINECTDATA*>* mpKinectData;

    // light
    LightHelper mLight;

    // skull
    XMFLOAT4X4 mSkullWorld;
    UINT mSkullIndexCount;
    Material mMaterialSkull;

    XMFLOAT4X4 mMikuWorld;
    vector<Material> m_vecMaterialMiku;
    ID3D11Resource * m_pResource = nullptr;
    int m_iTextureCount = 0;
    //vector<string> m_vecTextureName;
    vector<ID3D11ShaderResourceView *> m_vecDiffuseMapSRV;

    class Object * m_pObject = nullptr;

public:
    Kinect3DRenderer();
    ~Kinect3DRenderer();

    void Init(class Object * obj, FrameManager * frameManager, const vector<KINECTDATA*>* kinectData);
    void Update(float fDeltaTime);
    void Render(class Object * obj/*, ColorF::Enum color*/, RenderTargets * tRenderTargets, float fDeltaTime);
    void ReleaseResources();

    virtual void OnDataFileLoaded(RenderTargets * tRenderTargets);

private:
    void BuildGeometryBuffers();
    void BuildSkullGeometry(std::vector<PosNormalTex> & vertices, std::vector<UINT> & indices);
    void InitializeShader();
    void BuildVertexLayout();

    void MMDLoader();

    void DataFileLoaded();
    //virtual void OnComparativeDataFileLoaded();
};

