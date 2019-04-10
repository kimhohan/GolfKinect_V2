#include "OpenPoseStepAnalyzer.h"

FLOAT distance2D2(D2D1_POINT_2F a, D2D1_POINT_2F b)
{
	return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

void OpenPoseStepAnalyzer::InitializeData(vector<OpenPoseData>* const openPoseData, UINT32 * openPoseSteps)
{
	assert(openPoseData);
	vecOpenPoseFrameData = openPoseData;
	m_pOpenPoseSteps = openPoseSteps;
}

void OpenPoseStepAnalyzer::AnalyzeSteps()
{
	m_pOpenPoseSteps[(UINT32)EGolfStepType::Impact] = AnalyzeImpact();
	m_pOpenPoseSteps[(UINT32)EGolfStepType::Finish] = AnalyzeFinish(m_pOpenPoseSteps[(UINT32)EGolfStepType::Impact]);
	m_pOpenPoseSteps[(UINT32)EGolfStepType::TopOfSwing] = AnalyzeTopOfSwing(m_pOpenPoseSteps[(UINT32)EGolfStepType::Impact]);
	m_pOpenPoseSteps[(UINT32)EGolfStepType::Address] = AnalyzeAddress(m_pOpenPoseSteps[(UINT32)EGolfStepType::TopOfSwing]);
	m_pOpenPoseSteps[(UINT32)EGolfStepType::Begin] = 0;
	m_pOpenPoseSteps[(UINT32)EGolfStepType::End] = vecOpenPoseFrameData->size() - 1;
}

OpenPoseStepAnalyzer::OpenPoseStepAnalyzer()
{
}


OpenPoseStepAnalyzer::~OpenPoseStepAnalyzer()
{
}

UINT32 OpenPoseStepAnalyzer::AnalyzeAddress(UINT32 topOfSwingStep)
{
	D2D1_POINT_2F *joint2ds;
	UINT32 i = 0;
	UINT32 minIndex_DistanceOfHandAndHipCenter_x = i;
	FLOAT min_DistanceOfHandAndHipCenter_x = 9999999.f;
	FLOAT hipCenter_x;
	FLOAT handCenter_x;

	//손의 중점과 골반 중점 사이 최솟값의 인덱스를 찾습니다.
	for (i = 0; i < topOfSwingStep; i++)
	{
		joint2ds = vecOpenPoseFrameData->at(i).m2DLocation;
		hipCenter_x = (joint2ds[EOpenPoseKey::RHip].x + joint2ds[EOpenPoseKey::LHip].x) / 2;
		handCenter_x = (joint2ds[EOpenPoseKey::RWrist].x + joint2ds[EOpenPoseKey::LWrist].x) / 2;
		if (abs(hipCenter_x - handCenter_x) < min_DistanceOfHandAndHipCenter_x)
		{
			minIndex_DistanceOfHandAndHipCenter_x = i;
			min_DistanceOfHandAndHipCenter_x = abs(hipCenter_x - handCenter_x);
		}
	}

	return minIndex_DistanceOfHandAndHipCenter_x;
}

UINT32 OpenPoseStepAnalyzer::AnalyzeTopOfSwing(UINT32 impactStep)
{
	UINT32 i = 1;
	D2D1_POINT_2F *joint2ds = vecOpenPoseFrameData->at(i - 1).m2DLocation;
	D2D1_POINT_2F handCenter;
	D2D1_POINT_2F previous_handCenter = { (joint2ds[EOpenPoseKey::RWrist].x + joint2ds[EOpenPoseKey::LWrist].x) / 2,
		(joint2ds[EOpenPoseKey::RWrist].y + joint2ds[EOpenPoseKey::LWrist].y) / 2 };
	UINT32 minIndex_velocityHandCenter = i;
	FLOAT min_velocityHandCenter = 0;

	//양손의 중점 속도가 최소인 임팩트 스텝 이전의 인덱스를 찾습니다.
	for (; i < impactStep; i++)
	{
		joint2ds = vecOpenPoseFrameData->at(i).m2DLocation;
		handCenter.x = (joint2ds[EOpenPoseKey::RWrist].x + joint2ds[EOpenPoseKey::LWrist].x) / 2;
		handCenter.y = (joint2ds[EOpenPoseKey::RWrist].y + joint2ds[EOpenPoseKey::LWrist].y) / 2;
		if (min_velocityHandCenter < distance2D2(handCenter, previous_handCenter) &&
			(joint2ds[EOpenPoseKey::RWrist].y + joint2ds[EOpenPoseKey::LWrist].y) / 2 < joint2ds[EOpenPoseKey::RShoulder].y  &&	//양 손의 중점이 오른쪽 어깨 위이어야 함
			vecOpenPoseFrameData->at(i).bTracked[EOpenPoseKey::LWrist] != false && //현재 프레임 양 손이 트랙되어야 함
			vecOpenPoseFrameData->at(i).bTracked[EOpenPoseKey::RWrist] != false &&
			vecOpenPoseFrameData->at(i - 1).bTracked[EOpenPoseKey::LWrist] != false && //이전 프레임 양 손이 트랙되어야 함
			vecOpenPoseFrameData->at(i - 1).bTracked[EOpenPoseKey::RWrist] != false
			)
		{
			minIndex_velocityHandCenter = i;
			min_velocityHandCenter = abs(previous_handCenter.x - handCenter.x);//distance(handCenter, previous_handCenter);
		}
		previous_handCenter.x = (joint2ds[EOpenPoseKey::RWrist].x + joint2ds[EOpenPoseKey::LWrist].x) / 2;
		previous_handCenter.y = (joint2ds[EOpenPoseKey::RWrist].y + joint2ds[EOpenPoseKey::LWrist].y) / 2;
	}
	return minIndex_velocityHandCenter;
}

UINT32 OpenPoseStepAnalyzer::AnalyzeImpact()
{
	UINT32 i = 1;
	D2D1_POINT_2F *joint2ds = vecOpenPoseFrameData->at(i - 1).m2DLocation;
	D2D1_POINT_2F handCenter;
	// https://github.com/CMU-Perceptual-Computing-Lab/openpose/blob/master/doc/output.md
	D2D1_POINT_2F previous_handCenter = 
	{ 
		(joint2ds[EOpenPoseKey::RWrist].x + joint2ds[EOpenPoseKey::LWrist].x) / 2, 
		(joint2ds[EOpenPoseKey::RWrist].y + joint2ds[EOpenPoseKey::LWrist].y) / 2
	};
	UINT32 minIndex_velocityHandCenter = i;
	FLOAT min_velocityHandCenter = 0;

	//양손의 중점 속도가 최소인 인덱스를 찾습니다.
	for (; i < vecOpenPoseFrameData->size(); i++)
	{
		joint2ds = vecOpenPoseFrameData->at(i).m2DLocation;
		handCenter.x = (joint2ds[EOpenPoseKey::RWrist].x + joint2ds[EOpenPoseKey::LWrist].x) / 2;
		handCenter.y = (joint2ds[EOpenPoseKey::RWrist].y + joint2ds[EOpenPoseKey::LWrist].y) / 2;
		if (min_velocityHandCenter < abs(previous_handCenter.x - handCenter.x) &&//distance(handCenter, previous_handCenter) &&
			vecOpenPoseFrameData->at(i).bTracked[EOpenPoseKey::LWrist] != false && //현재 프레임 양 손이 트랙되어야 함
			vecOpenPoseFrameData->at(i).bTracked[EOpenPoseKey::RWrist] != false &&
			vecOpenPoseFrameData->at(i - 1).bTracked[EOpenPoseKey::LWrist] != false && //이전 프레임 양 손이 트랙되어야 함
			vecOpenPoseFrameData->at(i - 1).bTracked[EOpenPoseKey::RWrist] != false
			)
		{
			minIndex_velocityHandCenter = i;
			min_velocityHandCenter = abs(previous_handCenter.x - handCenter.x);//distance(handCenter, previous_handCenter);
		}
		previous_handCenter.x = (joint2ds[EOpenPoseKey::RWrist].x + joint2ds[EOpenPoseKey::LWrist].x) / 2;
		//previous_handCenter.y = (joint2ds[EOpenPoseKey::RWrist].y + joint2ds[EOpenPoseKey::LWrist].y) / 2;
	}
	return minIndex_velocityHandCenter;
}

UINT32 OpenPoseStepAnalyzer::AnalyzeFinish(UINT32 impactStep)
{
	UINT32 i = impactStep + 1;
	D2D1_POINT_2F *joint2ds = vecOpenPoseFrameData->at(i - 1).m2DLocation;
	D2D1_POINT_2F handCenter;
	D2D1_POINT_2F previous_handCenter = { (joint2ds[EOpenPoseKey::RWrist].x + joint2ds[EOpenPoseKey::LWrist].x) / 2, 
		(joint2ds[EOpenPoseKey::RWrist].y + joint2ds[EOpenPoseKey::LWrist].y) / 2 };
	UINT32 minIndex_velocityHandCenter = i;
	FLOAT min_velocityHandCenter = 0;

	//양손의 중점 속도가 최소인 임팩트 스텝 이후의 인덱스를 찾습니다.
	for (; i < vecOpenPoseFrameData->size(); i++)
	{
		joint2ds = vecOpenPoseFrameData->at(i).m2DLocation;
		handCenter.x = (joint2ds[EOpenPoseKey::RWrist].x + joint2ds[EOpenPoseKey::LWrist].x) / 2;
		handCenter.y = (joint2ds[EOpenPoseKey::RWrist].y + joint2ds[EOpenPoseKey::LWrist].y) / 2;
		if (min_velocityHandCenter < distance2D2(handCenter, previous_handCenter) &&
			(joint2ds[EOpenPoseKey::RWrist].y + joint2ds[EOpenPoseKey::LWrist].y) / 2 < joint2ds[EOpenPoseKey::RShoulder].y  &&	//양 손의 중점이 오른쪽 어깨 위이어야 함
			vecOpenPoseFrameData->at(i).bTracked[EOpenPoseKey::LWrist] != false && //현재 프레임 양 손이 트랙되어야 함
			vecOpenPoseFrameData->at(i).bTracked[EOpenPoseKey::RWrist] != false &&
			vecOpenPoseFrameData->at(i - 1).bTracked[EOpenPoseKey::LWrist] != false && //이전 프레임 양 손이 트랙되어야 함
			vecOpenPoseFrameData->at(i - 1).bTracked[EOpenPoseKey::RWrist] != false
			)
		{
			minIndex_velocityHandCenter = i;
			min_velocityHandCenter = abs(previous_handCenter.x - handCenter.x);//distance(handCenter, previous_handCenter);
		}
		previous_handCenter.x = (joint2ds[EOpenPoseKey::RWrist].x + joint2ds[EOpenPoseKey::LWrist].x) / 2;
		previous_handCenter.y = (joint2ds[EOpenPoseKey::RWrist].y + joint2ds[EOpenPoseKey::LWrist].y) / 2;
	}
	return minIndex_velocityHandCenter;
}
