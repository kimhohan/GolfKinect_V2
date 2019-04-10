#include "Project4KPScene.h"
#include "../HashTable.h"
#include "../Object/Form.h"
#include "../Object/UIPanel.h"
#include "../Object/UIButton.h"
#include "../Collider/ColliderRect.h"
#include "../Core/Core.h"
#include "../Core/DeviceManager.h"
#include "../Core/DirectX11Helper.h"
#include "../Core/Direct2DHelper.h"
#include "../Kinect/Kinect2DRenderer.h"
#include "../Kinect/Kinect2DGraphRenderer.h"
#include "../Kinect/Kinect3DRenderer.h"
#include "../Kinect/KinectDataManager.h"
#include "../Kinect/KinectManager.h"
#include "../OpenPose/OpenPoseDataManager.h"
#include "../OpenPose/OpenPoseGraphRenderer.h"
#include "../Core/InputManager.h"
#include "../Core/FormManager.h"
#include "../Scene/SceneManager.h"
#include "../Core/DirectWriteHelper.h"

#include "../Object/Player.h"

#include <Windows.h> 

Kinect2DRenderer * Project4KPScene::m_pKinect2DRenderer = nullptr;

Project4KPScene::Project4KPScene()
{
}

Project4KPScene::~Project4KPScene()
{
	SAFE_DELETE(m_pDirectX11Helper);
	SAFE_DELETE(m_pKinect2DRenderer);
	SAFE_DELETE(m_pKinect2DGraphRenderer);
	SAFE_DELETE(m_pKinect3DRenderer);
	SAFE_DELETE(m_pKinect3DCmpRenderer);

	GET_SINGLE(KinectDataManager)->Release();
	DESTROY_SINGLE(KinectDataManager);
	GET_SINGLE(KinectManager)->Release();
	DESTROY_SINGLE(KinectManager);
}

