#include "Kinect3DRenderer.h"
#include "../Core/PathManager.h"
#include "KinectDataManager.h"
#include "../Core/Core.h"
#include "../Core/InputManager.h"
#include "../Core/DeviceManager.h"
#include "../Core/DirectX11Helper.h"

Kinect3DRenderer::Kinect3DRenderer()
{
}


Kinect3DRenderer::~Kinect3DRenderer()
{
    ReleaseResources();

    if (scene) SAFE_DELETE(scene);
    if (model) SAFE_DELETE(model);
    if (mpDirect3DCamera) SAFE_DELETE_ARRAY(mpDirect3DCamera);
    if (mpPosNormalCollector) SAFE_DELETE(mpPosNormalCollector);
    if (mpVertexIndexCollector) SAFE_DELETE(mpVertexIndexCollector);
    Safe_Release_VectorList(m_vecDiffuseMapSRV);
    SAFE_RELEASE(m_pResource);
}

void Kinect3DRenderer::Init(class Object * obj, FrameManager * frameManager, const vector<KINECTDATA*>* kinectData)
{
    assert(obj);
    assert(kinectData);
    assert(frameManager);

    m_pObject = obj;

    mpFrameManager = frameManager;
    mpKinectData = kinectData;

    /*m_pD3DHelper->pD3DImmediateContext->RSSetState(m_pD3DHelper->pRasterizerState);*/

    XMMATRIX I = XMMatrixIdentity();
    XMStoreFloat4x4(&mSkullWorld, I);

    mpDirect3DCamera = new Direct3DCamera[2];
    Direct3DCamera::SetActive(0);

    mLight.Initialize(XMFLOAT4(1.f, 1.f, 1.f, 1.0f),
        XMFLOAT4(1.f, 1.f, 1.f, 1.0f),
        XMFLOAT4(1.f, 1.f, 1.f, 1.0f),
        XMFLOAT3(0.0f, 0.f, 0.f));

    mMaterialSkull.mAmbient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    mMaterialSkull.mDiffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
    mMaterialSkull.mSpecular = XMFLOAT4(0.65f, 0.65f, 0.65f, 60.f);

    MMDLoader();
}

void Kinect3DRenderer::Update(float fDeltaTime)
{
    mpDirect3DCamera[Direct3DCamera::GetActive()].Update(fDeltaTime);
    if (GetAsyncKeyState('1') & 0x8000)
    {
        Direct3DCamera::SetActive(0);
    }
    if (GetAsyncKeyState('2') & 0x8000)
    {
        Direct3DCamera::SetActive(1);
    }
    if (GetAsyncKeyState('3') & 0x8000)
    {
        XMFLOAT3 * pDirection = mLight.GetDirection();
        pDirection->y += 0.01f;
    }
    if (GetAsyncKeyState('4') & 0x8000)
    {
        XMFLOAT3 * pDirection = mLight.GetDirection();
        pDirection->y -= 0.01f;
    }
    if (GetAsyncKeyState('5') & 0x8000)
    {
        XMFLOAT3 * pDirection = mLight.GetDirection();
        pDirection->z += 0.01f;
    }
    if (GetAsyncKeyState('6') & 0x8000)
    {
        XMFLOAT3 * pDirection = mLight.GetDirection();
        pDirection->z -= 0.01f;
    }
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
    {
        //ShowCursor(FALSE);
        GetCursorPos(&mClickedMousePos);
        ScreenToClient(GET_WINDOWHANDLE, &mClickedMousePos);
        POSITION mMouseMove = GET_MOUSEMOVE;

        {
            // Make each pixel correspond to a quarter of a degree.
            float dx = XMConvertToRadians(0.25f*static_cast<float>(mMouseMove.x/* - mClickedMousePos.x*/));
            float dy = XMConvertToRadians(0.25f*static_cast<float>(mMouseMove.y/* - mClickedMousePos.y*/));

            mpDirect3DCamera[Direct3DCamera::GetActive()].Pitch(dy);
            mpDirect3DCamera[Direct3DCamera::GetActive()].RotateY(dx);
        }
        if (!GetSystemMetrics(SM_REMOTESESSION))
        {
            //GetCursorPos(&mClickedMousePos);
            //BOOL result = SetCursorPos(mClickedMousePos.x, mClickedMousePos.y);
        }
        else
        {
            // 원격 세션 지원용 코드
            //mClickedMousePos.x = mMouseMove.x;
            //mClickedMousePos.y = mMouseMove.y;
        }
    }
    else
    {
        //ShowCursor(TRUE);
    }

}

