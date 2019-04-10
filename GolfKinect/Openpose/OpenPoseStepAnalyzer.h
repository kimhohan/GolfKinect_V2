#pragma once
#include "OpenPoseDataManager.h"

class OpenPoseStepAnalyzer
{
private:
	vector<OpenPoseData> * vecOpenPoseFrameData;
	UINT32 * m_pOpenPoseSteps = nullptr;

public:
	void InitializeData(vector<OpenPoseData> * const openPoseData, UINT32 *openPoseSteps);

	void AnalyzeSteps();

public:
	OpenPoseStepAnalyzer();
	~OpenPoseStepAnalyzer();

private:
	UINT32 AnalyzeAddress(UINT32 topOfSwingStep);
	UINT32 AnalyzeTopOfSwing(UINT32 impactStep);
	UINT32 AnalyzeImpact();
	UINT32 AnalyzeFinish(UINT32 impactStep);
};

