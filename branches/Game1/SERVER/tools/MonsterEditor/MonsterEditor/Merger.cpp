// Merger.cpp : 实现文件
//

#include "stdafx.h"
#include "MonsterEditor.h"
#include "Merger.h"


// CMerger 对话框
vector<tagMonster>vecMerMon;//装各文件记录的容器

IMPLEMENT_DYNAMIC(CMerger, CDialog)

CMerger::CMerger(CWnd* pParent /*=NULL*/)
	: CDialog(CMerger::IDD, pParent)
{

}

CMerger::~CMerger()
{
}

void CMerger::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMerger, CDialog)
	ON_BN_CLICKED(IDC_RADIO1, &CMerger::OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CMerger::OnRadio2)
	ON_BN_CLICKED(IDC_BUTTON1, &CMerger::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON17, &CMerger::OnBnClickedButton17)
	ON_BN_CLICKED(IDC_BUTTON18, &CMerger::OnBnClickedButton18)
	ON_BN_CLICKED(IDC_BUTTON2, &CMerger::OnMerger)
END_MESSAGE_MAP()


// CMerger 消息处理程序
BOOL CMerger::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	return TRUE;
}

//DOWRD转换为CString

CString MergerDwordToString(DWORD n)
{

	CString str;

	str.Format("%d",n);
	return str;
}

void CMerger::OnRadio1()
{
	// TODO: 在此添加控件通知处理程序代码
	SetDlgItemText(IDC_STATIC1,"打开monsterlist.ini的文件");
	GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
}

void CMerger::OnRadio2()
{
	// TODO: 在此添加控件通知处理程序代码
	SetDlgItemText(IDC_STATIC1,"打开monsterskilllist.ini的文件");
	GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
}

void CMerger::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString FilePathName;
	CFileDialog dlg(TRUE);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	if(dlg.DoModal()==IDOK)
	FilePathName=dlg.GetPathName();
	SetDlgItemText(IDC_EDIT1,FilePathName);
}

void CMerger::OnBnClickedButton17()
{
	// TODO: 在此添加控件通知处理程序代码
	CString FilePathName;
	CFileDialog dlg(TRUE);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	if(dlg.DoModal()==IDOK)
	FilePathName=dlg.GetPathName();
	SetDlgItemText(IDC_EDIT2,FilePathName);
}

