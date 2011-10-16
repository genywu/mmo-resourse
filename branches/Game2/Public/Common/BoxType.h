/**
 *  brief 关于MessageBox的类型定义
 *  date  2010/06/12
 */
#pragma  once 

/// 选择结果
enum   eMsgResult
{
    S_INValid = -1,
    S_Ok,
    S_Mk, 
    S_Cancel,
    S_Esc,
};

// 消息提示框的风格
enum eMsgBoxType
{
    Msg_Ok,        ///> 只有一个确定按钮
    Msg_YesNo,     ///> 是，否
    Msg_GoOn,      ///> 继续，退出
    Msg_Agree,     ///> 同意，不同意
    Msg_Ok_Yes_No, ///> 确定，拒绝，取消
};