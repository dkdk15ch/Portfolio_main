#include "Animation/AnimationKeyFrame.h"

AnimationKeyFrame::AnimationKeyFrame() 
	: m_Time(-1.0)
	, m_Tick(-1.0)
	, m_Position(FLT_MAX, FLT_MAX, FLT_MAX)
	, m_Rotation(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX)
	, m_Scale(FLT_MAX, FLT_MAX, FLT_MAX)
{
}