void CMerger::OnBnClickedButton18()
{
	// TODO: 在此添加控件通知处理程序代码
	CString FilePathName;
	CFileDialog dlg(TRUE);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	if(dlg.DoModal()==IDOK)
	FilePathName=dlg.GetPathName();
	SetDlgItemText(IDC_EDIT3,FilePathName);
}
//点击了合并另存为按钮
void CMerger::OnMerger()
{
	// TODO: 在此添加控件通知处理程序代码

	//清空vecMonsterSkillMerger，每次合并保存时不会将以前的数据写入文件
	vecMerMon.clear();
	CString str1,str2,str3;
	//获取EDIT文件路径
	GetDlgItemText(IDC_EDIT1,str1);
	GetDlgItemText(IDC_EDIT2,str2);
	GetDlgItemText(IDC_EDIT3,str3);
	//写将记录写入vecMerMon
	vector<tagMonster>::iterator iter;
	vector<tagSkill>::iterator iterskill;
	//合并后另存为文件名
	CString FilePathName;
	CFileDialog dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"Text File(*.txt)|*.*| |",AfxGetMainWnd());///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	if(dlg.DoModal()==IDOK)
	FilePathName=dlg.GetPathName();
	if(FilePathName=="")
		return;
	FilePathName+=".txt";
	//选中第一个radio
	if(IsDlgButtonChecked(IDC_RADIO1))
	{
		//当文件路径不为空时执行
		if(str1!="")
		{
		if(!MergerFile(str1))
			return;
		}
		if(str2!="")
		{
		if(!MergerFile(str2))
			return;
		}
		if(str3!="")
		{
		if(!MergerFile(str3))
			return;
		}
		if(FilePathName!="")
		{
			//先清空文件内容
			ofstream file;
			file.open(FilePathName);
			file.close();
			CFile   file3;   
			file3.Open(FilePathName,   CFile::modeReadWrite); 
			//文件长度不为0,清空文件
			if(file3.GetLength()!=0)
			{
				file3.SetLength(0);   
			}
			file3.Close();
				
			file.open(FilePathName);
			if(!file.is_open())
			{
				MessageBox("无法打开文件");
				return;
			}
			
			
			int i=0;
			//输出到文件
			for(iter=vecMerMon.begin();iter!=vecMerMon.end();iter++,i++)
			{
				file<<"*\t";
				file<<iter->dwIndex
					<<"\t"
					<<iter->strOrigName
					<<"\t"
					<<iter->strName
					<<"\t"
					<<iter->strBlock
					<<"\t"
					<<iter->strSuper
					<<"\t"
					<<iter->strCollection
					<<"\t"
					<<iter->strTitle
					<<"\t"
					<<iter->dwTitleColor
					<<"\t"
					<<iter->dwTitlePicture
					<<"\t"
					<<iter->strCountryID
					<<"\t"
					<<iter->strNpcSort
					<<"\t"
					<<iter->dwPicID
					<<"\t"
					<<iter->dwPicLevel
					<<"\t"
					<<iter->dwNameColorID
					<<"\t"
					<<iter->dwLevelColorID
					<<"\t"
					<<iter->dwHpColor
					<<"\t"
					<<iter->dwSoundID
					<<"\t"
					<<iter->strTame
					<<"\t"
					<<iter->dwTameTimes
					<<"\t"
					<<iter->strShape
					<<"\t"
					<<iter->nLevel
					<<"\t"
					<<iter->dwExp
					<<"\t"
					<<iter->dwWorkPoint
					<<"\t"
					<<iter->dwMaxHp
					<<"\t"
					<<iter->wMinAttack
					<<"\t"
					<<iter->wMaxAttack
					<<"\t"
					<<iter->wMinMagicAttack
					<<"\t"
					<<iter->wMaxMagicAttack
					<<"\t"
					<<iter->wHitRating
					<<"\t"
					<<iter->wDefense
					<<"\t"
					<<iter->wMagicDefense
					<<"\t"
					<<iter->wBlock
					<<"\t"
					<<iter->wParry
					<<"\t"
					<<iter->wDodge
					<<"\t"
					<<iter->fMoveSpd
					<<"\t"
					<<iter->fBatMoveSpd
					<<"\t"
					<<iter->wWillAnti
					<<"\t"
					<<iter->wGroundAnti
					<<"\t"
					<<iter->wWaterAnti
					<<"\t"
					<<iter->wFireAnti
					<<"\t"
					<<iter->wWindAnti
					<<"\t"
					<<iter->wSacreAnti
					<<"\t"
					<<iter->wDarkAnti
					<<"\t"
					<<iter->wNotElemAnti
					<<"\t"
					<<iter->wHpRecoverSpd
					<<"\t"
					<<iter->wMpRecoverSpd
					<<"\t"
					<<iter->wBodyAnti
					<<"\t"
					<<iter->wBloodAnti
					<<"\t"
					<<iter->wDuskAnti
					<<"\t"
					<<iter->wNumbAnti
					<<"\t"
					<<iter->wSleepAnti
					<<"\t"
					<<iter->wDrugAnti
					<<"\t"
					<<iter->wNotMoveAnti
					<<"\t"
					<<iter->wMinStayTimes
					<<"\t"
					<<iter->wMaxStayTimes
					<<"\t"
					<<iter->wMinWalkTimes
					<<"\t"
					<<iter->wMaxWalkTimes
					<<"\t"
					<<iter->wAlertRange
					<<"\t"
					<<iter->wPurRange
					<<"\t"
					<<iter->wPeaMoveRange
					<<"\t"
					<<iter->strAIModel
					<<"\t"
					<<iter->bTalk
					<<"\t"
					<<iter->strMonRace
					<<"\t"
					<<iter->strElem
					<<"\t"
					<<iter->wStayTime
					<<"\t"
					<<iter->strAIWay
					<<"\t"
					<<iter->wHurtHuman
					<<"\t"
					<<iter->wHurtElem
					<<"\t"
					<<iter->wHurtDiff
					<<"\t"
					<<iter->wHurtAnimal
					<<"\t"
					<<iter->wHurtDead
					<<"\t"
					<<iter->wHurtDevil
					<<"\t"
					<<iter->wHurtInsect
					<<"\t"
					<<iter->wHurtFire
					<<"\t"
					<<iter->wHurtWater
					<<"\t"
					<<iter->wHurtGround
					<<"\t"
					<<iter->wHurtWind
					<<"\t"
					<<iter->wHurtSacre
					<<"\t"
					<<iter->wHurtDark
					<<"\t"
					<<iter->wHurtNotElem
					<<"\t"
					<<iter->wVitality
					<<"\t"
					<<iter->wStormHit
					<<"\t"
					<<iter->wStormHurt
					<<"\t"
					<<iter->wStormAnti
					<<"\t"
					<<iter->wMagicStorm
					<<"\t"
					<<iter->wMagicStormHurt  
					<<"\t"
					<<iter->wMagicStormAnti
					<<"\t"
					<<iter->fInjurRate
					<<"\t"
					<<iter->wAlertTime
					<<"\t"
					<<iter->fEscape
					<<"\t"
					<<iter->wSkillRate
					<<"\t"
					<<iter->wSkillInter
					<<"\t"
					<<iter->strReturn
					<<"\t"
					<<iter->strAllRecover
					<<"\t"
					<<iter->wReDistance
					<<"\t"
					<<iter->wReTime
					<<"\t"
					<<iter->strRefe1
					<<"\t"
					<<iter->strRefe2
					<<"\t"
					<<iter->strRefe3
					<<"\t"
					<<iter->strRefe4
					<<"\t"
					<<iter->strRefe5
					<<"\t"
					<<iter->strBirthSct
					<<"\t"
					<<iter->strBattleSct
					<<"\t"
					<<iter->strInjurSct
					<<"\t"
					<<iter->strDeadSct
					<<"\t"
					<<iter->strCycleSct
					<<"\t"
					<<iter->strChoseSkillSct
					<<"\t"
					<<iter->strFindTgtSct
					<<"\t"
					<<iter->strKillTgtSct;
					
				file<<"\n";
				}
			}

		}
		//选中第二个radio
		else if(IsDlgButtonChecked(IDC_RADIO2))
		{
			//当文件路径不为空时
			if(str1!="")
			{
				if(!MergerFileSkill(str1))
					return;
			}
			if(str2!="")
			{
				if(!MergerFileSkill(str2))
					return;
			}
			if(str3!="")
			{
				if(!MergerFileSkill(str3))
					return;
			}
			if(FilePathName!="")
			{
				//建立文件
				FILE *fp;
				fp=fopen(FilePathName,"a");
				fclose(fp);
				CFile   file3;  
				//先清空文件内容
				file3.Open(FilePathName,   CFile::modeReadWrite); 
				if(file3.GetLength()!=0)
				{
					file3.SetLength(0);   
				}
				file3.Close();
				fstream fileskill;
				setlocale(LC_ALL,"Chinese-simplified");//设置中文环境
				fileskill.open(FilePathName);
				if(!fileskill.is_open())
				{
					MessageBox("无法打开文件");
					return;
				}
				//输出到文件
				for(iter=vecMerMon.begin();iter!=vecMerMon.end();iter++)
					{
						
						//如果有技能记录，则写入文件
						if((int)iter->vecSkill.size()>0)
						{
							fileskill<<"*\t"
									<<iter->strOrigName
									<<"\n";
							
							for(iterskill=iter->vecSkill.begin();iterskill!=
								iter->vecSkill.end();
								iterskill++)
							{
								
								//技能类型为">"，是基本技能
								if(iterskill->strSkillType==">")
								{
									fileskill<<">"
										<<"\t"
										<<iterskill->dwSkillID
										<<"\t"
										<<iterskill->wSkillLevel;
									fileskill<<"\n";
										
								}
								//技能类型为"#",是普通技能
								else if(iterskill->strSkillType=="#")
								{
									fileskill<<"#"
										<<"\t"
										<<iterskill->dwSkillID
										<<"\t"
										<<iterskill->wSkillLevel;
									fileskill<<"\n";

								}
								//技能类型为"@"，是周期技能
								else if(iterskill->strSkillType=="@")
								{
									fileskill<<"@"
										<<"\t"
										<<iterskill->dwSkillID
										<<"\t"
										<<iterskill->wSkillLevel
										<<"\t"
										<<iterskill->wInterTime;
									fileskill<<"\n";
								}
							}
						}
						else
							continue;

					}
					fileskill.close();
			}

		}
}
//将光标移动到“*”之后
int ReadTot(fstream &stream,string name)
{
		string str;
	stream >> str;
	while (str != name)
	{
		//如果已到文件末尾，返回0
		if (stream.eof())
		{

			return 0;
		}
		//读取下一个串
		stream >> str;
	}
	return 1;
}
//判断是索引是否有重复
bool CMerger::IsSame(CString strfilename,tagMonster *monster)
{	
	vector<tagMonster>::iterator iter;
	//检查合并文件中是否有索引或原始名相同的记录
	for(iter=vecMerMon.begin();iter!=vecMerMon.end();iter++)
	{
		//索引相同
		if(iter->dwIndex==(*monster).dwIndex)
			{
				char strVal[100];
				sprintf_s(strVal,"'%s'发现索引相同的记录,是否覆盖原记录",strfilename);
				int ret=MessageBox(strVal,"MonsterEditor",MB_YESNO);
				//覆盖原记录
				if(ret==IDYES)
				{
					vecMerMon.erase(iter);
					vecMerMon.push_back(*monster);					
					return true;
				}
				//保留原记录
				else
					return true;
			}
		//原始名相同
		if(iter->strOrigName==(*monster).strOrigName)
			{
				char strVal[100];
				sprintf_s(strVal,"'%s'发现原始名相同的记录,是否覆盖原记录",strfilename);
				int ret=MessageBox(strVal,"MonsterEditor",MB_YESNO);
				//覆盖原记录
				if(ret==IDYES)
				{
					vecMerMon.erase(iter);
					vecMerMon.push_back(*monster);					
					return true;
				}
				//保留原记录
				else
					return true;
			}	
		
	}//for
	//没有找到相同的记录
	return false;

}
//合并属性文件
bool CMerger::MergerFile(CString strfilename)
{
	setlocale(LC_ALL,"Chinese-simplified");//设置中文环境
	int i=0;
	char strVal[100];
	tagMonster monster;
	fstream file;
	file.open(strfilename);
	if(!file.is_open())
	{
		MessageBox("文件无法打开");
		return false;
	}

	//读取文件
	while((ReadTot(file,"*"))!=0)
	{	
		file>> monster.dwIndex;
		itoa(monster.dwIndex,strVal,10);
		//判断打开的文件是否是怪物属性文件，防止打开文件出错
		for(;strVal[i]!='\0';i++)
		{
			if(strVal[i]<'0'||strVal[i]>'9')
			{
				vecMerMon.clear();
				MessageBox("请确认三份文件中的记录全是怪物属性");
				return false;
			}
			
		}

		file>>monster.strOrigName
			>>monster.strName;
		//判断文件打开是否正确
		if(monster.strName==">"||monster.strName=="#"||monster.strName=="@")
		{
			vecMerMon.clear();
			MessageBox("请确认三份文件中的记录全是怪物属性");
			return false;
		}
		file>>monster.strBlock
			>>monster.strSuper
			>>monster.strCollection
			>>monster.strTitle
			>>monster.dwTitleColor
			>>monster.dwTitlePicture
			>>monster.strCountryID
			>>monster.strNpcSort
			>>monster.dwPicID
			>>monster.dwPicLevel
			>>monster.dwNameColorID
			>>monster.dwLevelColorID
			>>monster.dwHpColor
			>>monster.dwSoundID
			>>monster.strTame
			>>monster.dwTameTimes
			>>monster.strShape
			>>monster.nLevel
			>>monster.dwExp
			>>monster.dwWorkPoint
			>>monster.dwMaxHp
			>>monster.wMinAttack
			>>monster.wMaxAttack
			>>monster.wMinMagicAttack
			>>monster.wMaxMagicAttack
			>>monster.wHitRating
			>>monster.wDefense
			>>monster.wMagicDefense
			>>monster.wBlock
			>>monster.wParry
			>>monster.wDodge
			>>monster.fMoveSpd
			>>monster.fBatMoveSpd
			>>monster.wWillAnti
			>>monster.wGroundAnti
			>>monster.wWaterAnti
			>>monster.wFireAnti
			>>monster.wWindAnti
			>>monster.wSacreAnti
			>>monster.wDarkAnti
			>>monster.wNotElemAnti
			>>monster.wHpRecoverSpd
			>>monster.wMpRecoverSpd
			>>monster.wBodyAnti
			>>monster.wBloodAnti
			>>monster.wDuskAnti
			>>monster.wNumbAnti
			>>monster.wSleepAnti
			>>monster.wDrugAnti
			>>monster.wNotMoveAnti
			>>monster.wMinStayTimes
			>>monster.wMaxStayTimes
			>>monster.wMinWalkTimes
			>>monster.wMaxWalkTimes
			>>monster.wAlertRange
			>>monster.wPurRange
			>>monster.wPeaMoveRange
			>>monster.strAIModel
			>>monster.bTalk
			>>monster.strMonRace
			>>monster.strElem
			>>monster.wStayTime
			>>monster.strAIWay
			>>monster.wHurtHuman
			>>monster.wHurtElem
			>>monster.wHurtDiff
			>>monster.wHurtAnimal
			>>monster.wHurtDead
			>>monster.wHurtDevil
			>>monster.wHurtInsect
			>>monster.wHurtFire
			>>monster.wHurtWater
			>>monster.wHurtGround
			>>monster.wHurtWind
			>>monster.wHurtSacre
			>>monster.wHurtDark
			>>monster.wHurtNotElem
			>>monster.wVitality
			>>monster.wStormHit
			>>monster.wStormHurt
			>>monster.wStormAnti
			>>monster.wMagicStorm
			>>monster.wMagicStormHurt  
			>>monster.wMagicStormAnti
			>>monster.fInjurRate
			>>monster.wAlertTime
			>>monster.fEscape
			>>monster.wSkillRate
			>>monster.wSkillInter
			>>monster.strReturn
			>>monster.strAllRecover
			>>monster.wReDistance
			>>monster.wReTime
			>>monster.strRefe1
			>>monster.strRefe2
			>>monster.strRefe3
			>>monster.strRefe4
			>>monster.strRefe5
			>>monster.strBirthSct
			>>monster.strBattleSct
			>>monster.strInjurSct
			>>monster.strDeadSct
			>>monster.strCycleSct
			>>monster.strChoseSkillSct
			>>monster.strFindTgtSct
			>>monster.strKillTgtSct;

			//判断索引是否重复
		if(!IsSame(strfilename,&monster))
		{
			vecMerMon.push_back(monster);
			continue;
		}
	
	}
	return true;
}
//判断技能文件中是否有相同记录
bool CMerger::IsSameSkill(CString strfilename,tagMonster *monster)
{
	vector<tagMonster>::iterator iter;
	//判断原始名是否重复
	for(iter=vecMerMon.begin();iter!=vecMerMon.end();iter++)
	{
		//有重复的记录
		if(iter->strOrigName.c_str()==(*monster).strOrigName)
		{
			char strVal[100];
			sprintf_s(strVal,"'%s'发现原始名相同的记录,是否覆盖原记录",strfilename);
			int ret=MessageBox(strVal,"MonsterEditor",MB_YESNO);
			//覆盖记录
			if(ret==IDYES)
			{
				//先删除原记录
				vecMerMon.erase(iter);
				//插入新记录
				vecMerMon.push_back(*monster);
				//清空技能容器，防止重复
				(*monster).vecSkill.clear();
				return true;
			}
			else
				return true;
		}
	}//for
	return false;
}
bool CMerger::MergerFileSkill(CString strfilename)
{
	fstream fileskill(strfilename);
	//如果文件没有打开，则返回
	if(!fileskill.is_open())
		return false;
	//如果不为0则，有相应的技能记录
	string strflag;
	int i=0;
	tagMonster monster;
	tagSkill skill;
	ReadTot(fileskill,"*");
	while(!fileskill.eof())
	{
		fileskill>>monster.strOrigName
			>>strflag;
		
		//判断打开的文件是否是怪物技能文件，防止打开文件出错
		if(strflag!=">"&&strflag!="#"&&strflag!="@")
		{
			vecMerMon.clear();
			MessageBox("请确认三份文件中的记录全是怪物技能记录");
			return false;
		}

		
		//有三种技能，“>”、“#”、“@”代表不同种类的技能
		while(strflag==">"||strflag=="#"||strflag=="@")		
		{
			//">"在文件中是表示基本技能
			if(strflag==">")
			{
				strflag="";
				skill.strSkillType=">";
				fileskill>>skill.dwSkillID
					>>skill.wSkillLevel
					>>strflag;
				//基本技能没有技能间隔，设置为0
				skill.wInterTime=0;
				monster.vecSkill.push_back(skill);

			}
			//"#"在文件中表示普通技能
			else if(strflag=="#")
			{
				strflag="";
				skill.strSkillType="#";
				fileskill>>skill.dwSkillID
					>>skill.wSkillLevel
					>>strflag;
				//普通技能没有技能间隔，设置为0
				skill.wInterTime=0;
				monster.vecSkill.push_back(skill);
			}
			//"@"在文件中表示周期技能
			else if(strflag=="@")
			{
				strflag="";
				skill.strSkillType="@";
				fileskill>>skill.dwSkillID
					>>skill.wSkillLevel
					>>skill.wInterTime
					>>strflag;
				//将技能skill插入到vecSkill中
				monster.vecSkill.push_back(skill);
			}
			//"*"表示下一个记录的开始，所以返回
			else if(strflag=="*")
			{
				break;
			}
		}
		int n=(int)vecMerMon.size();//记录条数
		//第一个元素直接插入到vector中
		if(n<1)
		{
			vecMerMon.push_back(monster);
			monster.vecSkill.clear();
			continue;
		}
		//判断是否有原始名相同的记录
		if(!IsSameSkill(strfilename,&monster))
		{
			vecMerMon.push_back(monster);
			monster.vecSkill.clear();
			continue;	
		}
		
	}//while
	return true;
}