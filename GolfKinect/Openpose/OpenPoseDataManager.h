#pragma once
#include "../Game.h"

class OpenPoseStepAnalyzer;

enum EOpenPoseKey
{
    Nose,
    Neck,
    RShoulder,
    RElbow,
    RWrist,
    LShoulder,
    LElbow,
    LWrist,
    MidHip,
    RHip,
    RKnee,
    RAnkle,
    LHip,
    LKnee,
    LAnkle,
    REye,
    LEye,
    REar,
    LEar,
    LBigToe,
    LSmallToe,
    LHeel,
    RBigToe,
    RSmallToe,
    RHeel,
    End
};

struct OpenPoseData
{
	D2D1_POINT_2F m2DLocation[EOpenPoseKey::End] = { 0, };
    //float x[EOpenPoseKey::End] = { 0.f, };
    //float y[EOpenPoseKey::End] = { 0.f, };
    float Reliability[EOpenPoseKey::End] = { 0.f, }; // 신뢰도 0 ~ 1
    bool bTracked[EOpenPoseKey::End] = { 0.f, }; // 트레킹 여부
};

class OpenPoseDataManager
{
private:
    size_t m_iCurrentIndex = 0;
    vector<OpenPoseData> vecOpenPoseFrameData;
	UINT32 mOpenPoseDataStep[(UINT32)EGolfStepType::Size] = { 0, };

	OpenPoseStepAnalyzer * m_pStepAnalyzer = nullptr;

	vector<GraphData> mGraphData;

public:
    void Init(wstring wstrPath);

public:
    const vector<OpenPoseData> * GetData() const
    {
        return &vecOpenPoseFrameData;
    }
    size_t GetIndex() const
    {
        return m_iCurrentIndex;
    }

public:
    void SetIndex(size_t index)
    {
        if (vecOpenPoseFrameData.size() > index)
        {
            m_iCurrentIndex = index;
        }
    }

	const UINT32 * GetOpenPoseSteps();

public:
	// graph
	void InitGraphData(UINT32 graphSize);
	void SetGraphData(UINT32 graphNumber, UINT32 isCompar, UINT32 jointNumber);
	vector<GraphData> GetGraphData();
	void InsertGraphData(UINT32 graphSize);
	void DeleteGraphData(UINT32 graphSize);

private:
    void LoadDataFromFile(wstring wstrFilename);

    DECLARE_SINGLE(OpenPoseDataManager);
};