void Kinect3DRenderer::Render(Object * obj/*, ColorF::Enum color*/, RenderTargets * tRenderTargets, float fDeltaTime)
{
    assert(tRenderTargets);
    assert(tRenderTargets->ptD3DHelper);

    m_ptRenderTargets = tRenderTargets;
    m_pD3DHelper = tRenderTargets->ptD3DHelper;

    mpDirect3DCamera[Direct3DCamera::GetActive()].SetLens(obj, XM_PIDIV2, 0.01f, 30.0f);

    //m_pD3DHelper->pD3DImmediateContext->ClearRenderTargetView(m_pD3DHelper->pRenderTargetView, reinterpret_cast<const float*>(&GraphicsColors::Black));
    //m_pD3DHelper->pD3DImmediateContext->ClearDepthStencilView(m_pD3DHelper->pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    m_pD3DHelper->pD3DImmediateContext->IASetInputLayout(mpInputLayout);
    m_pD3DHelper->pD3DImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_pD3DHelper->pD3DImmediateContext->IASetVertexBuffers(0, 1, &mpJointVertexBuffer, &stride, &offset);
    m_pD3DHelper->pD3DImmediateContext->IASetIndexBuffer(mpJointIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    m_pD3DHelper->pD3DImmediateContext->VSSetShader(mpVS, nullptr, 0);
    m_pD3DHelper->pD3DImmediateContext->PSSetShader(mpPS, nullptr, 0);
    m_pD3DHelper->pD3DImmediateContext->RSSetState(m_pD3DHelper->pRasterizerState);
    m_pD3DHelper->pD3DImmediateContext->VSSetConstantBuffers(0, 1, &mpcbWorldViewProj);

    mWorldViewProj.World = (XMMatrixIdentity());
    mWorldViewProj.View = XMMatrixTranspose(mpDirect3DCamera[Direct3DCamera::GetActive()].GetViewMatrix());
    mWorldViewProj.Projection = XMMatrixTranspose(mpDirect3DCamera[Direct3DCamera::GetActive()].GetProjMatrix());
    m_pD3DHelper->pD3DImmediateContext->UpdateSubresource(mpcbWorldViewProj, 0, nullptr, &mWorldViewProj, 0, 0);

    int seletedIndex = mpFrameManager->GetIndex();
    int index = 0;

    // Axis
    m_pD3DHelper->pD3DImmediateContext->DrawIndexed
    (mpOffset->at(index).IndexCount, mpOffset->at(index).IndexOffset, mpOffset->at(index).VertexOffset);
    ++index;

    // 골격
    m_pD3DHelper->pD3DImmediateContext->DrawIndexed
    (mpOffset->at(index).IndexCount, mpOffset->at(index).IndexOffset, mpOffset->at(index).VertexOffset + (seletedIndex * 25));

    // 카메라
    for (size_t i = 0; i < 2; ++i)
    {
        auto R = XMMatrixTranspose(mpDirect3DCamera[i].GetViewMatrix());
        R.r[0].m128_f32[3] = 0.f;
        R.r[1].m128_f32[3] = 0.f;
        R.r[2].m128_f32[3] = 0.f;

        static XMFLOAT4X4 worldtemp;
        XMStoreFloat4x4(&worldtemp, XMMatrixIdentity());
        auto cam = mpDirect3DCamera[i].GetPosition();
        worldtemp(3, 0) = cam.x;
        worldtemp(3, 1) = cam.y;
        worldtemp(3, 2) = cam.z;
        worldtemp(3, 3) = 1.f;
        auto T = XMLoadFloat4x4(&worldtemp);
        auto RT = XMMatrixMultiply(R, T);
        mWorldViewProj.World = XMMatrixTranspose(RT);
        //mWorldViewProj.View = XMMatrixTranspose(mpDirect3DCamera[Direct3DCamera::GetActive()].GetViewMatrix());
        //mWorldViewProj.Projection = XMMatrixTranspose(mpDirect3DCamera[Direct3DCamera::GetActive()].GetProjMatrix());
        m_pD3DHelper->pD3DImmediateContext->UpdateSubresource(mpcbWorldViewProj, 0, nullptr, &mWorldViewProj, 0, 0);

        m_pD3DHelper->pD3DImmediateContext->DrawIndexed
        (mpOffset->at(2).IndexCount, mpOffset->at(2).IndexOffset, mpOffset->at(2).VertexOffset);
    }

    // skull
    UINT stride2 = sizeof(PosNormalTex);
    m_pD3DHelper->pD3DImmediateContext->IASetVertexBuffers(0, 1, &mpPosNormalJointVertexBuffer, &stride2, &offset);
    m_pD3DHelper->pD3DImmediateContext->IASetIndexBuffer(mpPosNormalJointIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    m_pD3DHelper->pD3DImmediateContext->IASetInputLayout(mpIALight);
    m_pD3DHelper->pD3DImmediateContext->VSSetShader(mpVSBasicLight, nullptr, 0);
    m_pD3DHelper->pD3DImmediateContext->PSSetShader(mpPSBasicLight, nullptr, 0);

    m_pD3DHelper->pD3DImmediateContext->VSSetConstantBuffers(0, 1, &mpcbLightPerFrame);

    m_pD3DHelper->pD3DImmediateContext->PSSetConstantBuffers(0, 1, &mpcbLight);
    m_pD3DHelper->pD3DImmediateContext->PSSetConstantBuffers(1, 1, &mpcbLightPerFrame);

    for (int i = 0; i < 26; ++i)
    {
        const vector<Vertex> * kinectVertices = mpVertexIndexCollector->GetVerticesVector();
        XMFLOAT3 skullS(0.01f, 0.01f, 0.01f);
        if (i == 3)
        {
            skullS.x = 0.03f;
            skullS.y = 0.03f;
            skullS.z = 0.03f;
        }
        XMFLOAT4 skullQ; XMStoreFloat4(&skullQ, XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMConvertToRadians(-30.0f)));
        XMFLOAT3 skullT;

        if (i != 25)
        {
            skullT.x = kinectVertices->at(mpOffset->at(1).VertexOffset + (seletedIndex * 25) + i).Pos.x;
            skullT.y = kinectVertices->at(mpOffset->at(1).VertexOffset + (seletedIndex * 25) + i).Pos.y - 0.1f;
            skullT.z = kinectVertices->at(mpOffset->at(1).VertexOffset + (seletedIndex * 25) + i).Pos.z;
        }
        else
        {   // cam
            XMFLOAT3 * pLishtPos = mLight.GetDirection();
            skullT.x = pLishtPos->x;
            skullT.y = pLishtPos->y;
            skullT.z = pLishtPos->z;
        }

        XMVECTOR S = XMLoadFloat3(&skullS);
        XMVECTOR T = XMLoadFloat3(&skullT);
        XMVECTOR Q = XMLoadFloat4(&skullQ);

        XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
        XMStoreFloat4x4(&mSkullWorld, XMMatrixAffineTransformation(S, zero, Q, T));

        cbLightPerFrame cbLight;
        cbLight.mEyePosW = mpDirect3DCamera[Direct3DCamera::GetActive()].GetPosition();
        cbLight.mMaterial = mLight;
        static float testFadeAmount = 0.f;
        static float testFadeValue = 0.001f;
        if (testFadeAmount > 1.f || testFadeAmount < 0.f) testFadeValue *= -1.f;
        testFadeAmount += testFadeValue;
        cbLight.mFadeAmount = testFadeAmount;

        m_pD3DHelper->pD3DImmediateContext->UpdateSubresource(mpcbLight, 0, nullptr, &cbLight, 0, 0);

        cbLightPerObject lightPerFrame;
        XMMATRIX skullWorld = XMLoadFloat4x4(&mSkullWorld);
        lightPerFrame.mWorld = XMMatrixTranspose(skullWorld);
        lightPerFrame.mWorldInvTranspose = XMMatrixTranspose(skullWorld);
        lightPerFrame.mWorldViewProj = XMMatrixTranspose(mpDirect3DCamera[Direct3DCamera::GetActive()].GetWorldViewProj(skullWorld));
        lightPerFrame.mMaterial = mMaterialSkull;
        //lightPerFrame.m_pDiffuseMapSRV = nullptr;

        m_pD3DHelper->pD3DImmediateContext->UpdateSubresource(mpcbLightPerFrame, 0, nullptr, &lightPerFrame, 0, 0);
        //m_pD3DHelper->pD3DImmediateContext->PSSetShaderResources(0, 1, &m_vecDiffuseMapSRV[0]);

        m_pD3DHelper->pD3DImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_pD3DHelper->pD3DImmediateContext->DrawIndexed
        (mpPosNormalTexOffset->at(0).IndexCount, mpPosNormalTexOffset->at(0).IndexOffset, mpPosNormalTexOffset->at(0).VertexOffset);
    }
    //m_pD3DHelper->pD3DImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//cbLightPerFrame cbLight;
//cbLight.mEyePosW = mpDirect3DCamera[Direct3DCamera::GetActive()].GetPosition();
//cbLight.mMaterial = mLight;
//static float testFadeAmount = 1.f;
//cbLight.mFadeAmount = testFadeAmount;
//m_pD3DHelper->pD3DImmediateContext->UpdateSubresource(mpcbLight, 0, nullptr, &cbLight, 0, 0);

    if (m_vecDiffuseMapSRV.size() == 0)
    {
        ID3D11Device * pD3DDevice = GET_DEVICE_MANAGER->GetDirectX11Helper()->GetD3DHelper()->pD3DDevice;
        int iMaterialsIndex = -1;
        int iIndexCount = 0;
        for (int i = 0; i < model->indices_.size();)
        {
            if (iIndexCount <= i)
            {
                ++iMaterialsIndex;
                iIndexCount += model->materials_[iMaterialsIndex].vertex_count;
                i = iIndexCount;

                ID3D11ShaderResourceView * pDiffuseMapSRV = nullptr;
                wstring wstrPath = GET_SINGLE(PathManager)->FindPath(MODEL_PATH);
                setlocale(LC_ALL, "Japanese");
                string strTextureFileName = (char *)model->materials_[iMaterialsIndex].texture_filename;
                wstring strResult;
                const char * str = strTextureFileName.c_str();
                wchar_t fileName[256] = { 0, };
                mbstate_t shiftState = mbstate_t();
                mbsrtowcs(fileName, &str, sizeof(fileName), &shiftState);
                strResult = fileName;
                int f = strResult.find(L"*");
                if (f > 0)
                {
                    //continue;
                    strResult[f] = 0;
                }

                f = strResult.find(L".tga");

                wstring wstr = wstrPath + L"유키미쿠\\" + strResult;
                if (f > 0)
                {
                    TexMetadata tTexMetadata;
                    ScratchImage cScratchImage;
                    LoadFromTGAFile(wstr.c_str(), &tTexMetadata, cScratchImage);
                    CreateTexture(pD3DDevice, cScratchImage.GetImages(), cScratchImage.GetImageCount(), tTexMetadata, &m_pResource);
                    CreateShaderResourceView(pD3DDevice, cScratchImage.GetImages(), cScratchImage.GetImageCount(), tTexMetadata, &pDiffuseMapSRV);
                    /*strResult.replace(f, 4, L".png");
                    strResult[f + 4] = 0;*/
                }
                else
                {
                    HRESULT hr = CreateWICTextureFromFile(pD3DDevice, wstr.c_str(), &m_pResource, &pDiffuseMapSRV);
                }
                SAFE_RELEASE(m_pResource);
                assert(pDiffuseMapSRV);
                m_vecDiffuseMapSRV.push_back(pDiffuseMapSRV);
            }
        }
    }

    {    // 미쿠
        XMFLOAT3 mikuS(1.0f, 1.0f, 1.0f);
        XMFLOAT4 mikuQ; XMStoreFloat4(&mikuQ, XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMConvertToRadians(0.0f)));
        XMFLOAT3 mikuT(0.f, 0.f, 0.f);

        XMVECTOR S = XMLoadFloat3(&mikuS);
        XMVECTOR Q = XMLoadFloat4(&mikuQ);
        XMVECTOR T = XMLoadFloat3(&mikuT);

        XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
        XMStoreFloat4x4(&mMikuWorld, XMMatrixAffineTransformation(S, zero, Q, T));

        cbLightPerFrame cbLight;
        cbLight.mEyePosW = mpDirect3DCamera[Direct3DCamera::GetActive()].GetPosition();
        cbLight.mMaterial = mLight;
        /*static float testFadeAmount = 0.f;
        static float testFadeValue = 0.001f;
        if (testFadeAmount > 1.f || testFadeAmount < 0.f) testFadeValue *= -1.f;
        testFadeAmount += testFadeValue;*/
        cbLight.mFadeAmount = 1.f;

        m_pD3DHelper->pD3DImmediateContext->UpdateSubresource(mpcbLight, 0, nullptr, &cbLight, 0, 0);

        for (int i = 0; i < m_vecMaterialMiku.size(); ++i)
        {
            cbLightPerObject lightPerFrame;
            XMMATRIX matMikuWorld = XMLoadFloat4x4(&mMikuWorld);
            lightPerFrame.mWorld = XMMatrixTranspose(matMikuWorld);
            lightPerFrame.mWorldInvTranspose = XMMatrixTranspose(matMikuWorld);
            lightPerFrame.mWorldViewProj = XMMatrixTranspose(mpDirect3DCamera[Direct3DCamera::GetActive()].GetWorldViewProj(matMikuWorld));

            lightPerFrame.mMaterial = m_vecMaterialMiku[i];
            //lightPerFrame.m_pDiffuseMapSRV = m_pDiffuseMapSRV;

            m_pD3DHelper->pD3DImmediateContext->UpdateSubresource(mpcbLightPerFrame, 0, nullptr, &lightPerFrame, 0, 0);

            m_pD3DHelper->pD3DImmediateContext->PSSetShaderResources(0, 1, &m_vecDiffuseMapSRV[i]);
            m_pD3DHelper->pD3DImmediateContext->DrawIndexed
            (mpPosNormalTexOffset->at(i + 1).IndexCount, mpPosNormalTexOffset->at(i + 1).IndexOffset, mpPosNormalTexOffset->at(i + 1).VertexOffset);
        }
    }
    //(mpD3DHelperPointers->pSwapChain->Present(0, 0));
}

