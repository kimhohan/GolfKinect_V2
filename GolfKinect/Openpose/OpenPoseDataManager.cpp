#include "../Game.h"
#include "OpenPoseDataManager.h"
#include "OpenPoseStepAnalyzer.h"

DEFINITION_SINGLE(OpenPoseDataManager)

OpenPoseDataManager::OpenPoseDataManager()
{
}


OpenPoseDataManager::~OpenPoseDataManager()
{
	SAFE_DELETE(m_pStepAnalyzer);
}

void OpenPoseDataManager::Init(wstring wstrPath)
{
	m_pStepAnalyzer = new OpenPoseStepAnalyzer;

    for (int i = 0; i < 70; ++i)
    {
        wstring wPath = L"Test\\";
        wstring temp = L"golf_";
        wstring Zero = L"000000000000";
        if (i == 0)
        {
            temp = temp + Zero;
            wPath = wPath + temp + L"_keypoints.json";
            LoadDataFromFile(wPath);
            continue;
        }
        int digit = (int)(log10((float)i)) + 1; // 자릿수
        int maxZero = 12;
        Zero.erase(0, digit);
        temp = temp + Zero;
        temp = temp + to_wstring(i);

        wPath = wPath + temp + L"_keypoints.json";
        LoadDataFromFile(wPath);
    }

	m_pStepAnalyzer->InitializeData(&vecOpenPoseFrameData, mOpenPoseDataStep);
	m_pStepAnalyzer->AnalyzeSteps();
}

const UINT32 * OpenPoseDataManager::GetOpenPoseSteps()
{
	return mOpenPoseDataStep;
}

void OpenPoseDataManager::InitGraphData(UINT32 graphSize)
{
	mGraphData.resize(0);
	for (UINT32 i = 0; i < graphSize; ++i)
	{
		InsertGraphData(i);
	}
}

void OpenPoseDataManager::SetGraphData(UINT32 graphNumber, UINT32 isCompar, UINT32 jointNumber)
{
	mGraphData.at(graphNumber).IsComparative = isCompar;
	mGraphData.at(graphNumber).JointNumber = jointNumber;
}

vector<GraphData> OpenPoseDataManager::GetGraphData()
{
	return mGraphData;
}

void OpenPoseDataManager::InsertGraphData(UINT32 graphSize)
{
	mGraphData.insert(mGraphData.begin() + graphSize, { 2,0 });
}

void OpenPoseDataManager::DeleteGraphData(UINT32 graphSize)
{
	mGraphData.erase(mGraphData.begin() + graphSize);
}

void OpenPoseDataManager::LoadDataFromFile(wstring wstrFilename)
{
    string strOpenPosData;
    size_t pos;

    std::ifstream inFile(wstrFilename.c_str(), std::ifstream::binary);
    if (inFile.fail()) 
    {
        return;
    }

    inFile >> strOpenPosData;
    pos = strOpenPosData.find("\"pose_keypoints_2d\"");
    if (pos != std::string::npos)
    {
        pos += strlen("\"pose_keypoints_2d\":[");
        strOpenPosData.erase(0, pos);
        size_t endPos = strOpenPosData.find("]");
        strOpenPosData.erase(endPos, strOpenPosData.size());

        OpenPoseData OpenPoseData;
        for (int i = 0; i < EOpenPoseKey::End; ++i)
        {
            size_t nextPos = strOpenPosData.find(",");
            //if (nextPos == std::string::npos) break;
            string x = strOpenPosData.substr(0, nextPos);
            strOpenPosData = strOpenPosData.substr(nextPos + 1);
            OpenPoseData.m2DLocation[i].x = strtof(x.c_str(), nullptr);

            nextPos = strOpenPosData.find(",");
            string y = strOpenPosData.substr(0, nextPos);
            strOpenPosData = strOpenPosData.substr(nextPos + 1);
            OpenPoseData.m2DLocation[i].y = strtof(y.c_str(), nullptr);

            nextPos = strOpenPosData.find(",");
            string reliability = strOpenPosData.substr(0, nextPos);
            strOpenPosData = strOpenPosData.substr(nextPos + 1);
            OpenPoseData.Reliability[i] = strtof(reliability.c_str(), nullptr);

			if (OpenPoseData.m2DLocation[i].x == 0 || OpenPoseData.m2DLocation[i].y == 0)
				OpenPoseData.bTracked[i] = false;
			else
				OpenPoseData.bTracked[i] = true;
        }
        vecOpenPoseFrameData.push_back(OpenPoseData);
    }
    else
    {
        //MessageBox(NULL, wstrFilename.c_str(), L"잘못된 Data file", MB_OK);
        //exit(0);
    }
}
