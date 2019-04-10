#include "KinectDataManager.h"
#include "KinectManager.h"
#include "KinectBody.h"

#include "FrameManager.h"
#include "StepAnalyzer.h"
#include "KinectDataAngle.h"
#include "KinectDataDistance.h"
#include "KinectDataAnnotation.h"

DEFINITION_SINGLE(KinectDataManager)

KinectDataManager::KinectDataManager()
{
}

KinectDataManager::~KinectDataManager()
{
}

void KinectDataManager::Initialize()
{
	mpKinectDataAngle = new KinectDataAngle();
	mpKinectDataDistance = new KinectDataDistance();
	mpStepAnalyzer = new StepAnalyzer();
    mpKinectDataFrameManager = new FrameManager(&mKinectData);
    mpKinectComparativeDataFrameManager = new FrameManager(&mKinectComparativeData);
}

void KinectDataManager::Update()
{
	if (mpKinectDataFrameManager)
	{
		mpKinectDataFrameManager->Update();
	}

    if (mpKinectComparativeDataFrameManager)
    {
        mpKinectComparativeDataFrameManager->Update();
    }
}

void KinectDataManager::ReleaseLoadedKinectData()
{
	if (mKinectData.size() != 0)
	{
        // 0만 하면 다 해제되니 건들지 말것.
		SAFE_DELETE_ARRAY(mKinectData[0]);
		mKinectData.clear();
	}
    ZeroMemory(mKinectDataStep, sizeof(UINT32) * (UINT32)EGolfStepType::Size);
}

void KinectDataManager::ReleaseLoadedKinectComparativeData()
{
    if (mKinectComparativeData.size() != 0)
    {
        SAFE_DELETE_ARRAY(mKinectComparativeData[0]);
        mKinectComparativeData.clear();
    }
    ZeroMemory(mKinectComparativeDataStep, sizeof(UINT32) * (UINT32)EGolfStepType::Size);
}

void KinectDataManager::ReleaseAllData()
{
    ReleaseLoadedKinectData();
    ReleaseLoadedKinectComparativeData();
    bCaptured = false;
    mCapturedData.clear();
}

void KinectDataManager::LoadKinectData(LPCWSTR path)
{
    ReleaseLoadedKinectData();
    LoadData(path, &mKinectData, mKinectDataStep, mpKinectDataFrameManager);
}

void KinectDataManager::LoadKinectComparativeData(LPCWSTR path)
{
    ReleaseLoadedKinectComparativeData();
    LoadData(path, &mKinectComparativeData, mKinectComparativeDataStep, mpKinectComparativeDataFrameManager);
}

void KinectDataManager::SaveCapturedData(wstring filename)
{
    mpKinectComparativeDataFrameManager->Initialize();
    AnalysisData(&mCapturedData, mKinectCapturedDataStep);

    SaveData(filename, &mCapturedData, mKinectCapturedDataStep);
}

void KinectDataManager::SaveKinectData(wstring filename)
{
    SaveData(filename, &mKinectData, mKinectDataStep);
}

bool KinectDataManager::IsCapturing()
{
	return bCaptured;
}

vector<KINECTDATA*>* const KinectDataManager::GetCapturedData()
{
	return &mCapturedData;
}
 
void KinectDataManager::SetCapture(bool flag)
{
	if (flag == true)
	{
		mCapturedData.clear();
	}
	bCaptured = flag;
}

void KinectDataManager::InitGraphData(UINT32 graphSize)
{
	mGraphData.resize(0);
	for (UINT32 i = 0; i < graphSize; ++i)
	{
		InsertGraphData(i);
	}
}

void KinectDataManager::SetGraphData(UINT32 graphNumber, UINT32 isCompar, UINT32 jointNumber)
{
	mGraphData.at(graphNumber).IsComparative = isCompar;
	mGraphData.at(graphNumber).JointNumber = jointNumber;
}

vector<GraphData> KinectDataManager::GetGraphData()
{
	return mGraphData;
}

void KinectDataManager::InsertGraphData(UINT32 graphSize)
{
	mGraphData.insert(mGraphData.begin() + graphSize, { 2,0 });
}

void KinectDataManager::DeleteGraphData(UINT32 graphSize)
{
	mGraphData.erase(mGraphData.begin() + graphSize);
}