void Kinect3DRenderer::ReleaseResources()
{
    if (mpcbWorldViewProj) SAFE_RELEASE(mpcbWorldViewProj);
    if (mpcbLight) SAFE_RELEASE(mpcbLight);
    if (mpcbLightPerFrame) SAFE_RELEASE(mpcbLightPerFrame);
    if (mpInputLayout) SAFE_RELEASE(mpInputLayout);
    if (mpIALight) SAFE_RELEASE(mpIALight);

    if (mpJointIndexBuffer) SAFE_RELEASE(mpJointIndexBuffer);
    if (mpJointVertexBuffer) SAFE_RELEASE(mpJointVertexBuffer);
    if (mpPosNormalJointIndexBuffer) SAFE_RELEASE(mpPosNormalJointIndexBuffer);
    if (mpPosNormalJointVertexBuffer) SAFE_RELEASE(mpPosNormalJointVertexBuffer);

    if (mpVS) SAFE_RELEASE(mpVS);
    if (mpPS) SAFE_RELEASE(mpPS);

    if (mpVSBuffer) SAFE_RELEASE(mpVSBuffer);
    if (mpPSBuffer) SAFE_RELEASE(mpPSBuffer);
    if (mpPSBasicLightBuffer) SAFE_RELEASE(mpPSBasicLightBuffer);

    if (scene) SAFE_DELETE(scene);
}

