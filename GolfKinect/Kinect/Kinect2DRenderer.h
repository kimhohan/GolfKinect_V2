#pragma once
#include "../Game.h"
#include "FrameManager.h"

class Kinect2DRenderer
{
    friend class Core;

private:
    class Kinect2DGraphRenderer * m_pKinect2DGraphRenderer = nullptr;
    size_t mJointIndex = 0;
    size_t mIsCompar = 1;

public:
	Kinect2DRenderer();
	~Kinect2DRenderer();

	void Init(Kinect2DGraphRenderer * pKinect2DGraphRenderer);
	void Update(FrameManager * frameManager, const vector<KINECTDATA*>* kinectData, class Object * obj, RenderTargets * tRenderTargets, float fDeltaTime);
	void Render(class Object * obj, ColorF::Enum color, RenderTargets * tRenderTargets, float fDeltaTime, KINECTDATA * kinectData = nullptr);

    virtual LRESULT UIProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

