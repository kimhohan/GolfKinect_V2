#include "Kinect2DRenderer.h"
#include "../Core/Core.h"
#include "../Core/Direct2DHelper.h"
#include "../Object/Object.h"
#include "KinectManager.h"
#include "KinectBody.h"
#include "../HashTable.h"
#include "../Core/InputManager.h"
#include "../Object/Form.h"
#include "KinectDataManager.h"
#include "Kinect2DGraphRenderer.h"

void WinProcPopupMenu(HMENU hPopupMenu, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   // if (GET_SINGLE(SceneManager)->GetScene() == EScene::DataView)
    {
        HMENU playPopupMenu = CreatePopupMenu();
        InsertMenu(playPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_FRAMESTOP, L"정지");
        InsertMenu(playPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_FRAMEPLAY_REPEAT, L"반복재생");
        InsertMenu(playPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_FRAMEPLAY, L"재생");
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING | MF_POPUP, (UINT_PTR)playPopupMenu, L"재생");

        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, NULL, nullptr);
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_KINECT2DCOMPARATIVEDATA_RENDERTARGET, L"Comparative Data 2D Render Target");
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_KINECT3DCOMPARATIVEDATA_RENDERTARGET, L"Comparative Data 3D Render Target");
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, NULL, nullptr);
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_KINECTGRAPH_RENDERTARGET, L"Graph Render Target");
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_KINECT2DDATA_RENDERTARGET, L"Data 2D Render Target");
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_KINECT3DDATA_RENDERTARGET, L"Data 3D Render Target");
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_KINECTDATA_SUBLISTBOX, L"Data Sub List Box");
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, NULL, nullptr);
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_OPENCOMPARATIVE, L"대조 데이터 불러오기");
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_OPEN, L"데이터 불러오기");
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, NULL, nullptr);
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SAVE, L"데이터 저장하기");
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, NULL, nullptr);

        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDM_KinectView, L"Kinect View");
    }
   /* else if (GET_SINGLE(SceneManager)->GetScene() == EScene::KinectView)
    {
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_KINECT2DDATA_RENDERTARGET, L"Data 2D Render Target");
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SAVE, L"데이터 저장하기");
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, NULL, nullptr);
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDM_DataView, L"Data View");
    }
    else
    {
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDM_DataView, L"Data View");
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDM_KinectView, L"Kinect View");
    }*/
    SetForegroundWindow(hWnd);
}

Kinect2DRenderer::Kinect2DRenderer()
{
}


Kinect2DRenderer::~Kinect2DRenderer()
{
}

void Kinect2DRenderer::Init(Kinect2DGraphRenderer * pKinect2DGraphRenderer)
{
    m_pKinect2DGraphRenderer = pKinect2DGraphRenderer;
}