void Kinect3DRenderer::BuildGeometryBuffers()
{
    vector<Vertex> xyzAxis(6);
    xyzAxis[0] = { XMFLOAT3(-2, 0, 0), XMFLOAT4((const float*)&GraphicsColors::Red) };
    xyzAxis[1] = { XMFLOAT3(2, 0, 0), XMFLOAT4((const float*)&GraphicsColors::Red) };
    xyzAxis[2] = { XMFLOAT3(0, 0, 0), XMFLOAT4((const float*)&GraphicsColors::Blue) };
    xyzAxis[3] = { XMFLOAT3(0, 0, 5), XMFLOAT4((const float*)&GraphicsColors::Blue) };
    xyzAxis[4] = { XMFLOAT3(0, -2, 0), XMFLOAT4((const float*)&GraphicsColors::Green) };
    xyzAxis[5] = { XMFLOAT3(0, 2, 0), XMFLOAT4((const float*)&GraphicsColors::Green) };

    vector<Vertex> xyzAxisForCam(6);
    xyzAxisForCam[0] = { XMFLOAT3(-0.004f, 0, 0.1f), XMFLOAT4((const float*)&GraphicsColors::White) };
    xyzAxisForCam[1] = { XMFLOAT3(0.004f, 0, 0.1f), XMFLOAT4((const float*)&GraphicsColors::White) };
    xyzAxisForCam[2] = { XMFLOAT3(0, 0, -0.001f), XMFLOAT4((const float*)&GraphicsColors::White) };
    xyzAxisForCam[3] = { XMFLOAT3(0, 0, 40), XMFLOAT4((const float*)&GraphicsColors::Black) };
    xyzAxisForCam[4] = { XMFLOAT3(0, -0.004f, 0.1f), XMFLOAT4((const float*)&GraphicsColors::White) };
    xyzAxisForCam[5] = { XMFLOAT3(0, 0.004f, 0.1f), XMFLOAT4((const float*)&GraphicsColors::White) };

    vector<Vertex> skelatonFrames(mpKinectData->size() * JointType_Count);
    if (mpFrameManager == GET_SINGLE(KinectDataManager)->GetFrameManager())
    {
        for (size_t i = 0; i < mpKinectData->size(); ++i)
        {
            for (size_t j = 0; j < JointType_Count; ++j)
            {
                skelatonFrames[(i * JointType_Count) + j] = {
                    XMFLOAT3
                    (
                        mpKinectData->at(i)->mCapturedJoints[j].Position.X,
                        mpKinectData->at(i)->mCapturedJoints[j].Position.Y,
                        mpKinectData->at(i)->mCapturedJoints[j].Position.Z
                    ),
                    XMFLOAT4((const float*)&GraphicsColors::Yellow)
                };
            }
        }
    }
    else
    {
        for (size_t i = 0; i < mpKinectData->size(); ++i)
        {
            for (size_t j = 0; j < JointType_Count; ++j)
            {
                skelatonFrames[(i * JointType_Count) + j] = {
                    XMFLOAT3
                    (
                        mpKinectData->at(i)->mCapturedJoints[j].Position.X,
                        mpKinectData->at(i)->mCapturedJoints[j].Position.Y,
                        mpKinectData->at(i)->mCapturedJoints[j].Position.Z
                    ),
                    XMFLOAT4((const float*)&GraphicsColors::Cyan)
                };
            }
        }
    }

    UINT totalVertexCount = xyzAxis.size() + xyzAxisForCam.size() + skelatonFrames.size();

    // Create the index buffer
    size_t maxIndexCount = 0;
    UINT xyzAxisIndices[]
        =
    {
        0, 1,
        2, 3,
        4, 5
    };
    UINT skelatonIndices[]
        =
    {
        // 머리부터 SPINE_BASE
        3, 2,
        2, 20,
        20, 1,
        1, 0,

        // SPINE_BASE 부터 FOOT_LEFT
        0, 12,
        12, 13,
        13, 14,
        14, 15,

        // SPINE_BASE 부터 FOOT_RIGHT
        0, 16,
        16, 17,
        17, 18,
        18, 19,

        // SPINE_SHOULDER 부터 왼손
        20, 4,
        4, 5,
        5, 6,
        6, 7,
        7, 21,
        7, 22,

        // SPINE_SHOULDER 부터 오른손
        20, 8,
        8, 9,
        9, 10,
        10, 11,
        11, 23,
        11, 24
    };
    maxIndexCount = ARRAYSIZE(xyzAxisIndices) + ARRAYSIZE(xyzAxisIndices) + ARRAYSIZE(skelatonIndices);

    mpVertexIndexCollector = new VertexIndexCollector<Vertex>(totalVertexCount /*+ model->vertices_.size()*/, maxIndexCount /*+ model->indices_.size()*/, 4);
    mpVertexIndexCollector->AddItem(&xyzAxis[0], xyzAxis.size(), &xyzAxisIndices[0], ARRAYSIZE(xyzAxisIndices));
    mpVertexIndexCollector->AddItem(&skelatonFrames[0], skelatonFrames.size(), &skelatonIndices[0], ARRAYSIZE(skelatonIndices));
    mpVertexIndexCollector->AddItem(&xyzAxisForCam[0], xyzAxisForCam.size(), &xyzAxisIndices[0], ARRAYSIZE(xyzAxisIndices));

    mpOffset = mpVertexIndexCollector->GetOffset();

    std::vector<PosNormalTex> skull;
    std::vector<UINT> skullIndex;
    BuildSkullGeometry(skull, skullIndex);

    // 미쿠
    vector<PosNormalTex> mmdVertices(model->vertices_.size());
    for (int i = 0; i < mmdVertices.size(); ++i)
    {
        mmdVertices[i].Pos.x = model->vertices_[i].pos[0];
        mmdVertices[i].Pos.y = model->vertices_[i].pos[1];
        mmdVertices[i].Pos.z = model->vertices_[i].pos[2];
        mmdVertices[i].Normal.x = model->vertices_[i].normal[0];
        mmdVertices[i].Normal.y = model->vertices_[i].normal[1];
        mmdVertices[i].Normal.z = -model->vertices_[i].normal[2];

        mmdVertices[i].mTexture.x = model->vertices_[i].uv[0];
        mmdVertices[i].mTexture.y = model->vertices_[i].uv[1];
    }
    std::vector<UINT> mmdIndices(model->indices_.size());
    int iMaterialsIndex = -1;
    int iIndexCount = 0;
    for (int i = 0; i < mmdIndices.size(); ++i)
    {
        mmdIndices[i] = model->indices_[i];

        if (iIndexCount <= i)
        {
            ++iMaterialsIndex;
            iIndexCount += model->materials_[iMaterialsIndex].vertex_count;

            // 텍스처
            //string strFileName = (char *)model->materials_[iMaterialsIndex].texture_filename;
            //m_vecTextureName.push_back(strFileName);
            m_iTextureCount++;

            Material tModelMaterial;
            tModelMaterial.mAmbient.x = model->materials_[iMaterialsIndex].ambient[0];
            tModelMaterial.mAmbient.y = model->materials_[iMaterialsIndex].ambient[1];
            tModelMaterial.mAmbient.z = model->materials_[iMaterialsIndex].ambient[2];
            tModelMaterial.mAmbient.w = model->materials_[iMaterialsIndex].alpha;

            tModelMaterial.mDiffuse.x = model->materials_[iMaterialsIndex].diffuse[0];
            tModelMaterial.mDiffuse.y = model->materials_[iMaterialsIndex].diffuse[1];
            tModelMaterial.mDiffuse.z = model->materials_[iMaterialsIndex].diffuse[2];
            tModelMaterial.mDiffuse.w = model->materials_[iMaterialsIndex].alpha;

            tModelMaterial.mSpecular.x = model->materials_[iMaterialsIndex].specular[0];
            tModelMaterial.mSpecular.y = model->materials_[iMaterialsIndex].specular[1];
            tModelMaterial.mSpecular.z = model->materials_[iMaterialsIndex].specular[2];
            tModelMaterial.mSpecular.w = model->materials_[iMaterialsIndex].alpha;

            m_vecMaterialMiku.push_back(tModelMaterial);

            //ID3D11ShaderResourceView * pDiffuseMapSRV = nullptr;
            //wstring wstrPath = GET_SINGLE(PathManager)->FindPath(MODEL_PATH);
            //string strTextureFileName = (char *)model->materials_[iMaterialsIndex].texture_filename;
            //wstring wstrTextureFileName(strTextureFileName.begin(), strTextureFileName.end());
            //wstring wstr = wstrPath + wstrTextureFileName;

            //ID3D11Device * pD3DDevice = GET_DEVICE_MANAGER->GetDirectX11Helper()->GetD3DHelper()->pD3DDevice;
            //CreateWICTextureFromFile(pD3DDevice, wstr.c_str(), &m_pResource, &pDiffuseMapSRV);
            //SAFE_RELEASE(m_pResource);
            //assert(pDiffuseMapSRV);
            //m_vecDiffuseMapSRV.push_back(pDiffuseMapSRV);
        }

        //mmdVertices[i].mAmbient.x = model->materials_[iMaterialsIndex].ambient[0];
        //mmdVertices[i].mAmbient.y = model->materials_[iMaterialsIndex].ambient[1];
        //mmdVertices[i].mAmbient.z = model->materials_[iMaterialsIndex].ambient[2];
        //mmdVertices[i].mAmbient.w = model->materials_[iMaterialsIndex].alpha;

        //mmdVertices[i].mDiffuse.x = model->materials_[iMaterialsIndex].diffuse[0];
        //mmdVertices[i].mDiffuse.y = model->materials_[iMaterialsIndex].diffuse[1];
        //mmdVertices[i].mDiffuse.z = model->materials_[iMaterialsIndex].diffuse[2];
        //mmdVertices[i].mDiffuse.w = model->materials_[iMaterialsIndex].alpha;

        //mmdVertices[i].mSpecular.x = model->materials_[iMaterialsIndex].specular[0];
        //mmdVertices[i].mSpecular.y = model->materials_[iMaterialsIndex].specular[1];
        //mmdVertices[i].mSpecular.z = model->materials_[iMaterialsIndex].specular[2];
        //mmdVertices[i].mSpecular.w = model->materials_[iMaterialsIndex].alpha;
    }


    totalVertexCount = skull.size() + model->vertices_.size() * m_iTextureCount;
    maxIndexCount = skullIndex.size() + model->indices_.size();

    mpPosNormalCollector = new VertexIndexCollector<PosNormalTex>(totalVertexCount, maxIndexCount, 1 + m_iTextureCount);
    mpPosNormalCollector->AddItem(&skull[0], skull.size(), &skullIndex[0], skullIndex.size());

    int vStartOffset = 0;
    for (int i = 0; i < model->materials_.size(); ++i)
    {
        int iSize = model->materials_[i].vertex_count;
        mpPosNormalCollector->AddItem(&mmdVertices[0], mmdVertices.size(), &mmdIndices[vStartOffset], iSize);

        vStartOffset += iSize;
    }
    //mpVertexIndexCollector->AddItem(&mmdVertices[0], mmdVertices.size(), &mmdIndices[0], mmdIndices.size());

    mpPosNormalTexOffset = mpPosNormalCollector->GetOffset();

    //LoadSkinnedModel();

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = mpVertexIndexCollector->GetVertexByteWidth();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = mpVertexIndexCollector->GetVertices();
    (m_pD3DHelper->pD3DDevice->CreateBuffer(&vbd, &vinitData, &mpJointVertexBuffer));

    vbd.ByteWidth = mpPosNormalCollector->GetVertexByteWidth();
    vinitData.pSysMem = mpPosNormalCollector->GetVertices();
    (m_pD3DHelper->pD3DDevice->CreateBuffer(&vbd, &vinitData, &mpPosNormalJointVertexBuffer));

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = mpVertexIndexCollector->GetIndexByteWidth();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = mpVertexIndexCollector->GetIndices();
    (m_pD3DHelper->pD3DDevice->CreateBuffer(&ibd, &iinitData, &mpJointIndexBuffer));

    ibd.ByteWidth = mpPosNormalCollector->GetIndexByteWidth();
    iinitData.pSysMem = mpPosNormalCollector->GetIndices();
    (m_pD3DHelper->pD3DDevice->CreateBuffer(&ibd, &iinitData, &mpPosNormalJointIndexBuffer));

    mpVertexIndexCollector->ReleaseVertexIndexFromMemory();
    mpPosNormalCollector->ReleaseVertexIndexFromMemory();
}

