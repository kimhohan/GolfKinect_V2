#pragma once
#include "Scene.h"
class Project4KPScene :
	public Scene
{
private:
	friend class SceneManager;
	friend class Core;

private:
    static class Kinect2DRenderer * m_pKinect2DRenderer;
	class Kinect2DGraphRenderer * m_pKinect2DGraphRenderer = nullptr;
	class OpenPoseGraphRenderer * m_pOpenPoseGraphRenderer = nullptr;
	class Kinect3DRenderer * m_pKinect3DRenderer = nullptr;
	class Kinect3DRenderer * m_pKinect3DCmpRenderer = nullptr;

public:
	Project4KPScene();
	~Project4KPScene();

public:
	virtual bool Init();
	virtual void Input(float fDeltaTime);

private:
	class DirectX11Helper * m_pDirectX11Helper = nullptr;
	PD3DHelper m_ptD3DHelper = nullptr;

	// 버튼
	void Button(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime);
    // 원본군
	void RenderOrigin2D(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime);
	// Test 프로그램
	void TestProgram(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime);
    // 사각형
	void RenderRectangle(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime);
    // 대조군
	void RenderCmp2D(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime);
    // 그래프
	void RenderGraph(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime);
	void RenderOpenPoseGraph(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime);
    // 3D
    void Render3D(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime);
    void Render3DCmp(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime);
    // Time Line
    void RenderTimeLine(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime);
    void RenderEndTimeLine(Object * obj, float fDeltaTime);
    void TimeLineCallbackDown(class UIButton * pBtn, float fDeltaTime);
    // OpenPose 원본군
    void RenderOpenPose2D(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime);
};

