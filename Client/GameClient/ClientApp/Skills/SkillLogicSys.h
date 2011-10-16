/**
* @filename:SkillLogicSys.h
* @date: 2010/5/18
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 技能的逻辑层
*/

class SkillLogicSys
{
public:
	//向服务器发送设置mainbar快捷键为某个技能
	void Send_Set_Skill_HotKey(uint hotkeyIdex,uint skillID);
};