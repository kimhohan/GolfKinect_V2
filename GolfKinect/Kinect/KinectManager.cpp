#include "KinectManager.h"

//#include "GraphicsManager.h"
#include "../Core/Direct2DHelper.h"
#include "KinectBody.h"
#include "KinectColor.h"

DEFINITION_SINGLE(KinectManager)

KinectManager::KinectManager()
{
}

KinectManager::~KinectManager()
{
}

void KinectManager::Initialize()
{
	InitializeDefaultSensor();

	pKinectBody = new KinectBody();
	pKinectBody->Initialize(mpKinectSensor);
	pKinectColor = new KinectColor();
	pKinectColor->Initialize(mpKinectSensor);
}

HRESULT KinectManager::InitializeDefaultSensor()
{
	HRESULT hr;

	hr = GetDefaultKinectSensor(&mpKinectSensor);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = mpKinectSensor->Open();

	return hr;
}

void KinectManager::Update()
{
	if (mCurrentStreamingType == EKinectStreamingType::Color || mCurrentStreamingType == EKinectStreamingType::Both)
	{
		pKinectColor->Update();
	}
	if (mCurrentStreamingType == EKinectStreamingType::Body || mCurrentStreamingType == EKinectStreamingType::Both)
	{
		pKinectBody->Update();
	}
}

void KinectManager::Render(ID2D1HwndRenderTarget* pD2D1HwndRenderTarget, ID2D1SolidColorBrush * trackedColor, ID2D1SolidColorBrush * inferredColor)
{
    pD2D1HwndRenderTarget->BeginDraw();
	if (mCurrentStreamingType == EKinectStreamingType::Color || mCurrentStreamingType == EKinectStreamingType::Both)
	{
		pKinectColor->Render(pD2D1HwndRenderTarget);
	}
	if (mCurrentStreamingType == EKinectStreamingType::Body || mCurrentStreamingType == EKinectStreamingType::Both)
	{
		if (mCurrentStreamingType == EKinectStreamingType::Body && pKinectBody->GetTracked() == true)
		{
            pD2D1HwndRenderTarget->Clear();
		}
		pKinectBody->Render(pD2D1HwndRenderTarget, trackedColor, inferredColor);
	}
    pD2D1HwndRenderTarget->EndDraw();
}

void KinectManager::Release()
{
	pKinectBody->Release();
	pKinectColor->Release();
	SAFE_DELETE(pKinectBody);
	SAFE_DELETE(pKinectColor);
}

void KinectManager::SetKinectStreamingType(EKinectStreamingType type)
{
	if (mCurrentStreamingType != type)
	{
		//pKinectColor->DiscardDirect2DResources();
		//pKinectBody->DiscardDirect2DResources();
		mCurrentStreamingType = type;
	}
}

KinectBody * KinectManager::GetKinectBody()
{
	assert(pKinectBody);
	return pKinectBody;
}

KinectColor * KinectManager::GetKinectColor()
{
	assert(pKinectColor);
	return pKinectColor;
}