void Kinect3DRenderer::BuildSkullGeometry(std::vector<PosNormalTex>& vertices, std::vector<UINT>& indices)
{
    wstring wstrPath = GET_SINGLE(PathManager)->FindPath(MODEL_PATH);
    wstring wstr = wstrPath + L"skull.txt";
    std::ifstream fin(wstr.c_str());

    if (!fin)
    {
        MessageBox(0, L"Models/skull.txt not found.", 0, 0);
        return;
    }

    UINT vcount = 0;
    UINT tcount = 0;
    std::string ignore;

    fin >> ignore >> vcount;
    fin >> ignore >> tcount;
    fin >> ignore >> ignore >> ignore >> ignore;

    vertices.resize(vcount);
    for (UINT i = 0; i < vcount; ++i)
    {
        fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
        fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
        //vertices[i].mAmbient = { 1.f, 1.f, 1.f, 1.f };
        //vertices[i].mDiffuse = { 1.f, 1.f, 1.f, 1.f };
        //vertices[i].mSpecular = { 1.f, 1.f, 1.f, 1.f };
        //vertices[i].mReflect = { 1.f, 1.f, 1.f, 1.f };
        vertices[i].mTexture = { 0.f, 0.f };
    }

    fin >> ignore;
    fin >> ignore;
    fin >> ignore;

    mSkullIndexCount = 3 * tcount;
    indices.resize(mSkullIndexCount);
    for (UINT i = 0; i < tcount; ++i)
    {
        fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
    }

    fin.close();
}