bool Project4KPScene::Init()
{
	if (!Scene::Init())
		return false;

	Layer * pLayer = FindLayer(HASH_UI);
	Scene * pScene = GET_SINGLE(SceneManager)->GetScene();

	GET_SINGLE(KinectManager)->Initialize();
	GET_SINGLE(KinectDataManager)->Initialize();

    {
        Form * pForm = GET_SINGLE(FormManager)->CreateEngineForm(HASH_Form + 100, pLayer,0);
        pForm->SetTitle(L"그래프");

        pForm->SetRenderCallback(this, &Project4KPScene::RenderGraph);
        GET_SINGLE(FormManager)->ShowEngineForm(HASH_Form + 100);
        SAFE_RELEASE(pForm);
    }

	{
		Form * pForm = GET_SINGLE(FormManager)->CreateEngineForm(HASH_Form, pLayer, 1);
		pForm->SetTitle(L"원본군 2D");

		pForm->SetRenderCallback(this, &Project4KPScene::RenderOrigin2D);
		GET_SINGLE(FormManager)->ShowEngineForm(HASH_Form);
		SAFE_RELEASE(pForm);
	}

	{
		Form * pForm = GET_SINGLE(FormManager)->CreateEngineForm(HASH_Form + 101, pLayer, 0, 1);
		pForm->SetTitle(L"대조군 2D");

		pForm->SetRenderCallback(this, &Project4KPScene::RenderCmp2D);
		GET_SINGLE(FormManager)->ShowEngineForm(HASH_Form + 101);
		SAFE_RELEASE(pForm);
	}

    {
        /*Form * pForm = GET_SINGLE(FormManager)->CreateEngineForm(HASH_Form + 102, pLayer);
        pForm->SetTitle(L"타임 라인");

        pForm->SetRenderCallback(this, &Project4KPScene::RenderTimeLine);
        pForm->SetRenderEndCallback(this, &Project4KPScene::RenderEndTimeLine);
        GET_SINGLE(FormManager)->ShowEngineForm(HASH_Form + 102);
        SAFE_RELEASE(pForm);*/
    }
	{
		// 현재 프로그램의 작업(Working Directory) 경로를 저장하기 위해서 배열을 선언한다.
		wstring path;
		path.resize(MAX_PATH);
		// 현재 프로그램의 작업 경로를 얻는다.
		GetCurrentDirectory(MAX_PATH, (LPWSTR)path.c_str());

		OPENFILENAME filename;
		static TCHAR lpstFile[100] = L""; // 에러메시지, 선택된 파일이름
		const int nMaxFile = 100;

		memset(&filename, 0, sizeof(OPENFILENAME));
		filename.lStructSize = sizeof(OPENFILENAME);
		filename.hwndOwner = GET_WINDOWHANDLE;
		filename.lpstrFilter = L"dat 파일 \0*.dat\0모든 파일(*.*)\0*.*\0";
		filename.lpstrFile = lpstFile;
		filename.nMaxFile = nMaxFile;
		//filename.lpstrInitialDir = _T("C:");
		if (GetOpenFileName(&filename))
		{
			SetCurrentDirectory(path.c_str());
			GET_SINGLE(KinectDataManager)->ReleaseLoadedKinectData();
			GET_SINGLE(KinectDataManager)->LoadKinectData(lpstFile);
		}
		SetCurrentDirectory(path.c_str());
        if (GetOpenFileName(&filename))
        {
            SetCurrentDirectory(path.c_str());
            GET_SINGLE(KinectDataManager)->ReleaseLoadedKinectComparativeData();
            GET_SINGLE(KinectDataManager)->LoadKinectComparativeData(lpstFile);
        }
        SetCurrentDirectory(path.c_str());
	}

    m_pKinect2DGraphRenderer = new Kinect2DGraphRenderer;

	m_pKinect2DRenderer = new Kinect2DRenderer;
	m_pKinect2DRenderer->Init(m_pKinect2DGraphRenderer);
    m_pKinect2DGraphRenderer->Init();

    m_pKinect2DGraphRenderer->OnDataFileLoaded();
    m_pKinect2DGraphRenderer->OnComparativeDataFileLoaded();

    m_pKinect3DRenderer = new Kinect3DRenderer;
    {
        Form * pForm = GET_SINGLE(FormManager)->CreateEngineForm(HASH_Form + 103, pLayer, 1);
        pForm->SetTitle(L"3D 원본군");

        pForm->SetRenderCallback(this, &Project4KPScene::Render3D);
        GET_SINGLE(FormManager)->ShowEngineForm(HASH_Form + 103);
        m_pKinect3DRenderer->Init(pForm, GET_SINGLE(KinectDataManager)->GetFrameManager(), GET_SINGLE(KinectDataManager)->GetKinectData());
        SAFE_RELEASE(pForm);
    }

    m_pKinect3DCmpRenderer = new Kinect3DRenderer;
    {
        Form * pForm = GET_SINGLE(FormManager)->CreateEngineForm(HASH_Form + 104, pLayer, 1, 1);
        pForm->SetTitle(L"3D 대조군");

        pForm->SetRenderCallback(this, &Project4KPScene::Render3DCmp);
        GET_SINGLE(FormManager)->ShowEngineForm(HASH_Form + 104);
        m_pKinect3DCmpRenderer->Init(pForm, GET_SINGLE(KinectDataManager)->GetComparativeFrameManager(), GET_SINGLE(KinectDataManager)->GetKinectComparativeData());
        SAFE_RELEASE(pForm);
    }

	GET_SINGLE(OpenPoseDataManager)->Init(L"");
	m_pOpenPoseGraphRenderer = new OpenPoseGraphRenderer;
	m_pOpenPoseGraphRenderer->Init();
	m_pOpenPoseGraphRenderer->OnDataFileLoaded();
	{
		Form * pForm = GET_SINGLE(FormManager)->CreateEngineForm(HASH_Form + 106, pLayer, 0);
		pForm->SetTitle(L"OpenPose 그래프");
		pForm->SetRenderCallback(this, &Project4KPScene::RenderOpenPoseGraph);
		GET_SINGLE(FormManager)->ShowEngineForm(HASH_Form + 106);
		SAFE_RELEASE(pForm);
	}

	{
		Form * pForm = GET_SINGLE(FormManager)->CreateEngineForm(HASH_Form + 105, pLayer, 1, 1);
		pForm->SetTitle(L"OpenPose 2D");

		pForm->SetRenderCallback(this, &Project4KPScene::RenderOpenPose2D);
		GET_SINGLE(FormManager)->ShowEngineForm(HASH_Form + 105);
		SAFE_RELEASE(pForm);
	}
	
	//<<<<<<< HEAD
	//=======
//>>>>>>> ba8b6f9542dd89db3f9bb00b599bcbe472d13db2
	return true;
}

void Project4KPScene::Input(float fDeltaTime)
{
    if (KEYDOWN(HASH_Ctrl))
    {
    }
	if (KEYDOWN(HASH_Shift))
	{
		Layer * pLayer = FindLayer(HASH_UI);
		{
			if (FormDocker::m_pFocusForm)
			{
                // 외부 폼일때 내부로
				if (FormDocker::m_pFocusForm->m_hWnd)
				{
					HWND hWnd = FormDocker::m_pFocusForm->m_hWnd;
			
					FormDocker * pFormDocker = GET_SINGLE(FormManager)->GetFormDocker(GET_WINDOWHANDLE);
					GET_SINGLE(FormManager)->MoveEngineForm(FormDocker::m_pFocusForm,
						pFormDocker);

					FormDocker::m_pFocusForm->FrameCallbackPressIn(0.f);

					GET_SINGLE(FormManager)->ShowEngineForm(FormDocker::m_pFocusForm->GetHashTag());
					DestroyWindow(hWnd);
				}
                // 내부 폼일때 도킹
				else
				{
                    if (FormDocker::m_pFocusForm->m_pCurrentFormDocker->m_listForm.size() == 1)
                    {
                        FormDocker * pFormDocker = nullptr;
                        GET_SINGLE(FormManager)->GetFormDocker(FormDocker::m_pFocusForm, &pFormDocker);
                        GET_SINGLE(FormManager)->MoveEngineForm(FormDocker::m_pFocusForm,
                            pFormDocker);
                    }
                    else
                    {
                        FormDocker * pFormDocker = nullptr;
                        int ret = GET_SINGLE(FormManager)->GetFormDocker(FormDocker::m_pFocusForm, &pFormDocker);
                        if (pFormDocker->m_listForm.size() > 1)
                        {
                            pFormDocker = GET_SINGLE(FormManager)->CreateFormDocker(GET_WINDOWHANDLE, ret);
                        }
                        GET_SINGLE(FormManager)->MoveEngineForm(FormDocker::m_pFocusForm,
                            pFormDocker);
                    }
				}
			}
			GET_SINGLE(Core)->OnResize(GET_WINDOWHANDLE);
		}
	}
}