void Kinect2DRenderer::Update(FrameManager * frameManager, const vector<KINECTDATA*>* kinectData, Object * obj, RenderTargets * tRenderTargets, float fDeltaTime)
{
    assert(frameManager);
    assert(kinectData);
    Form * pForm = (Form *)obj;

    if (KEYDOWN(HASH_MouseRButton))
    {
        UINT message = 0;
        WPARAM wParam = 0;
        LPARAM lParam = 0;
        HMENU hPopupMenu = CreatePopupMenu();

        if (kinectData)
        {
            POINT point;// GET_MOUSECLIENTPOS;
            GetCursorPos(&point);
            ScreenToClient(GET_WINDOWHANDLE, &point);
            auto graphData = GET_SINGLE(KinectDataManager)->GetGraphData();
            _SIZE size = pForm->GetRealClientSize();
            _SIZE dpi = pForm->GetDPI();
            size = size * dpi;
            _SIZE pos = pForm->GetPosDPI();
            point.x -= pos.x;
            point.y -= pos.y;

            //GetCursorPos(&point);
            //ScreenToClient(mhCurrent, &point);
            point.x = (LONG)(((float)point.x * cDepthWidth) / size.x);
            point.y = (LONG)(((float)point.y * cDepthHeight) / size.y);

            auto frameIndex = frameManager->GetIndex();
            mJointIndex = JointType_Count;
            size_t errorRange = 6;
            if (kinectData->size() > 0)
            {
                for (size_t i = 0; i < JointType_Count; ++i)
                {
                    if ((point.x + errorRange > (kinectData->at(frameIndex)->m2DLocation[i].x))
                        && (point.x - errorRange < (kinectData->at(frameIndex)->m2DLocation[i].x))
                        && (point.y + errorRange > (kinectData->at(frameIndex)->m2DLocation[i].y))
                        && (point.y - errorRange < (kinectData->at(frameIndex)->m2DLocation[i].y)))
                    {   // if문이 히트하면 끗
                        mJointIndex = i;
                        HMENU stepAnalyzerPopupMenu = CreatePopupMenu();

                        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING | MF_POPUP, (UINT_PTR)stepAnalyzerPopupMenu, L"그래프에 그리기");
                        InsertMenu(stepAnalyzerPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_NEWGRAPHADD, L"새 그래프에 그리기");
                        for (int j = graphData.size(); j > 0; --j)
                        {
                            wstring str = cJointTypeToString[mJointIndex] + L"를 " + to_wstring(j) + L"번 그래프에 그리기";
                            InsertMenu(stepAnalyzerPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_DRAWGRAPH + j - 1, (LPCWSTR)str.c_str());
                        }
                        break;
                    }
                }
            }
        }

        POINT point2;
        GetCursorPos(&point2);
        WinProcPopupMenu(hPopupMenu, GET_WINDOWHANDLE, message, wParam, lParam);
        TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, point2.x, point2.y, 0, GET_WINDOWHANDLE, nullptr);
    }
}

void Kinect2DRenderer::Render(Object * obj, ColorF::Enum color, RenderTargets * tRenderTargets, float fDeltaTime, KINECTDATA * kinectData)
{
	GET_SINGLE(KinectManager)->GetKinectBody()->DrawTrackedBody2(obj,
		tRenderTargets->pRT2D, tRenderTargets->pD2DHelper->GetBrush(color),
		tRenderTargets->pD2DHelper->GetBrush(D2D1::ColorF::Gray),
		kinectData->mCapturedJoints, kinectData->m2DLocation);

	GET_SINGLE(KinectManager)->GetKinectBody()->DrawAngle(obj,
		tRenderTargets, tRenderTargets->pD2DHelper->GetBrush(D2D1::ColorF::White),
		kinectData->m2DLocation, kinectData->mAngle);

	GET_SINGLE(KinectManager)->GetKinectBody()->DrawDistance(obj,
		tRenderTargets, tRenderTargets->pD2DHelper->GetBrush(D2D1::ColorF::White),
		kinectData->m2DLocation, kinectData->mDistance);

	GET_SINGLE(KinectManager)->GetKinectBody()->DrawGravity(obj,
		tRenderTargets, tRenderTargets->pD2DHelper->GetBrush(D2D1::ColorF::Black),
		kinectData->CentryGravity, tRenderTargets->pD2DHelper->GetBrush(D2D1::ColorF::White), 
		tRenderTargets->pD2DHelper->GetBrush(color));
}

LRESULT Kinect2DRenderer::UIProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        auto dataManager = GET_SINGLE(KinectDataManager);
        size_t commandWParam = LOWORD(wParam);
        if (mJointIndex < JointType_Count)
        {
            if (commandWParam == ID_NEWGRAPHADD)
            {
                dataManager->InsertGraphData(dataManager->GetGraphData().size());
                dataManager->SetGraphData(dataManager->GetGraphData().size() - 1, mIsCompar, mJointIndex);
                m_pKinect2DGraphRenderer->OnGraphDataChanged();
            }
            else if ((commandWParam > ID_DRAWGRAPH - 1) && (ID_NEWGRAPHADD > commandWParam))
            {
                dataManager->SetGraphData(commandWParam - ID_DRAWGRAPH, mIsCompar, mJointIndex);
                m_pKinect2DGraphRenderer->OnGraphDataChanged();
            }
            mJointIndex = JointType_Count;
        }
    }
    }
    return 0;
}