void Kinect3DRenderer::InitializeShader()
{
    wstring wstrPath = GET_SINGLE(PathManager)->FindPath(SHADER_PATH);

    wstring wstr = wstrPath + L"BasicVertex.cso";
    D3DReadFileToBlob(wstr.c_str(), &mpVSBuffer);
    m_pD3DHelper->pD3DDevice->CreateVertexShader(mpVSBuffer->GetBufferPointer(), mpVSBuffer->GetBufferSize(), nullptr, &mpVS);
    m_pD3DHelper->pD3DImmediateContext->VSSetShader(mpVS, 0, 0);

    wstr = wstrPath + L"BasicPixel.cso";
    D3DReadFileToBlob(wstr.c_str(), &mpPSBuffer);
    m_pD3DHelper->pD3DDevice->CreatePixelShader(mpPSBuffer->GetBufferPointer(), mpPSBuffer->GetBufferSize(), nullptr, &mpPS);
    m_pD3DHelper->pD3DImmediateContext->PSSetShader(mpPS, 0, 0);

    wstr = wstrPath + L"VSBasicLight.cso";
    D3DReadFileToBlob(wstr.c_str(), &mpVSBasicLightBuffer);
    m_pD3DHelper->pD3DDevice->CreateVertexShader(mpVSBasicLightBuffer->GetBufferPointer(), mpVSBasicLightBuffer->GetBufferSize(), nullptr, &mpVSBasicLight);
    m_pD3DHelper->pD3DImmediateContext->VSSetShader(mpVSBasicLight, 0, 0);

    wstr = wstrPath + L"PSBasicLight.cso";
    D3DReadFileToBlob(wstr.c_str(), &mpPSBasicLightBuffer);
    m_pD3DHelper->pD3DDevice->CreatePixelShader(mpPSBasicLightBuffer->GetBufferPointer(), mpPSBasicLightBuffer->GetBufferSize(), nullptr, &mpPSBasicLight);
    m_pD3DHelper->pD3DImmediateContext->PSSetShader(mpPSBasicLight, 0, 0);
}