void Project4KPScene::RenderOrigin2D(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime)
{
	Form * pForm = (Form *)obj;
    _SIZE tSize = pForm->GetSize();
    POSITION tPos = pForm->GetPos();
	//if(pForm->m_pCurrentFormDocker->m_pFocusForm != pForm) return;

	int index = (int)GET_SINGLE(KinectDataManager)->GetFrameManager()->GetIndex();
	auto kinectData = GET_SINGLE(KinectDataManager)->GetKinectData();
	if (KEYPRESS(HASH_VK_LEFT))
	{
		GET_SINGLE(KinectDataManager)->GetFrameManager()->SetIndex(index - 1);
	}
	if (KEYPRESS(HASH_VK_RIGHT))
	{
		GET_SINGLE(KinectDataManager)->GetFrameManager()->SetIndex(index + 1);
	}

    ID2D1SolidColorBrush * brush = tRenderTargets->pD2DHelper->GetBrush(ColorF::Enum(0x1E1E1E));
    tRenderTargets->pRT2D->FillRectangle(
        D2D1_RECT_F{ tPos.x, tPos.y, tPos.x + tSize.x, tPos.y + tSize.y }, brush);

    if (pForm->m_pCurrentFormDocker->m_pFocusForm == pForm)
    {
        m_pKinect2DRenderer->Update(GET_SINGLE(KinectDataManager)->GetFrameManager(), kinectData, obj, tRenderTargets, fDeltaTime);
    }
	m_pKinect2DRenderer->Render(obj, D2D1::ColorF::Green, tRenderTargets, fDeltaTime, kinectData->at(index));
}

//  test 만들어보기
void Project4KPScene::TestProgram(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime)
{
	wstring m_strText = L"김호한";

	if (GET_SINGLE(InputManager)->KeyPress(HASH_MouseLButton) == true)
	{
		POSITION pt;
		pt = GET_SINGLE(InputManager)->GetMouseClientPos();
		D2D1_RECT_F layout = D2D1::RectF(pt.x, pt.y, pt.x + 40, pt.y + 40);

		tRenderTargets->pRT2D->DrawText(m_strText.c_str(), m_strText.size(), tRenderTargets->pD2DHelper->GetDirectWriteHelper()->GetTextFormat(), layout,
		tRenderTargets->pD2DHelper->GetBrush(ColorF::Enum::White));
	}
}


void Project4KPScene::RenderRectangle(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime)
{
	Form * pForm = (Form *)obj;
	ID2D1SolidColorBrush * brush = tRenderTargets->pD2DHelper->GetBrush(ColorF::Enum::White);
	POSITION tPos = pForm->GetPos();
	_SIZE tSize = pForm->GetSize();
	tRenderTargets->pRT2D->DrawRectangle(
		D2D1_RECT_F{ tPos.x, tPos.y, tPos.x + 100, tPos.y + 100}, brush);
}

void Project4KPScene::RenderCmp2D(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime)
{
    Form * pForm = (Form *)obj;
    _SIZE tSize = pForm->GetSize();
    POSITION tPos = pForm->GetPos();
    //if(pForm->m_pCurrentFormDocker->m_pFocusForm != pForm) return;

    int index = (int)GET_SINGLE(KinectDataManager)->GetComparativeFrameManager()->GetIndex();
    auto kinectData = GET_SINGLE(KinectDataManager)->GetKinectComparativeData();
    if (KEYPRESS(HASH_VK_LEFT))
    {
        GET_SINGLE(KinectDataManager)->GetComparativeFrameManager()->SetIndex(index - 1);
    }
    if (KEYPRESS(HASH_VK_RIGHT))
    {
        GET_SINGLE(KinectDataManager)->GetComparativeFrameManager()->SetIndex(index + 1);
    }

    ID2D1SolidColorBrush * brush = tRenderTargets->pD2DHelper->GetBrush(ColorF::Enum(0x1E1E1E));
    tRenderTargets->pRT2D->FillRectangle(
        D2D1_RECT_F{ tPos.x, tPos.y, tPos.x + tSize.x, tPos.y + tSize.y }, brush);

    if (pForm->m_pCurrentFormDocker->m_pFocusForm == pForm)
    {
        m_pKinect2DRenderer->Update(GET_SINGLE(KinectDataManager)->GetFrameManager(), kinectData, obj, tRenderTargets, fDeltaTime);
    }
    m_pKinect2DRenderer->Render(obj, D2D1::ColorF::DarkCyan, tRenderTargets, fDeltaTime, kinectData->at(index));
}

