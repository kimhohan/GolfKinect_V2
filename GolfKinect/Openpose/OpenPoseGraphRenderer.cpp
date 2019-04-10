#include "OpenPoseGraphRenderer.h"
#include "OpenPoseDataManager.h"
#include "../Kinect/GraphComponent.h"

OpenPoseGraphRenderer::OpenPoseGraphRenderer()
{
}


OpenPoseGraphRenderer::~OpenPoseGraphRenderer()
{
}

void OpenPoseGraphRenderer::Init()
{
	UINT32 i = 0;
	mGraphComponent.resize(MAX_GRAPH_COUNT_Y);
	for (; i < MAX_GRAPH_COUNT_Y; ++i)//그래프 갯수를 MAX_COUNT_Y를 변경
	{
		for (UINT32 j = 0; j < MAX_GRAPH_COUNT_X; ++j)
		{
			GraphComponent * graphComponent = new GraphComponent();
			mGraphComponent[i].mComponent.push_back(graphComponent);
			mGraphComponent[i].mComponent[j]->Initialize(MAX_GRAPH_COUNT_Y, MAX_GRAPH_COUNT_X, i, j);
		}
	}
	GET_SINGLE(OpenPoseDataManager)->InitGraphData(i);
}

void OpenPoseGraphRenderer::Update(Object * obj, float fDeltaTime)
{
	assert(obj);
	m_pObject = obj;

	for (UINT32 i = 0; i < mGraphComponent.size(); ++i)
	{
		for (UINT32 j = 0; j < mGraphComponent[i].mComponent.size(); ++j)
		{
			mGraphComponent[i].mComponent[j]->Update(obj, fDeltaTime);
		}
	}
}

void OpenPoseGraphRenderer::Render(Object * obj, RenderTargets * tRenderTargets, float fDeltaTime)
{
	assert(obj);
	m_pObject = obj;

	for (UINT32 i = 0; i < mGraphComponent.size(); ++i)
	{
		for (UINT32 j = 0; j < mGraphComponent[i].mComponent.size(); ++j)
		{
			mGraphComponent[i].mComponent[j]->Render(obj, tRenderTargets, fDeltaTime);
		}
	}
}

void OpenPoseGraphRenderer::OnGraphDataChanged()
{
	auto dataManager = GET_SINGLE(OpenPoseDataManager);
	if (mGraphComponent.size() < dataManager->GetGraphData().size())
	{//새로그릴경우 제일끝에 추가
		auto selectedGraphIndex = mGraphComponent.size();
		//frameManager->InsertGraphData(selectedGraphIndex);
		GraphRenderTargetData temp;
		mGraphComponent.insert(mGraphComponent.begin() + selectedGraphIndex, temp);
		for (UINT32 j = 0; j < MAX_GRAPH_COUNT_X; ++j)
		{
			GraphComponent * graphComponent = new GraphComponent();
			mGraphComponent[selectedGraphIndex].mComponent.push_back(graphComponent);
			mGraphComponent[selectedGraphIndex].mComponent[j]->Initialize(MAX_GRAPH_COUNT_Y, MAX_GRAPH_COUNT_X, selectedGraphIndex, j);
		}
		/*for (UINT32 i = 0; i < mGraphComponent.size(); ++i)
		{
			for (UINT32 j = 0; j < MAX_GRAPH_COUNT_X; ++j)
				mGraphComponent[i].mComponent[j]->DiscardDirect2DResources();
		}
		for (UINT32 i = 0; i < mGraphComponent.size(); ++i)
		{
			for (UINT32 j = 0; j < MAX_GRAPH_COUNT_X; ++j)
				mGraphComponent[i].mComponent[j]->EnsureDirect2DResources(mGraphComponent.size(), mGraphComponent[i].mComponent.size(), i, j);
		}*/
		//GET_SINGLE(Core)->OnResize(GET_WINDOWHANDLE);
		OnResize();
	}
	auto graphData = dataManager->GetGraphData();
	for (UINT32 i = 0; i < graphData.size(); ++i)
	{
		InputOpenPoseData(i, graphData.at(i).IsComparative, graphData.at(i).JointNumber);
	}
}