void Kinect3DRenderer::BuildVertexLayout()
{// Create the vertex input layout.
    D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    // Create the input layout
    (m_pD3DHelper->pD3DDevice->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), mpVSBuffer->GetBufferPointer(),
        mpVSBuffer->GetBufferSize(), &mpInputLayout));

    D3D11_INPUT_ELEMENT_DESC vertexDesc2[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12 + 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12 + 12 + 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12 + 12 + 16 + 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 3, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12 + 12 + 16 + 16 + 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12 + 12 + 16 + 16 + 16 + 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    // Create the input layout
    (m_pD3DHelper->pD3DDevice->CreateInputLayout(vertexDesc2, ARRAYSIZE(vertexDesc2), mpVSBasicLightBuffer->GetBufferPointer(),
        mpVSBasicLightBuffer->GetBufferSize(), &mpIALight));

    SAFE_RELEASE(mpVSBuffer);
    SAFE_RELEASE(mpPSBuffer);
    SAFE_RELEASE(mpVSBasicLightBuffer);
    SAFE_RELEASE(mpPSBasicLightBuffer);

    // Create the constant buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(cbWorldViewProj);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_pD3DHelper->pD3DDevice->CreateBuffer(&bd, nullptr, &mpcbWorldViewProj);
    bd.ByteWidth = sizeof(cbLightPerFrame);
    m_pD3DHelper->pD3DDevice->CreateBuffer(&bd, nullptr, &mpcbLight);
    bd.ByteWidth = sizeof(cbLightPerObject);
    m_pD3DHelper->pD3DDevice->CreateBuffer(&bd, nullptr, &mpcbLightPerFrame);
}

