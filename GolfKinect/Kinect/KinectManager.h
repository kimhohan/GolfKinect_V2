#pragma once
#include "../Game.h"

//class PerformanceTimer;
class KinectBody;
class KinectColor;

class KinectManager
{
private:
	// Current Kinect
	IKinectSensor * mpKinectSensor = nullptr;
	KinectBody * pKinectBody = nullptr;
	KinectColor * pKinectColor = nullptr;
	EKinectStreamingType mCurrentStreamingType = EKinectStreamingType::Nothing;

public:
	void Initialize();
	void Update();
	void Render(ID2D1HwndRenderTarget* pD2D1HwndRenderTarget, ID2D1SolidColorBrush * trackedColor, ID2D1SolidColorBrush * inferredColor);
	void Release();

	void SetKinectStreamingType(EKinectStreamingType type);

	KinectBody *  GetKinectBody();
	KinectColor * GetKinectColor();

	DECLARE_SINGLE(KinectManager)

private:
	HRESULT InitializeDefaultSensor();
};

