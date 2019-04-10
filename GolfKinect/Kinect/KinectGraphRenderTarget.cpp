//#include "KinectGraphRenderTarget.h"
//#include "BasicView.h"
//#include "GraphicsManager.h"
//#include "Direct2DHelper.h"
//#include "GraphComponent.h"
//#include "KinectDataManager.h"
//#include "FrameManager.h"
//#include "resource.h"
//
////RECT mCurrentRect;
//
//LRESULT KinectGraphRenderTarget::UIProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	switch (message)
//	{
//	case WM_RBUTTONDOWN:
//	{
//        mSelectedGraphIndex = lParam;
//		HMENU hPopupMenu = CreatePopupMenu();
//
//		HMENU stepAnalyzerPopupMenu1 = CreatePopupMenu();
//		InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING | MF_POPUP, (UINT_PTR)stepAnalyzerPopupMenu1, L"그래프 첨삭");
//        InsertMenu(stepAnalyzerPopupMenu1, 0, MF_BYPOSITION | MF_STRING, ID_GRAPHERASE, L"제거");
//		InsertMenu(stepAnalyzerPopupMenu1, 0, MF_BYPOSITION | MF_STRING, ID_GRAPHINSERT, L"추가");
//		if (GET_SINGLE(KinectDataManager)->GetGraphData().at(mSelectedGraphIndex).IsComparative != 2)
//		{
//			HMENU stepAnalyzerPopupMenu2 = CreatePopupMenu();
//			InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING | MF_POPUP, (UINT_PTR)stepAnalyzerPopupMenu2, L"x, y표시설정");
//			InsertMenu(stepAnalyzerPopupMenu2, 0, MF_BYPOSITION | MF_STRING, ID_WATCHXY, L"x, y 모두보기");
//			InsertMenu(stepAnalyzerPopupMenu2, 0, MF_BYPOSITION | MF_STRING, ID_WATCHY, L"y만 보기");
//			InsertMenu(stepAnalyzerPopupMenu2, 0, MF_BYPOSITION | MF_STRING, ID_WATCHX, L"x만 보기");
//		}
//		if (GET_SINGLE(KinectDataManager)->GetKinectComparativeData()->size() && GET_SINGLE(KinectDataManager)->GetKinectData()->size())
//		{
//			HMENU stepAnalyzerPopupMenu3 = CreatePopupMenu();
//			InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING | MF_POPUP, (UINT_PTR)stepAnalyzerPopupMenu3, L"원본, 대조 표시설정");
//			InsertMenu(stepAnalyzerPopupMenu3, 0, MF_BYPOSITION | MF_STRING, ID_WATCHORIGINALCOMPARATIVE, L"원본, 대조 모두보기");
//			InsertMenu(stepAnalyzerPopupMenu3, 0, MF_BYPOSITION | MF_STRING, ID_WATCHCOMPARATIVE, L"대조만 보기");
//			InsertMenu(stepAnalyzerPopupMenu3, 0, MF_BYPOSITION | MF_STRING, ID_WATCHORIGINAL, L"원본만 보기");
//		}
//
//		WinProcPopupMenu(hPopupMenu, hWnd, message, wParam, lParam);
//
//		POINT point;
//		GetCursorPos(&point);
//		TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, point.x, point.y, 0, hWnd, nullptr);
//		return DefWindowProc(hWnd, message, wParam, lParam);
//		//case WM_GETMINMAXINFO:
//		//{
//		//	MINMAXINFO* mmi = (MINMAXINFO*)lParam;
//		//	mmi->ptMinTrackSize.x = 450;
//		//	mmi->ptMinTrackSize.y = 250;
//		//	break;
//		//}
//	}
//	case WM_COMMAND:
//    {
//		switch (LOWORD(wParam))
//		{
//		case ID_WATCHX:
//			if ((mGraphComponent.at(mSelectedGraphIndex).mOffY == 1) && (mGraphComponent.at(mSelectedGraphIndex).mOffX == 0))
//				break;
//			mGraphComponent.at(mSelectedGraphIndex).mOffY = 1;
//			mGraphComponent.at(mSelectedGraphIndex).mOffX = 0;
//			GetBasicViewInstance()->OnGraphDataChanged();
//			break;
//
//		case ID_WATCHY:
//			if ((mGraphComponent.at(mSelectedGraphIndex).mOffY == 0) && (mGraphComponent.at(mSelectedGraphIndex).mOffX == 1))
//				break;
//			mGraphComponent.at(mSelectedGraphIndex).mOffY = 0;
//			mGraphComponent.at(mSelectedGraphIndex).mOffX = 1;
//			GetBasicViewInstance()->OnGraphDataChanged();
//			break;
//		case ID_WATCHXY:
//			if ((mGraphComponent.at(mSelectedGraphIndex).mOffY == 0) && (mGraphComponent.at(mSelectedGraphIndex).mOffX == 0))
//				break;
//			mGraphComponent.at(mSelectedGraphIndex).mOffY = 0;
//			mGraphComponent.at(mSelectedGraphIndex).mOffX = 0;
//			GetBasicViewInstance()->OnGraphDataChanged();
//			break;
//		case ID_WATCHORIGINAL:
//			if ((mGraphComponent.at(mSelectedGraphIndex).mOffOriginalComparative[0] ==0) && (mGraphComponent.at(mSelectedGraphIndex).mOffOriginalComparative[1] == 1))
//				break;
//			mGraphComponent.at(mSelectedGraphIndex).mOffOriginalComparative[0] = 0;
//			mGraphComponent.at(mSelectedGraphIndex).mOffOriginalComparative[1] = 1;
//			GetBasicViewInstance()->OnGraphDataChanged();
//			break;
//		case ID_WATCHCOMPARATIVE:
//			if ((mGraphComponent.at(mSelectedGraphIndex).mOffOriginalComparative[0] == 1) && (mGraphComponent.at(mSelectedGraphIndex).mOffOriginalComparative[1] == 0))
//				break;
//			mGraphComponent.at(mSelectedGraphIndex).mOffOriginalComparative[0] = 1;
//			mGraphComponent.at(mSelectedGraphIndex).mOffOriginalComparative[1] = 0;
//			GetBasicViewInstance()->OnGraphDataChanged();
//			break;
//		case ID_WATCHORIGINALCOMPARATIVE:
//			if ((mGraphComponent.at(mSelectedGraphIndex).mOffOriginalComparative[0] == 0) && (mGraphComponent.at(mSelectedGraphIndex).mOffOriginalComparative[1] == 0))
//				break;
//			mGraphComponent.at(mSelectedGraphIndex).mOffOriginalComparative[0] = 0;
//			mGraphComponent.at(mSelectedGraphIndex).mOffOriginalComparative[1] = 0;
//			GetBasicViewInstance()->OnGraphDataChanged();
//			break;
//		default:
//			GraphInserteDelete(LOWORD(wParam));
//		}
//			
//	}
//	}
//	return BasicUIElement::UIProc(hWnd, message, wParam, lParam);
//}
//
//KinectGraphRenderTarget::KinectGraphRenderTarget()
//{
//}
//
//KinectGraphRenderTarget::~KinectGraphRenderTarget()
//{
//}
//
//void KinectGraphRenderTarget::Initialize(HWND hWnd, int x, int y)
//{
//	mCurrentRect.right = 450;
//	mCurrentRect.bottom = 250;
//	BasicUIElement::Initialize(hWnd, L"Kinect Graph Render Target", WS_OVERLAPPED | WS_THICKFRAME | WS_CAPTION | WS_CHILD | WS_CLIPSIBLINGS, x, y);
//	UINT32 i = 0;
//	mGraphComponent.resize(MAX_COUNT_Y);
//	for (; i < MAX_COUNT_Y; ++i)//그래프 갯수를 MAX_COUNT_Y를 변경
//	{
//		for (UINT32 j = 0; j < MAX_COUNT_X; ++j)
//		{
//			GraphComponent * graphComponent = new GraphComponent();
//			mGraphComponent[i].mComponent.push_back(graphComponent);
//			mGraphComponent[i].mComponent[j]->Initialize(mhCurrent, MAX_COUNT_Y, MAX_COUNT_X, i, j);
//		}
//	}
//	GET_SINGLE(KinectDataManager)->InitGraphData(i);
//}
//
//void KinectGraphRenderTarget::Update()
//{
//	for (UINT32 i = 0; i < mGraphComponent.size(); ++i)
//	{
//		for (UINT32 j = 0; j < mGraphComponent[i].mComponent.size(); ++j)
//		{
//			mGraphComponent[i].mComponent[j]->Update();
//		}
//	}
//
//	if (!IsProcessable()) return;
//}
//
//void KinectGraphRenderTarget::Render()
//{
//	if (!IsProcessable()) return;
//	for (UINT32 i = 0; i < mGraphComponent.size(); ++i)
//	{
//		for (UINT32 j = 0; j < mGraphComponent[i].mComponent.size(); ++j)
//		{
//			mGraphComponent[i].mComponent[j]->Render();
//		}
//	}
//}
//
//void KinectGraphRenderTarget::Release()
//{
//	for (UINT32 i = 0; i < mGraphComponent.size(); ++i)
//	{
//		for (UINT32 j = 0; j < mGraphComponent[i].mComponent.size(); ++j)
//		{
//			mGraphComponent[i].mComponent[j]->Release();
//			SAFE_DELETE(mGraphComponent[i].mComponent[j]);
//		}
//		mGraphComponent[i].mComponent.clear();
//	}
//	mGraphComponent.clear();
//	BasicUIElement::Release();
//}
//
//void KinectGraphRenderTarget::GraphInserteDelete(UINT insertErase)
//{
//	auto dataManager = GET_SINGLE(KinectDataManager);
//	if (insertErase == ID_GRAPHINSERT)
//	{
//		dataManager->InsertGraphData(mSelectedGraphIndex+1);
//		GraphRenderTargetData temp;
//		mGraphComponent.insert(mGraphComponent.begin() + mSelectedGraphIndex + 1, temp);
//		for (UINT32 j = 0; j < MAX_COUNT_X; ++j)
//		{
//			GraphComponent * graphComponent = new GraphComponent();
//			mGraphComponent[mSelectedGraphIndex + 1].mComponent.push_back(graphComponent);
//			mGraphComponent[mSelectedGraphIndex + 1].mComponent[j]->Initialize(mhCurrent, MAX_COUNT_Y, MAX_COUNT_X, mSelectedGraphIndex + 1, j);
//		}
//
//		for (UINT32 i = 0; i < mGraphComponent.size(); ++i)
//		{
//			for (UINT32 j = 0; j < MAX_COUNT_X; ++j)
//			{
//				mGraphComponent[i].mComponent[j]->DiscardDirect2DResources();
//			}
//			
//		}
//		for (UINT32 i = 0; i < mGraphComponent.size(); ++i)
//		{
//			for (UINT32 j = 0; j < mGraphComponent[i].mComponent.size(); ++j)
//				mGraphComponent[i].mComponent[j]->EnsureDirect2DResources(mGraphComponent.size(), mGraphComponent[i].mComponent.size(), i, j);
//		}
//		OnResize();
//	}
//	else if (insertErase == ID_GRAPHERASE)
//	{
//		if (mGraphComponent.size() > 1)
//		{
//			dataManager->DeleteGraphData(mSelectedGraphIndex);
//			for (UINT32 j = 0; j < MAX_COUNT_X; ++j)
//			{
//				mGraphComponent.at(mSelectedGraphIndex).mComponent[j]->Release();
//			}
//			mGraphComponent.erase(mGraphComponent.begin() + mSelectedGraphIndex);
//
//			for (UINT32 i = 0; i < mGraphComponent.size(); ++i)
//			{
//				for (UINT32 j = 0; j < MAX_COUNT_X; ++j)
//				{
//					mGraphComponent[i].mComponent[j]->DiscardDirect2DResources();
//				}
//			}
//			for (UINT32 i = 0; i < mGraphComponent.size(); ++i)
//			{
//				for (UINT32 j = 0; j < MAX_COUNT_X; ++j)
//					mGraphComponent[i].mComponent[j]->EnsureDirect2DResources(mGraphComponent.size(), MAX_COUNT_X, i, j);
//			}
//			OnResize();
//		}
//	}
//}
//
//void KinectGraphRenderTarget::InputKinectData(UINT32 graphIndex, UINT32 isCompar, UINT32 jointIndex)
//{//GraphComponent::InputData를 쓰기위해 형식을 변환해주는 함수
//	if (isCompar == 2)
//		return;
//	vector<float> graphDot;
//	const vector<KINECTDATA*>* graphKinectData;
//	UINT32 * stapData;
//	if (isCompar)
//	{
//		graphKinectData = GET_SINGLE(KinectDataManager)->GetKinectComparativeData();
//		stapData = GET_SINGLE(KinectDataManager)->GetKinectComparativeSteps();
//	}
//	else
//	{
//		graphKinectData = GET_SINGLE(KinectDataManager)->GetKinectData();
//		stapData = GET_SINGLE(KinectDataManager)->GetKinectSteps();
//	}
//	wstring str = L"";
//	if (graphKinectData->size() == 0)
//		return;
//	//auto GraphKinectData
//	UINT32 switch_on;
//	UINT32 numOfFrame = 0;
//	float dataMax = (float)0;
//	float dataMin = (float)0;
//	dataMin = dataMax = 0;
//	const vector<KINECTDATA*>* kinectData;
//	if (mGraphComponent.at(graphIndex).mOffX == 0)
//	{//x가 켜져있으면
//		dataMin = dataMax = graphKinectData->at(0)->m2DLocation[jointIndex].x;
//		kinectData = GET_SINGLE(KinectDataManager)->GetKinectData();
//		if ((mGraphComponent.at(graphIndex).mOffOriginalComparative[0] == 0) && (kinectData->size() != 0))
//		{
//			for (int i = 0; i < kinectData->size(); ++i)
//			{
//				if (dataMax < kinectData->at(i)->m2DLocation[jointIndex].x)
//				{
//					dataMax = kinectData->at(i)->m2DLocation[jointIndex].x;
//				}
//				if (dataMin > kinectData->at(i)->m2DLocation[jointIndex].x)
//					dataMin = kinectData->at(i)->m2DLocation[jointIndex].x;
//			}
//		}
//		kinectData = GET_SINGLE(KinectDataManager)->GetKinectComparativeData();
//		if ((mGraphComponent.at(graphIndex).mOffOriginalComparative[1] == 0) && (kinectData->size() != 0))
//		{
//			for (int i = 0; i < kinectData->size(); ++i)
//			{
//				if (dataMax < kinectData->at(i)->m2DLocation[jointIndex].x)
//				{
//					dataMax = kinectData->at(i)->m2DLocation[jointIndex].x;
//				}
//				if (dataMin > kinectData->at(i)->m2DLocation[jointIndex].x)
//					dataMin = kinectData->at(i)->m2DLocation[jointIndex].x;
//			}
//		}
//	}
//	if (mGraphComponent.at(graphIndex).mOffY == 0)
//	{
//		if (dataMin == dataMax)
//			dataMin = dataMax = graphKinectData->at(0)->m2DLocation[jointIndex].y;
//		kinectData = GET_SINGLE(KinectDataManager)->GetKinectData();
//		if ((mGraphComponent.at(graphIndex).mOffOriginalComparative[0] == 0) && (kinectData->size() != 0))
//		{
//			for (int i = 0; i < kinectData->size(); ++i)
//			{
//				if (dataMax < kinectData->at(i)->m2DLocation[jointIndex].y)
//				{
//					dataMax = kinectData->at(i)->m2DLocation[jointIndex].y;
//				}
//				if (dataMin > kinectData->at(i)->m2DLocation[jointIndex].y)
//					dataMin = kinectData->at(i)->m2DLocation[jointIndex].y;
//			}
//		}
//		kinectData = GET_SINGLE(KinectDataManager)->GetKinectComparativeData();
//		if ((mGraphComponent.at(graphIndex).mOffOriginalComparative[1] == 0) && (kinectData->size() != 0))
//		{
//			for (int i = 0; i < kinectData->size(); ++i)
//			{
//				if (dataMax < kinectData->at(i)->m2DLocation[jointIndex].y)
//				{
//					dataMax = kinectData->at(i)->m2DLocation[jointIndex].y;
//				}
//				if (dataMin > kinectData->at(i)->m2DLocation[jointIndex].y)
//					dataMin = kinectData->at(i)->m2DLocation[jointIndex].y;
//			}
//		}
//	}
//	for (UINT32 l = 0; l < mGraphComponent[graphIndex].mComponent.size(); ++l)
//	{
//		graphDot.resize(0);
//		if ((mGraphComponent.at(graphIndex).mOffX == 0) && (mGraphComponent.at(graphIndex).mOffOriginalComparative[isCompar] == 0))
//		{
//			for (UINT32 i = stapData[l + 1]; i < stapData[l + 2]; i++)
//			{
//				graphDot.push_back(graphKinectData->at(i)->m2DLocation[jointIndex].x);
//			}
//			//입력되는 스트링은 여기서 변경
//			str = cJointTypeToString[jointIndex] + L"'s x";
//			if (isCompar)
//			{
//				str = L"Comparative " + str;
//
//			}
//			if (l != 0)
//				str = L"";
//			mGraphComponent.at(graphIndex).mComponent[l]->InputData(&graphDot, cKinectStepType[l + 1], str, (UINT)isCompar * 2, (UINT32)1, dataMax, dataMin);
//		}
//		else
//		{
//			str = L"";
//			mGraphComponent.at(graphIndex).mComponent[l]->InputData(&graphDot, cKinectStepType[l + 1], str, (UINT)isCompar * 2, (UINT32)0, dataMax, dataMin);
//		}
//	}
//
//
//
//	for (UINT32 l = 0; l < mGraphComponent[graphIndex].mComponent.size(); ++l)
//	{
//		graphDot.resize(0);
//		if ((mGraphComponent.at(graphIndex).mOffY == 0) && (mGraphComponent.at(graphIndex).mOffOriginalComparative[isCompar] == 0))
//		{
//			for (UINT32 i = stapData[l + 1]; i < stapData[l + 2]; i++)
//			{
//				graphDot.push_back(graphKinectData->at(i)->m2DLocation[jointIndex].y);
//			}
//			//입력되는 스트링은 여기서 변경
//			str = cJointTypeToString[jointIndex] + L"'s y";
//			if (isCompar)
//			{
//				str = L"Comparative " + str;
//			}
//			if (l != 0)
//				str = L"";
//			mGraphComponent.at(graphIndex).mComponent[l]->InputData(&graphDot, cKinectStepType[l + 1], str, (UINT)isCompar * 2 + 1, mGraphComponent.at(graphIndex).mOffX, dataMax, dataMin);
//		}
//		else
//		{
//			str = L"";
//			mGraphComponent.at(graphIndex).mComponent[l]->InputData(&graphDot, cKinectStepType[l + 1], str, (UINT)isCompar * 2 + 1, (UINT32)0, dataMax, dataMin);
//		}
//	}
//
//	///////////////////////////////////////////////////////////////////////////////////////////
//	if (isCompar)
//	{
//		graphKinectData = GET_SINGLE(KinectDataManager)->GetKinectData();
//		stapData = GET_SINGLE(KinectDataManager)->GetKinectSteps();
//		isCompar = 0;
//	}
//	else
//	{
//		graphKinectData = GET_SINGLE(KinectDataManager)->GetKinectComparativeData();
//		stapData = GET_SINGLE(KinectDataManager)->GetKinectComparativeSteps();
//		isCompar = 1;
//	}
//	if (graphKinectData->size() == 0)
//	{
//		return;
//	}
//
//	for (UINT32 l = 0; l < mGraphComponent[graphIndex].mComponent.size(); ++l)
//	{
//		graphDot.resize(0);
//		if ((mGraphComponent.at(graphIndex).mOffX == 0) && (mGraphComponent.at(graphIndex).mOffOriginalComparative[isCompar] == 0))
//		{
//			for (UINT32 i = stapData[l + 1]; i < stapData[l + 2]; i++)
//			{
//				graphDot.push_back(graphKinectData->at(i)->m2DLocation[jointIndex].x);
//			}
//			//입력되는 스트링은 여기서 변경
//			str = cJointTypeToString[jointIndex] + L"'s x";
//			if (isCompar)
//			{
//				str = L"Comparative " + str;
//			}
//			if (l != 0)
//				str = L"";
//			mGraphComponent.at(graphIndex).mComponent[l]->InputData(&graphDot, cKinectStepType[l + 1], str, (UINT)isCompar * 2, (UINT32)0, dataMax, dataMin);
//		}
//		else
//		{
//			str = L"";
//			mGraphComponent.at(graphIndex).mComponent[l]->InputData(&graphDot, cKinectStepType[l + 1], str, (UINT)isCompar * 2, (UINT32)0, dataMax, dataMin);
//		}
//	}
//
//	for (UINT32 l = 0; l < mGraphComponent[graphIndex].mComponent.size(); ++l)
//	{
//		graphDot.resize(0);
//		if ((mGraphComponent.at(graphIndex).mOffY == 0) && (mGraphComponent.at(graphIndex).mOffOriginalComparative[isCompar] == 0))
//		{
//			for (UINT32 i = stapData[l + 1]; i < stapData[l + 2]; i++)
//			{
//				graphDot.push_back(graphKinectData->at(i)->m2DLocation[jointIndex].y);
//			}
//			//입력되는 스트링은 여기서 변경
//			str = cJointTypeToString[jointIndex] + L"'s y";
//			if (isCompar)
//			{
//				str = L"Comparative " + str;
//			}
//			if (l != 0)
//				str = L"";
//			mGraphComponent.at(graphIndex).mComponent[l]->InputData(&graphDot, cKinectStepType[l + 1], str, (UINT)isCompar * 2 + 1, (UINT32)0, dataMax, dataMin);
//		}
//		else
//		{
//			str = L"";
//			mGraphComponent.at(graphIndex).mComponent[l]->InputData(&graphDot, cKinectStepType[l + 1], str, (UINT)isCompar * 2 + 1, (UINT32)0, dataMax, dataMin);
//		}
//	}
//
//}
//
//void KinectGraphRenderTarget::OnGraphDataChanged()
//{
//	auto dataManager = GET_SINGLE(KinectDataManager);
//	if (mGraphComponent.size() < dataManager->GetGraphData().size())
//	{//새로그릴경우 제일끝에 추가
//		auto selectedGraphIndex = mGraphComponent.size();
//		//frameManager->InsertGraphData(selectedGraphIndex);
//		GraphRenderTargetData temp;
//		mGraphComponent.insert(mGraphComponent.begin() + selectedGraphIndex, temp);
//		for (UINT32 j = 0; j < MAX_COUNT_X; ++j)
//		{
//			GraphComponent * graphComponent = new GraphComponent();
//			mGraphComponent[selectedGraphIndex].mComponent.push_back(graphComponent);
//			mGraphComponent[selectedGraphIndex].mComponent[j]->Initialize(mhCurrent, MAX_COUNT_Y, MAX_COUNT_X, selectedGraphIndex, j);
//		}
//		for (UINT32 i = 0; i < mGraphComponent.size(); ++i)
//		{
//			for (UINT32 j = 0; j < MAX_COUNT_X; ++j)
//				mGraphComponent[i].mComponent[j]->DiscardDirect2DResources();
//		}
//		for (UINT32 i = 0; i < mGraphComponent.size(); ++i)
//		{
//			for (UINT32 j = 0; j < MAX_COUNT_X; ++j)
//				mGraphComponent[i].mComponent[j]->EnsureDirect2DResources(mGraphComponent.size(), mGraphComponent[i].mComponent.size(), i, j);
//		}
//		OnResize();
//	}
//	auto graphData = dataManager->GetGraphData();
//	for (UINT32 i = 0; i < graphData.size(); ++i)
//	{
//		InputKinectData(i, graphData.at(i).IsComparative, graphData.at(i).JointNumber);
//	}
//}
//
//void KinectGraphRenderTarget::OnResize()
//{
//	for (UINT32 i = 0; i < mGraphComponent.size(); ++i)
//	{
//		for (UINT32 j = 0; j < mGraphComponent[i].mComponent.size(); ++j)
//		{
//			mGraphComponent[i].mComponent[j]->OnResize(mhCurrent, mGraphComponent.size(), mGraphComponent[i].mComponent.size(), i, j);
//		}
//	}
//
//	GET_SINGLE(KinectDataManager)->GetFrameManager()->SetIndexUpdated(true);
//}
//void KinectGraphRenderTarget::OnDataFileLoaded()
//{
//	for (UINT32 i = 0; i < mGraphComponent.size(); ++i)
//	{
//		auto dataManager = GET_SINGLE(KinectDataManager);
//		auto graphData = dataManager->GetGraphData();
//		if (graphData.at(i).IsComparative == 2)
//		{
//			dataManager->SetGraphData(i, (UINT32)0, i);
//			InputKinectData(i, (UINT32)0, i);
//		}
//	}
//	OnGraphDataChanged();
//	GET_SINGLE(KinectDataManager)->GetFrameManager()->SetIndexUpdated(true);
//}
//void KinectGraphRenderTarget::OnComparativeDataFileLoaded()
//{
//	for (UINT32 i = 0; i < mGraphComponent.size(); ++i)
//	{
//		auto dataManager = GET_SINGLE(KinectDataManager);
//		auto graphData = dataManager->GetGraphData();
//		if (graphData.at(i).IsComparative == 2)
//		{
//			dataManager->SetGraphData(i, (UINT32)1, i);
//			InputKinectData(i, (UINT32)1, i);
//		}
//	}
//	OnGraphDataChanged();
//	GET_SINGLE(KinectDataManager)->GetFrameManager()->SetIndexUpdated(true);
//}