#pragma once
#include "../Game.h"

class OpenPoseGraphRenderer
{
private:
	class Object * m_pObject = nullptr;
	vector<GraphRenderTargetData> mGraphComponent;
	UINT32 mSelectedGraphIndex = 0;

public:
	OpenPoseGraphRenderer();
	~OpenPoseGraphRenderer();

public:
	void Init();
	void Update(class Object * obj, float fDeltaTime);
	void Render(class Object * obj, RenderTargets * tRenderTargets, float fDeltaTime);

public:
	void OnGraphDataChanged();
	void OnResize();
	void OnDataFileLoaded();
	//void OnComparativeDataFileLoaded();

private:
	void InputOpenPoseData(UINT32 graphIndex, UINT32 isCompar, UINT32 jointIndex);
};