void Project4KPScene::RenderGraph(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime)
{
    Form * pForm = (Form *)obj;
    _SIZE tSize = pForm->GetSize();
    POSITION tPos = pForm->GetPos();
    ID2D1SolidColorBrush * brush = tRenderTargets->pD2DHelper->GetBrush(ColorF::Enum(0x1E1E1E));
    tRenderTargets->pRT2D->FillRectangle(
        D2D1_RECT_F{ tPos.x, tPos.y, tPos.x + tSize.x, tPos.y + tSize.y }, brush);

    m_pKinect2DGraphRenderer->Update(obj, fDeltaTime);
    m_pKinect2DGraphRenderer->Render(obj, tRenderTargets, fDeltaTime);
}

void Project4KPScene::RenderOpenPoseGraph(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime)
{
	Form * pForm = (Form *)obj;
	_SIZE tSize = pForm->GetSize();
	POSITION tPos = pForm->GetPos();
	ID2D1SolidColorBrush * brush = tRenderTargets->pD2DHelper->GetBrush(ColorF::Enum(0x1E1E1E));
	tRenderTargets->pRT2D->FillRectangle(
		D2D1_RECT_F{ tPos.x, tPos.y, tPos.x + tSize.x, tPos.y + tSize.y }, brush);

	m_pOpenPoseGraphRenderer->Update(obj, fDeltaTime);
	m_pOpenPoseGraphRenderer->Render(obj, tRenderTargets, fDeltaTime);
}

bool bRender3DInit = false;
void Project4KPScene::Render3D(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime)
{
    Form * pForm = (Form *)obj;
    UINT iNum = 1;
    D3D11_VIEWPORT tOldScreenViewport = { 0, };
    D3D11_VIEWPORT tNewScreenViewport = { 0, };
    tNewScreenViewport.Width = pForm->GetRealClientSize().x * pForm->GetDPI().x;
    tNewScreenViewport.Height = pForm->GetRealClientSize().y * pForm->GetDPI().y;
    tNewScreenViewport.TopLeftX = pForm->GetPosDPI().x;
    tNewScreenViewport.TopLeftY = pForm->GetPosDPI().y;
    tNewScreenViewport.MaxDepth = 1.f;
    tNewScreenViewport.MinDepth = 0.f;

    tRenderTargets->ptD3DHelper->pD3DImmediateContext->RSGetViewports(&iNum, &tOldScreenViewport);
    tRenderTargets->ptD3DHelper->pD3DImmediateContext->RSSetViewports(iNum, &tNewScreenViewport);
    {
        if (!bRender3DInit)
        {
            m_pKinect3DRenderer->OnDataFileLoaded(tRenderTargets);
            bRender3DInit = true;
        }
        m_pKinect3DRenderer->Update(fDeltaTime);
        m_pKinect3DRenderer->Render(obj, tRenderTargets, fDeltaTime);
    }
    tRenderTargets->ptD3DHelper->pD3DImmediateContext->RSSetViewports(iNum, &tOldScreenViewport);
}

bool bRender3DCmpInit = false;
void Project4KPScene::Render3DCmp(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime)
{
    Form * pForm = (Form *)obj;
    UINT iNum = 1;
    D3D11_VIEWPORT tOldScreenViewport = { 0, };
    D3D11_VIEWPORT tNewScreenViewport = { 0, };
    tNewScreenViewport.Width = pForm->GetRealClientSize().x * pForm->GetDPI().x;
    tNewScreenViewport.Height = pForm->GetRealClientSize().y * pForm->GetDPI().y;
    tNewScreenViewport.TopLeftX = pForm->GetPosDPI().x;
    tNewScreenViewport.TopLeftY = pForm->GetPosDPI().y;
    tNewScreenViewport.MaxDepth = 1.f;
    tNewScreenViewport.MinDepth = 0.f;

    tRenderTargets->ptD3DHelper->pD3DImmediateContext->RSGetViewports(&iNum, &tOldScreenViewport);
    tRenderTargets->ptD3DHelper->pD3DImmediateContext->RSSetViewports(iNum, &tNewScreenViewport);
    {
        if (!bRender3DCmpInit)
        {
            m_pKinect3DCmpRenderer->OnDataFileLoaded(tRenderTargets);
            bRender3DCmpInit = true;
        }
        m_pKinect3DCmpRenderer->Update(fDeltaTime);
        m_pKinect3DCmpRenderer->Render(obj, tRenderTargets, fDeltaTime);
    }
    tRenderTargets->ptD3DHelper->pD3DImmediateContext->RSSetViewports(iNum, &tOldScreenViewport);
}

