//#pragma once
//#include "BasicUIElement.h"
//#define MAX_COUNT_Y 2
//#define MAX_COUNT_X 4
//class GraphComponent;
//struct GraphRenderTargetData
//{
//	vector<GraphComponent *> mComponent;
//	UINT32 mOffX = 0;
//	UINT32 mOffY = 0;
//	UINT32 mOffOriginalComparative[2] = { 0, 0 };//�����̳� ������ �׷����� �׸��� ���ϴ� ����(0�̸� ���̰�, 1�̸� �Ⱥ��̰�)
//};
//
//class KinectGraphRenderTarget :
//	public BasicUIElement
//{
//private:
//	vector<GraphRenderTargetData> mGraphComponent;
//    UINT32 mSelectedGraphIndex = 0;
//
//public:
//	KinectGraphRenderTarget();
//	virtual ~KinectGraphRenderTarget();
//
//	virtual void Initialize(HWND hWnd, int x, int y);
//	virtual void Update();
//	virtual void Render();
//	virtual void Release();
//
//	//void GraphInserteDelete(UINT insertErase, unsigned short graphNumber);
//	void GraphInserteDelete(UINT insertErase);
//
//	virtual LRESULT UIProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//
//private:
//	void InputKinectData(UINT32 graphIndex, UINT32 isCompar, UINT32 jointIndex);
//
//	virtual void OnGraphDataChanged();
//	virtual void OnResize();
//	virtual void OnDataFileLoaded();
//	virtual void OnComparativeDataFileLoaded();
//};