void KinectDataManager::LoadData(LPCWSTR path, vector<KINECTDATA*>* const kinectData, UINT32 * kinectDataStep, FrameManager * frameManager)
{
    assert(kinectData);
    assert(kinectDataStep);
    assert(frameManager);
    FILE *fp = _wfopen(path, L"rb");

    wstring version;
    wchar_t _version[5] = { 0, };
    fread(_version, sizeof(wchar_t), 4, fp);
    version = _version;

    if (version != L"KG01")
    {
        MessageBox(nullptr, L"과거 버전입니다.", L"version", MB_OK);
        fseek(fp, 0, SEEK_SET);
    }

	UINT32 indexCount = 0;
    fread((void *)&indexCount, sizeof(UINT32), 1, fp);
    if (indexCount != 0)
    {
        KINECTDATA * data = new KINECTDATA[indexCount];
        ZeroMemory(data, sizeof(KINECTDATA) * indexCount);

        if (version == L"KG01")
        {
            fread(kinectDataStep, sizeof(UINT32), (UINT32)EGolfStepType::Size, fp);
            fread(data, sizeof(KINECTDATA), indexCount, fp);
        }
        else
        {
            KinectData * loadData = new KinectData[indexCount];
            fread(loadData, sizeof(KinectData), indexCount, fp);

            for (unsigned int i = 0; i < indexCount; ++i)
            {
                memcpy(&data[i], &loadData[i], sizeof(KinectData));
            }
            SAFE_DELETE_ARRAY(loadData);
        }

        for (UINT32 i = 0; i < indexCount; ++i)
        {
            kinectData->push_back(&data[i]);
        }

        frameManager->Initialize();

        UINT32 temp[(UINT32)EGolfStepType::Size] = { 0, };
        if (memcmp(kinectDataStep, temp, sizeof(UINT32) * (UINT32)EGolfStepType::Size) == 0)
        {
            AnalysisData(kinectData, kinectDataStep);
        }
    }
    fclose(fp);
}

const UINT32 * KinectDataManager::GetKinectSteps()
{
	return mKinectDataStep;
}

const UINT32 * KinectDataManager::GetKinectComparativeSteps()
{
    return mKinectComparativeDataStep;
}

void KinectDataManager::Release()
{
    ReleaseLoadedKinectData();
    ReleaseLoadedKinectComparativeData();

	SAFE_DELETE(mpStepAnalyzer);
	SAFE_DELETE(mpKinectDataFrameManager);
    SAFE_DELETE(mpKinectComparativeDataFrameManager);
	SAFE_DELETE(mpKinectDataDistance);
	SAFE_DELETE(mpKinectDataAngle);
}

const vector<KINECTDATA*>* const KinectDataManager::GetKinectData()
{
	return &mKinectData;
}

FrameManager * const KinectDataManager::GetFrameManager()
{
	assert(mpKinectDataFrameManager);
	return mpKinectDataFrameManager;
}

const vector<KINECTDATA*>* const KinectDataManager::GetKinectComparativeData()
{
    return &mKinectComparativeData;
}

FrameManager * const KinectDataManager::GetComparativeFrameManager()
{
    return mpKinectComparativeDataFrameManager;
}

void KinectDataManager::SaveData(wstring filename, const vector<KINECTDATA*>* const kinectData, UINT32 * kinectDataStep)
{
    if (kinectData->size() > 0)
    {
        if (filename.rfind(L".dat") == std::wstring::npos && filename.size() < 250)
        {
            filename += L".dat";
        }
        FILE *fp = _wfopen(filename.c_str(), L"wb");

        fwrite(mVersion.c_str(), sizeof(wchar_t) * mVersion.size(), 1, fp);

		UINT32 indexcount = (UINT32)kinectData->size();
        fwrite(&indexcount, sizeof(UINT32), 1, fp);
        fwrite(kinectDataStep, sizeof(UINT32), (UINT32)EGolfStepType::Size, fp);
        fwrite(kinectData->at(0), sizeof(KinectDataV1) * kinectData->size(), 1, fp);
        fclose(fp);
    }
    else
    {
        MessageBox(nullptr, L"저장할 데이터가 없습니다.", L"save", MB_OK);
    }
}

void KinectDataManager::AnalysisData(const vector<KINECTDATA*>* const kinectData, UINT32 * kinectDataStep)
{
    assert(kinectData);
    assert(kinectDataStep);

    mpKinectDataAngle->InitializeData(kinectData);
    mpKinectDataDistance->InitializeData(kinectData);
    mpStepAnalyzer->InitializeData(kinectData, kinectDataStep);

	mpKinectDataAngle->SetSethaToPoint();
	mpKinectDataDistance->SetDistanceToPoint();
    mpStepAnalyzer->AnalyzeSteps();
}