static bool bRenderTimeLineInit = false;
void Project4KPScene::RenderTimeLine(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime)
{
    //int index = (int)GET_SINGLE(KinectDataManager)->GetComparativeFrameManager()->GetIndex();
    Form * pForm = (Form *)obj;
    _SIZE FormSize = pForm->GetSize();
    int iBtnSizeY = 15;
    Layer * pLayer = FindLayer(HASH_UI);
    int iMaxFrameData = (int)GET_SINGLE(KinectDataManager)->GetFrameManager()->GetSize();
    int iMaxCmpFrameData = (int)GET_SINGLE(KinectDataManager)->GetComparativeFrameManager()->GetSize();
    int iStartIndex = 0;
    int iCmpStartIndex = 0;
    static int iMaxIndex = 0;
    // EKinectStepType::Size
    const UINT32 * mKinectDataStep = GET_SINGLE(KinectDataManager)->GetKinectSteps();
    const UINT32 * mKinectDataCmpStep = GET_SINGLE(KinectDataManager)->GetKinectComparativeSteps();
    
    _SIZE tDPI = pForm->GetDPI();

    UIButton * m_pBtn = nullptr;
    ColliderRect * m_pRCBtn = nullptr;

    if (!bRenderTimeLineInit)
    {
        for (int i = 0; i < iMaxFrameData; ++i)
        {
            StringHash strHash = ToStringHash("TimeLineBtn") + i;
            m_pBtn = Object::CreateObject<UIButton>(strHash, pLayer);
            m_pBtn->SetText(to_wstring(i));
            m_pBtn->SetCallbackDown2(this, &Project4KPScene::TimeLineCallbackDown);
            if (mKinectDataStep[(UINT32)EGolfStepType::Address] == i || 
                mKinectDataStep[(UINT32)EGolfStepType::TopOfSwing] == i ||
                mKinectDataStep[(UINT32)EGolfStepType::Impact] == i ||
                mKinectDataStep[(UINT32)EGolfStepType::Finish] == i ||
                mKinectDataStep[(UINT32)EGolfStepType::End] == i
                )
            {
                m_pBtn->SetBrushColorIdle(ColorF::Red);
            }
            m_pRCBtn = (ColliderRect*)m_pBtn->GetCollider(HASH_ButtonBody);

            SAFE_RELEASE(m_pBtn);
            SAFE_RELEASE(m_pRCBtn);
        }
        for (int i = 0; i < iMaxCmpFrameData; ++i)
        {
            StringHash strHash = ToStringHash("TimeLineBtn2") + i;
            m_pBtn = Object::CreateObject<UIButton>(strHash, pLayer);
            m_pBtn->SetText(L"C" + to_wstring(i));
            m_pBtn->SetCallbackDown2(this, &Project4KPScene::TimeLineCallbackDown);

            if (mKinectDataCmpStep[(UINT32)EGolfStepType::Address] == i ||
                mKinectDataCmpStep[(UINT32)EGolfStepType::TopOfSwing] == i ||
                mKinectDataCmpStep[(UINT32)EGolfStepType::Impact] == i ||
                mKinectDataCmpStep[(UINT32)EGolfStepType::Finish] == i ||
                mKinectDataCmpStep[(UINT32)EGolfStepType::End] == i
                )
            {
                m_pBtn->SetBrushColorIdle(ColorF::Blue);
            }

            m_pRCBtn = (ColliderRect*)m_pBtn->GetCollider(HASH_ButtonBody);

            SAFE_RELEASE(m_pBtn);
            SAFE_RELEASE(m_pRCBtn);
        }
        bRenderTimeLineInit = true;
    }

    //if (iMaxIndex != iTempMaxIndex)
    {
        static int iAddIndex = 0;

        int iMaxFrame = iMaxFrameData > iMaxCmpFrameData ? iMaxFrameData : iMaxCmpFrameData;
        int iPageCount = iMaxIndex = FormSize.y / (iBtnSizeY * tDPI.y) + 1;
        iPageCount -= 1;
        iMaxIndex = iMaxIndex > iMaxFrame ? iMaxFrame : iMaxIndex;
        iMaxIndex = iMaxIndex + iAddIndex > iMaxFrame ? iMaxFrame : iMaxIndex + iAddIndex;
        
        if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        {
            if(iAddIndex < iMaxFrame - iPageCount)
                ++iAddIndex;
        }
        if (GetAsyncKeyState(VK_UP) & 0x8000)
        {
            if (iAddIndex > 0)
                --iAddIndex;
        }

        iStartIndex += iAddIndex;
        {
            for (int i = 0; i < iStartIndex; ++i)
            {
                StringHash strHash = ToStringHash("TimeLineBtn") + i;
                m_pBtn = (UIButton *)Object::FindObject(strHash);
                if (m_pBtn)
                {
                    m_pBtn->SetRender(false);
                    m_pBtn->SetUpdate(false);
                    m_pBtn->GetCollider(NULL);
                    SAFE_RELEASE(m_pBtn);
                }
            }

            for (int i = 0; i < iStartIndex; ++i)
            {
                StringHash strHash = ToStringHash("TimeLineBtn2") + i;
                m_pBtn = (UIButton *)Object::FindObject(strHash);
                if (m_pBtn)
                {
                    m_pBtn->SetRender(false);
                    m_pBtn->SetUpdate(false);
                    m_pBtn->GetCollider(NULL);

                    SAFE_RELEASE(m_pBtn);
                }
            }

            for (int i = iStartIndex; i < iMaxIndex; ++i)
            {
                _SIZE tDPI = pForm->GetDPI();
                StringHash strHash = ToStringHash("TimeLineBtn") + i;
                m_pBtn = (UIButton *)Object::FindObject(strHash);
                if (m_pBtn)
                {
                    m_pBtn->SetSize(FormSize.x * 0.5f, iBtnSizeY * tDPI.y);
                    m_pBtn->SetPos(pForm->GetPos().x, pForm->GetPos().y + (i - iAddIndex) * iBtnSizeY * tDPI.y);

                    m_pRCBtn = (ColliderRect*)m_pBtn->GetCollider(HASH_ButtonBody);
                    m_pRCBtn->SetSize(m_pBtn->GetSize());
                    m_pBtn->SetRender(true);
                    m_pBtn->SetUpdate(true);
                    SAFE_RELEASE(m_pBtn);
                    SAFE_RELEASE(m_pRCBtn);
                }

                tDPI = pForm->GetDPI();
                strHash = ToStringHash("TimeLineBtn2") + i;
                m_pBtn = (UIButton *)Object::FindObject(strHash);
                if (m_pBtn)
                {
                    float fWidth = FormSize.x * 0.5f;
                    m_pBtn->SetSize(fWidth, iBtnSizeY * tDPI.y);
                    m_pBtn->SetPos(pForm->GetPos().x + fWidth, pForm->GetPos().y + (i - iAddIndex) * iBtnSizeY * tDPI.y);

                    m_pRCBtn = (ColliderRect*)m_pBtn->GetCollider(HASH_ButtonBody);
                    m_pRCBtn->SetSize(m_pBtn->GetSize());
                    m_pBtn->SetRender(true);
                    m_pBtn->SetUpdate(true);
                    SAFE_RELEASE(m_pBtn);
                    SAFE_RELEASE(m_pRCBtn);
                }
            }

            for (int i = iMaxIndex; i < iMaxFrameData; ++i)
            {
                StringHash strHash = ToStringHash("TimeLineBtn") + i;
                m_pBtn = (UIButton *)Object::FindObject(strHash);
                if (m_pBtn)
                {
                    m_pBtn->SetRender(false);
                    m_pBtn->SetUpdate(false);
                    m_pBtn->GetCollider(NULL);
                    SAFE_RELEASE(m_pBtn);
                }
            }

            for (int i = iMaxIndex; i < iMaxCmpFrameData; ++i)
            {
                StringHash strHash = ToStringHash("TimeLineBtn2") + i;
                m_pBtn = (UIButton *)Object::FindObject(strHash);
                if (m_pBtn)
                {
                    m_pBtn->SetRender(false);
                    m_pBtn->SetUpdate(false);
                    m_pBtn->GetCollider(NULL);
                    SAFE_RELEASE(m_pBtn);
                }
            }
        }
    }
}

