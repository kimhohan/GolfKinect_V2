#pragma once
#include "../Game.h"

class KinectDataAngle;
class KinectDataDistance;
class StepAnalyzer;
class FrameManager;

class KinectDataManager
{
private:
    wstring mVersion = L"KG01";
	bool bIndexUpdated = false;
	UINT32 mCurrentFrame = 0;
	vector<GraphData> mGraphData;

	KinectDataAngle * mpKinectDataAngle = nullptr;
	KinectDataDistance * mpKinectDataDistance = nullptr;
	//KinectDataAnnotation * mpKinectDataAnnotation = nullptr;

	StepAnalyzer * mpStepAnalyzer = nullptr;

    // Kinect Data
    FrameManager * mpKinectDataFrameManager = nullptr;    // KINECTDATA 별로 별도 관리 필요
	vector<KinectDataV1 *> mKinectData;
    UINT32 mKinectDataStep[(UINT32)EGolfStepType::Size] = { 0, };

    // Kinect Comparative Data
    FrameManager * mpKinectComparativeDataFrameManager = nullptr;    // KINECTDATA 별로 별도 관리 필요
    vector<KinectDataV1 *> mKinectComparativeData;
    UINT32 mKinectComparativeDataStep[(UINT32)EGolfStepType::Size] = { 0, };

	bool bCaptured = false;
	vector<KinectDataV1 *> mCapturedData;
    UINT32 mKinectCapturedDataStep[(UINT32)EGolfStepType::Size] = { 0, };

public:
	void Initialize();
	void Update();
	void ReleaseLoadedKinectData();
    void ReleaseLoadedKinectComparativeData();
    void ReleaseAllData();
	void Release();

	void LoadKinectData(LPCWSTR path);
    void LoadKinectComparativeData(LPCWSTR path);

	void SaveCapturedData(wstring filename);
    void SaveKinectData(wstring filename);

    vector<KINECTDATA *> * const GetCapturedData();

    const vector<KINECTDATA *> * const GetKinectData();
    FrameManager * const GetFrameManager();

    const vector<KINECTDATA *> * const GetKinectComparativeData();
    FrameManager * const GetComparativeFrameManager();

    const UINT32 *                GetKinectSteps();
    const UINT32 *                GetKinectComparativeSteps();

	bool IsCapturing();
	void SetCapture(bool flag);

	void InitGraphData(UINT32 graphSize);
	void SetGraphData(UINT32 graphNumber, UINT32 isCompar, UINT32 jointNumber);
	vector<GraphData> GetGraphData();
	void InsertGraphData(UINT32 graphSize);
	void DeleteGraphData(UINT32 graphSize);

	DECLARE_SINGLE(KinectDataManager)

private:
    void LoadData(LPCWSTR path, vector<KINECTDATA *> * const kinectData, UINT32 * kinectDataStep, FrameManager * frameManager);
    void SaveData(wstring filename, const vector<KINECTDATA*>* const kinectData, UINT32 * kinectDataStep);
    void AnalysisData(const vector<KINECTDATA*>* const kinectData, UINT32 * kinectDataStep);
};