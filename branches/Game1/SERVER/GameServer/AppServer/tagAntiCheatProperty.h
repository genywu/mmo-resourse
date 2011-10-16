#pragma  once

namespace AntiCheat
{
	struct tagTradeAreaProperty 
	{
		int nActFactor;					/// 活跃度
		int nAccWrongTimes;				///	累计答错次数
		int nCurWrongTimes;				/// 当前答错次数
		int nCurRightTimes;				/// 连续答对次数
	};
}