void Project4KPScene::RenderEndTimeLine(Object * obj, float fDeltaTime)
{
    if (!bRenderTimeLineInit) return;
    Form * pForm = (Form *)obj;
    _SIZE FormSize = pForm->GetSize();
    int iBtnSizeY = 15;
    Layer * pLayer = FindLayer(HASH_UI);
    int iMaxFrameData = (int)GET_SINGLE(KinectDataManager)->GetFrameManager()->GetSize();
    int iMaxCmpFrameData = (int)GET_SINGLE(KinectDataManager)->GetComparativeFrameManager()->GetSize();
    int iStartIndex = 0;
    int iCmpStartIndex = 0;
    static int iMaxIndex = 0;
    // EKinectStepType::Size
    const UINT32 * mKinectDataStep = GET_SINGLE(KinectDataManager)->GetKinectSteps();
    const UINT32 * mKinectDataCmpStep = GET_SINGLE(KinectDataManager)->GetKinectComparativeSteps();

    _SIZE tDPI = pForm->GetDPI();

    UIButton * m_pBtn = nullptr;
    ColliderRect * m_pRCBtn = nullptr;
    if (pForm->m_pCurrentFormDocker->m_pCurrentForm != pForm)
    {
        bRenderTimeLineInit = false;
        for (int i = 0; i < iMaxFrameData; ++i)
        {
            StringHash strHash = ToStringHash("TimeLineBtn") + i;
            m_pBtn = (UIButton *)Object::FindObject(strHash);
            if (m_pBtn)
            {
                m_pBtn->Die();
                SAFE_RELEASE(m_pBtn);
            }
        }

        for (int i = 0; i < iMaxCmpFrameData; ++i)
        {
            StringHash strHash = ToStringHash("TimeLineBtn2") + i;
            m_pBtn = (UIButton *)Object::FindObject(strHash);
            if (m_pBtn)
            {
                m_pBtn->Die();
                SAFE_RELEASE(m_pBtn);
            }
        }
        return;
    }
}

void Project4KPScene::TimeLineCallbackDown(UIButton * pBtn, float fDeltaTime)
{
    wstring str = pBtn->GetText();
    int iBtnIndex = 0;
    if (str.find('C') != std::string::npos)
    {
        str[0] = '0';
        iBtnIndex = std::stoi(str.c_str());
        GET_SINGLE(KinectDataManager)->GetComparativeFrameManager()->SetIndex(iBtnIndex);
    }
    else
    {
        iBtnIndex = std::stoi(str.c_str());
        GET_SINGLE(KinectDataManager)->GetFrameManager()->SetIndex(iBtnIndex);
    }
}