void OpenPoseGraphRenderer::OnResize()
{
	for (UINT32 i = 0; i < mGraphComponent.size(); ++i)
	{
		for (UINT32 j = 0; j < MAX_GRAPH_COUNT_X; ++j)
		{
			mGraphComponent[i].mComponent[j]->OnResize(mGraphComponent.size(), MAX_GRAPH_COUNT_X, i, j);
		}
	}
}

void OpenPoseGraphRenderer::OnDataFileLoaded()
{
	for (UINT32 i = 0; i < mGraphComponent.size(); ++i)
	{
		auto dataManager = GET_SINGLE(OpenPoseDataManager);
		auto graphData = dataManager->GetGraphData();
		if (graphData.at(i).IsComparative == 2)
		{
			dataManager->SetGraphData(i, (UINT32)0, i);
			InputOpenPoseData(i, (UINT32)0, i);
		}
	}
	OnGraphDataChanged();
	//GET_SINGLE(OpenPoseDataManager)->GetFrameManager()->SetIndexUpdated(true);
}
//
//void OpenPoseGraphRenderer::OnComparativeDataFileLoaded()
//{
//}

void OpenPoseGraphRenderer::InputOpenPoseData(UINT32 graphIndex, UINT32 isCompar, UINT32 jointIndex)
{ //GraphComponent::InputData를 쓰기위해 형식을 변환해주는 함수
	//assert(isCompar > 1);

	vector<float> graphDot;
	const vector<OpenPoseData>* p_vecOpenPoseFrameData;
	const UINT32 * stapData;

	p_vecOpenPoseFrameData = GET_SINGLE(OpenPoseDataManager)->GetData();
	stapData = GET_SINGLE(OpenPoseDataManager)->GetOpenPoseSteps();
	//if (isCompar)
	//{
	//	p_vecOpenPoseFrameData = GET_SINGLE(OpenPoseDataManager)->GetData();
	//	stapData = GET_SINGLE(OpenPoseDataManager)->GetOpenPoseSteps();
	//}
	//else
	//{
	//	graphKinectData = GET_SINGLE(KinectDataManager)->GetKinectData();
	//	stapData = GET_SINGLE(KinectDataManager)->GetKinectSteps();
	//}
	wstring str = L"";
	if (p_vecOpenPoseFrameData->size() == 0)
		return;
	//auto GraphKinectData
	UINT32 switch_on;
	UINT32 numOfFrame = 0;
	float dataMax = (float)0;
	float dataMin = (float)0;
	dataMin = dataMax = 0;
	const vector<OpenPoseData> * kinectData;
	if (mGraphComponent.at(graphIndex).mOffX == 0)
	{//x가 켜져있으면
		dataMin = dataMax = p_vecOpenPoseFrameData->at(0).m2DLocation[jointIndex].x;
		kinectData = GET_SINGLE(OpenPoseDataManager)->GetData();
		if ((mGraphComponent.at(graphIndex).mOffOriginalComparative[0] == 0) && (kinectData->size() != 0))
		{
			for (int i = 0; i < kinectData->size(); ++i)
			{
				if (dataMax < kinectData->at(i).m2DLocation[jointIndex].x)
				{
					dataMax = kinectData->at(i).m2DLocation[jointIndex].x;
				}
				if (dataMin > kinectData->at(i).m2DLocation[jointIndex].x)
					dataMin = kinectData->at(i).m2DLocation[jointIndex].x;
			}
		}
		/*kinectData = GET_SINGLE(OpenPoseDataManager)->GetKinectComparativeData();
		if ((mGraphComponent.at(graphIndex).mOffOriginalComparative[1] == 0) && (kinectData->size() != 0))
		{
			for (int i = 0; i < kinectData->size(); ++i)
			{
				if (dataMax < kinectData->at(i)->m2DLocation[jointIndex].x)
				{
					dataMax = kinectData->at(i)->m2DLocation[jointIndex].x;
				}
				if (dataMin > kinectData->at(i)->m2DLocation[jointIndex].x)
					dataMin = kinectData->at(i)->m2DLocation[jointIndex].x;
			}
		}*/
	}
	if (mGraphComponent.at(graphIndex).mOffY == 0)
	{
		if (dataMin == dataMax)
			dataMin = dataMax = p_vecOpenPoseFrameData->at(0).m2DLocation[jointIndex].y;
		kinectData = GET_SINGLE(OpenPoseDataManager)->GetData();
		if ((mGraphComponent.at(graphIndex).mOffOriginalComparative[0] == 0) && (kinectData->size() != 0))
		{
			for (int i = 0; i < kinectData->size(); ++i)
			{
				if (dataMax < kinectData->at(i).m2DLocation[jointIndex].y)
				{
					dataMax = kinectData->at(i).m2DLocation[jointIndex].y;
				}
				if (dataMin > kinectData->at(i).m2DLocation[jointIndex].y)
					dataMin = kinectData->at(i).m2DLocation[jointIndex].y;
			}
		}
		/*kinectData = GET_SINGLE(KinectDataManager)->GetKinectComparativeData();
		if ((mGraphComponent.at(graphIndex).mOffOriginalComparative[1] == 0) && (kinectData->size() != 0))
		{
			for (int i = 0; i < kinectData->size(); ++i)
			{
				if (dataMax < kinectData->at(i)->m2DLocation[jointIndex].y)
				{
					dataMax = kinectData->at(i)->m2DLocation[jointIndex].y;
				}
				if (dataMin > kinectData->at(i)->m2DLocation[jointIndex].y)
					dataMin = kinectData->at(i)->m2DLocation[jointIndex].y;
			}
		}*/
	}
	for (UINT32 l = 0; l < mGraphComponent[graphIndex].mComponent.size(); ++l)
	{
		graphDot.resize(0);
		if ((mGraphComponent.at(graphIndex).mOffX == 0) && (mGraphComponent.at(graphIndex).mOffOriginalComparative[isCompar] == 0))
		{
			for (UINT32 i = stapData[l + 1]; i < stapData[l + 2]; i++)
			{
				graphDot.push_back(p_vecOpenPoseFrameData->at(i).m2DLocation[jointIndex].x);
			}
			//입력되는 스트링은 여기서 변경
			str = cJointTypeToString[jointIndex] + L"'s x";
			if (isCompar)
			{
				str = L"Comparative " + str;

			}
			if (l != 0)
				str = L"";
			mGraphComponent.at(graphIndex).mComponent[l]->InputData(&graphDot, cKinectStepType[l + 1], str, (UINT)isCompar * 2, (UINT32)1, dataMax, dataMin);
		}
		else
		{
			str = L"";
			mGraphComponent.at(graphIndex).mComponent[l]->InputData(&graphDot, cKinectStepType[l + 1], str, (UINT)isCompar * 2, (UINT32)0, dataMax, dataMin);
		}
	}



	for (UINT32 l = 0; l < mGraphComponent[graphIndex].mComponent.size(); ++l)
	{
		graphDot.resize(0);
		if ((mGraphComponent.at(graphIndex).mOffY == 0) && (mGraphComponent.at(graphIndex).mOffOriginalComparative[isCompar] == 0))
		{
			for (UINT32 i = stapData[l + 1]; i < stapData[l + 2]; i++)
			{
				graphDot.push_back(p_vecOpenPoseFrameData->at(i).m2DLocation[jointIndex].y);
			}
			//입력되는 스트링은 여기서 변경
			str = cJointTypeToString[jointIndex] + L"'s y";
			if (isCompar)
			{
				str = L"Comparative " + str;
			}
			if (l != 0)
				str = L"";
			mGraphComponent.at(graphIndex).mComponent[l]->InputData(&graphDot, cKinectStepType[l + 1], str, (UINT)isCompar * 2 + 1, mGraphComponent.at(graphIndex).mOffX, dataMax, dataMin);
		}
		else
		{
			str = L"";
			mGraphComponent.at(graphIndex).mComponent[l]->InputData(&graphDot, cKinectStepType[l + 1], str, (UINT)isCompar * 2 + 1, (UINT32)0, dataMax, dataMin);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	p_vecOpenPoseFrameData = GET_SINGLE(OpenPoseDataManager)->GetData();
	stapData = GET_SINGLE(OpenPoseDataManager)->GetOpenPoseSteps();
	//isCompar = 0;
	/*if (isCompar)
	{
		p_vecOpenPoseFrameData = GET_SINGLE(OpenPoseDataManager)->GetData();
		stapData = GET_SINGLE(OpenPoseDataManager)->GetOpenPoseSteps();
		isCompar = 0;
	}
	else
	{
		graphKinectData = GET_SINGLE(KinectDataManager)->GetKinectComparativeData();
		stapData = GET_SINGLE(KinectDataManager)->GetKinectComparativeSteps();
		isCompar = 1;
	}*/
	if (p_vecOpenPoseFrameData->size() == 0)
	{
		return;
	}

	for (UINT32 l = 0; l < mGraphComponent[graphIndex].mComponent.size(); ++l)
	{
		graphDot.resize(0);
		if ((mGraphComponent.at(graphIndex).mOffX == 0) && (mGraphComponent.at(graphIndex).mOffOriginalComparative[isCompar] == 0))
		{
			for (UINT32 i = stapData[l + 1]; i < stapData[l + 2]; i++)
			{
				graphDot.push_back(p_vecOpenPoseFrameData->at(i).m2DLocation[jointIndex].x);
			}
			//입력되는 스트링은 여기서 변경
			str = cJointTypeToString[jointIndex] + L"'s x";
			if (isCompar)
			{
				str = L"Comparative " + str;
			}
			if (l != 0)
				str = L"";
			mGraphComponent.at(graphIndex).mComponent[l]->InputData(&graphDot, cKinectStepType[l + 1], str, (UINT)isCompar * 2, (UINT32)0, dataMax, dataMin);
		}
		else
		{
			str = L"";
			mGraphComponent.at(graphIndex).mComponent[l]->InputData(&graphDot, cKinectStepType[l + 1], str, (UINT)isCompar * 2, (UINT32)0, dataMax, dataMin);
		}
	}

	for (UINT32 l = 0; l < mGraphComponent[graphIndex].mComponent.size(); ++l)
	{
		graphDot.resize(0);
		if ((mGraphComponent.at(graphIndex).mOffY == 0) && (mGraphComponent.at(graphIndex).mOffOriginalComparative[isCompar] == 0))
		{
			for (UINT32 i = stapData[l + 1]; i < stapData[l + 2]; i++)
			{
				graphDot.push_back(p_vecOpenPoseFrameData->at(i).m2DLocation[jointIndex].y);
			}
			//입력되는 스트링은 여기서 변경
			str = cJointTypeToString[jointIndex] + L"'s y";
			if (isCompar)
			{
				str = L"Comparative " + str;
			}
			if (l != 0)
				str = L"";
			mGraphComponent.at(graphIndex).mComponent[l]->InputData(&graphDot, cKinectStepType[l + 1], str, (UINT)isCompar * 2 + 1, (UINT32)0, dataMax, dataMin);
		}
		else
		{
			str = L"";
			mGraphComponent.at(graphIndex).mComponent[l]->InputData(&graphDot, cKinectStepType[l + 1], str, (UINT)isCompar * 2 + 1, (UINT32)0, dataMax, dataMin);
		}
	}

	str = L"";
}