void Kinect3DRenderer::MMDLoader()
{
    PMDReader pmdreader;
    string strPath = GET_SINGLE(PathManager)->FindPathMultiByte(MODEL_PATH);
    string str = strPath + "유키미쿠\\mikuXS雪ミク.pmd";
    //setlocale(LC_ALL, "ja_JP.Shift_JIS");
    //wstring test = "mikuXS雪ミク.pmd";
    model = pmdreader.LoadFromFile(str);
    assert(model);

    //VMDReader vmdreader;
    //anim = vmdreader.LoadFromFile(vmdmodel);
    //assert(anim);

    //scene = new MMDScene();
    //scene->SetModel(model);
    //scene->AttachAnimation(anim);

    //renderVertices = new float[3 * model->vertices_.size()];

    //DumpIK();
    //DumpBone();

    //CalculateBboxMinMax();

    //IdentifyChainBones("head", NULL);
}

void Kinect3DRenderer::DataFileLoaded()
{
    mClickedMousePos.x = 0;
    mClickedMousePos.y = 0;
    for (size_t i = 0; i < 2; ++i)
    {
        mpDirect3DCamera[i].Initialize(m_pObject);
    }

    ReleaseResources();
    BuildGeometryBuffers();
    InitializeShader();
    BuildVertexLayout();
}

void Kinect3DRenderer::OnDataFileLoaded(RenderTargets * tRenderTargets)
{
    m_ptRenderTargets = tRenderTargets;
    m_pD3DHelper = tRenderTargets->ptD3DHelper;

    DataFileLoaded();
}

//void Kinect3DRenderer::OnComparativeDataFileLoaded()
//{
//}