D2D1_POINT_2F BodyToScreen(Object * obj, const D2D1_POINT_2F & bodyPoint, int width, int height)
{
    // Calculate the body's position on the screen
    POSITION tPos = obj->GetPos();

    float screenPointX = tPos.x + static_cast<float>(bodyPoint.x * width) / (cDepthWidth * 0.7f);
    float screenPointY = tPos.y + static_cast<float>(bodyPoint.y * height) / (cDepthHeight * 0.7f);

    return D2D1::Point2F(screenPointX, screenPointY);
}

void Project4KPScene::RenderOpenPose2D(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime)
{
    Form * pForm = (Form *)obj;
    _SIZE tSize = pForm->GetSize();
    POSITION tPos = pForm->GetPos();

    const vector<OpenPoseData> * p_vecOpenPoseFrameData = GET_SINGLE(OpenPoseDataManager)->GetData();
    size_t index = GET_SINGLE(OpenPoseDataManager)->GetIndex();
    const OpenPoseData tOpenPoseData = p_vecOpenPoseFrameData->at(index);

    static size_t iTempIndex = 0;
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        ++iTempIndex;
    }
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        if(iTempIndex > 0)
            --iTempIndex;
    }
    GET_SINGLE(OpenPoseDataManager)->SetIndex(iTempIndex);

    // EOpenPoseKey; https://github.com/CMU-Perceptual-Computing-Lab/openpose/blob/master/doc/output.md#keypoint-ordering
    for (int i = 0; i < 4; ++i)
    {
        D2D1_POINT_2F pJointPoints = { tOpenPoseData.m2DLocation[i].x, tOpenPoseData.m2DLocation[i].y };
        D2D1_POINT_2F pJointPoints2 = { tOpenPoseData.m2DLocation[i+1].x, tOpenPoseData.m2DLocation[i+1].y };

        if ((pJointPoints.x != 0 && pJointPoints.y != 0) && (pJointPoints2.x != 0 && pJointPoints2.y != 0))
        {
            pJointPoints = BodyToScreen(obj, pJointPoints, tSize.x, tSize.y);
            pJointPoints2 = BodyToScreen(obj, pJointPoints2, tSize.x, tSize.y);
            tRenderTargets->pRT2D->DrawLine(pJointPoints, pJointPoints2, tRenderTargets->pD2DHelper->GetBrush(ColorF::Orange), 5.f);
        }
    }

    // 1 to 5
    {
        int i = 1, i2 = 5;
        D2D1_POINT_2F pJointPoints = { tOpenPoseData.m2DLocation[i].x, tOpenPoseData.m2DLocation[i].y };
        D2D1_POINT_2F pJointPoints2 = { tOpenPoseData.m2DLocation[i2].x, tOpenPoseData.m2DLocation[i2].y };
        if ((pJointPoints.x != 0 && pJointPoints.y != 0) && (pJointPoints2.x != 0 && pJointPoints2.y != 0))
        {
            pJointPoints = BodyToScreen(obj, pJointPoints, tSize.x, tSize.y);
            pJointPoints2 = BodyToScreen(obj, pJointPoints2, tSize.x, tSize.y);
            tRenderTargets->pRT2D->DrawLine(pJointPoints, pJointPoints2, tRenderTargets->pD2DHelper->GetBrush(ColorF::GreenYellow), 5.f);
        }
    }

    for (int i = 5; i < 7; ++i)
    {
        D2D1_POINT_2F pJointPoints = { tOpenPoseData.m2DLocation[i].x, tOpenPoseData.m2DLocation[i].y };
        D2D1_POINT_2F pJointPoints2 = { tOpenPoseData.m2DLocation[i+1].x, tOpenPoseData.m2DLocation[i+1].y };
        if ((pJointPoints.x != 0 && pJointPoints.y != 0) && (pJointPoints2.x != 0 && pJointPoints2.y != 0))
        {
            pJointPoints = BodyToScreen(obj, pJointPoints, tSize.x, tSize.y);
            pJointPoints2 = BodyToScreen(obj, pJointPoints2, tSize.x, tSize.y);
            tRenderTargets->pRT2D->DrawLine(pJointPoints, pJointPoints2, tRenderTargets->pD2DHelper->GetBrush(ColorF::Green), 5.f);
        }
    }

    // 1 to 8
    {
        int i = 1, i2 = 8;
        D2D1_POINT_2F pJointPoints = { tOpenPoseData.m2DLocation[i].x, tOpenPoseData.m2DLocation[i].y };
        D2D1_POINT_2F pJointPoints2 = { tOpenPoseData.m2DLocation[i2].x, tOpenPoseData.m2DLocation[i2].y };
        if ((pJointPoints.x != 0 && pJointPoints.y != 0) && (pJointPoints2.x != 0 && pJointPoints2.y != 0))
        {
            pJointPoints = BodyToScreen(obj, pJointPoints, tSize.x, tSize.y);
            pJointPoints2 = BodyToScreen(obj, pJointPoints2, tSize.x, tSize.y);
            tRenderTargets->pRT2D->DrawLine(pJointPoints, pJointPoints2, tRenderTargets->pD2DHelper->GetBrush(ColorF::Red), 5.f);
        }
    }

    // 8 to 9
    {
        int i = 8, i2 = 9;
        D2D1_POINT_2F pJointPoints = { tOpenPoseData.m2DLocation[i].x, tOpenPoseData.m2DLocation[i].y };
        D2D1_POINT_2F pJointPoints2 = { tOpenPoseData.m2DLocation[i2].x, tOpenPoseData.m2DLocation[i2].y };
        if ((pJointPoints.x != 0 && pJointPoints.y != 0) && (pJointPoints2.x != 0 && pJointPoints2.y != 0))
        {
            pJointPoints = BodyToScreen(obj, pJointPoints, tSize.x, tSize.y);
            pJointPoints2 = BodyToScreen(obj, pJointPoints2, tSize.x, tSize.y);
            tRenderTargets->pRT2D->DrawLine(pJointPoints, pJointPoints2, tRenderTargets->pD2DHelper->GetBrush(ColorF::Green), 5.f);
        }
    }

    for (int i = 9; i < 11; ++i)
    {
        D2D1_POINT_2F pJointPoints = { tOpenPoseData.m2DLocation[i].x, tOpenPoseData.m2DLocation[i].y };
        D2D1_POINT_2F pJointPoints2 = { tOpenPoseData.m2DLocation[i+1].x, tOpenPoseData.m2DLocation[i+1].y };
        if ((pJointPoints.x != 0 && pJointPoints.y != 0) && (pJointPoints2.x != 0 && pJointPoints2.y != 0))
        {
            pJointPoints = BodyToScreen(obj, pJointPoints, tSize.x, tSize.y);
            pJointPoints2 = BodyToScreen(obj, pJointPoints2, tSize.x, tSize.y);
            tRenderTargets->pRT2D->DrawLine(pJointPoints, pJointPoints2, tRenderTargets->pD2DHelper->GetBrush(ColorF::Cyan), 5.f);
        }
    }

    // 8 to 12
    {
        int i = 8, i2 = 12;
        D2D1_POINT_2F pJointPoints = { tOpenPoseData.m2DLocation[i].x, tOpenPoseData.m2DLocation[i].y };
        D2D1_POINT_2F pJointPoints2 = { tOpenPoseData.m2DLocation[i2].x, tOpenPoseData.m2DLocation[i2].y };
        if ((pJointPoints.x != 0 && pJointPoints.y != 0) && (pJointPoints2.x != 0 && pJointPoints2.y != 0))
        {
            pJointPoints = BodyToScreen(obj, pJointPoints, tSize.x, tSize.y);
            pJointPoints2 = BodyToScreen(obj, pJointPoints2, tSize.x, tSize.y);
            tRenderTargets->pRT2D->DrawLine(pJointPoints, pJointPoints2, tRenderTargets->pD2DHelper->GetBrush(ColorF::SkyBlue), 5.f);
        }
    }

    for (int i = 12; i < 14; ++i)
    {
        D2D1_POINT_2F pJointPoints = { tOpenPoseData.m2DLocation[i].x, tOpenPoseData.m2DLocation[i].y };
        D2D1_POINT_2F pJointPoints2 = { tOpenPoseData.m2DLocation[i+1].x, tOpenPoseData.m2DLocation[i+1].y };
        if ((pJointPoints.x != 0 && pJointPoints.y != 0) && (pJointPoints2.x != 0 && pJointPoints2.y != 0))
        {
            pJointPoints = BodyToScreen(obj, pJointPoints, tSize.x, tSize.y);
            pJointPoints2 = BodyToScreen(obj, pJointPoints2, tSize.x, tSize.y);
            tRenderTargets->pRT2D->DrawLine(pJointPoints, pJointPoints2, tRenderTargets->pD2DHelper->GetBrush(ColorF::Blue), 5.f);
        }
    }

    //if(pForm->m_pCurrentFormDocker->m_pFocusForm != pForm) return;


    /*int index = (int)GET_SINGLE(KinectDataManager)->GetFrameManager()->GetIndex();
    auto kinectData = GET_SINGLE(KinectDataManager)->GetKinectData();
    if (KEYPRESS(HASH_VK_LEFT))
    {
        GET_SINGLE(KinectDataManager)->GetFrameManager()->SetIndex(index - 1);
    }
    if (KEYPRESS(HASH_VK_RIGHT))
    {
        GET_SINGLE(KinectDataManager)->GetFrameManager()->SetIndex(index + 1);
    }

    ID2D1SolidColorBrush * brush = tRenderTargets->pD2DHelper->GetBrush(ColorF::Enum(0x1E1E1E));
    tRenderTargets->pRT2D->FillRectangle(
        D2D1_RECT_F{ tPos.x, tPos.y, tPos.x + tSize.x, tPos.y + tSize.y }, brush);

    if (pForm->m_pCurrentFormDocker->m_pFocusForm == pForm)
    {
        m_pKinect2DRenderer->Update(GET_SINGLE(KinectDataManager)->GetFrameManager(), kinectData, obj, tRenderTargets, fDeltaTime);
    }
    m_pKinect2DRenderer->Render(obj, D2D1::ColorF::Green, tRenderTargets, fDeltaTime, kinectData->at(index));*/
}
