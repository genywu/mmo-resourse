// Recover.cpp : 实现文件
//

#include "stdafx.h"
#include "MonsterEditor.h"
#include "Recover.h"
#include "MonsterEditorDlg.h"

// CRecover 对话框
string FilePathName,FilePathNameSkill;
IMPLEMENT_DYNAMIC(CRecover, CDialog)

CRecover::CRecover(CWnd* pParent /*=NULL*/)
	: CDialog(CRecover::IDD, pParent)
{

}

CRecover::~CRecover()
{
}

void CRecover::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRecover, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CRecover::OnOpenMonsterFile)
	ON_BN_CLICKED(IDC_BUTTON2, &CRecover::OnOpenMonsterSkillFile)
	ON_BN_CLICKED(IDC_BUTTON3, &CRecover::OnRecover)
END_MESSAGE_MAP()


// CRecover 消息处理程序
//将光标移动到“*”之后
int RecoverReadTo(fstream &stream,string name)
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

void CRecover::OnOpenMonsterFile()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	//打开OPEN公用对话框
	if(dlg.DoModal()==IDOK)
	//获取文件路径
	FilePathName=dlg.GetPathName();
	//将文件路径显示在EDIT中
	SetDlgItemText(IDC_EDIT1,FilePathName.c_str());
}

void CRecover::OnOpenMonsterSkillFile()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	//打开OPEN公用对话框
	if(dlg.DoModal()==IDOK)
	//获取文件路径
	FilePathNameSkill=dlg.GetPathName();
	//将文件路径显示在EDIT中
	SetDlgItemText(IDC_EDIT2,FilePathNameSkill.c_str());
}

void CRecover::OnRecover()
{
	// TODO: 在此添加控件通知处理程序代码
	if(FilePathName==""||FilePathNameSkill=="")
	{
		MessageBox("请先选择文件");
		return;
	}
	fstream file;
	string strflag;
	setlocale(LC_ALL,"Chinese-simplified");//设置中文环境
	file.open(FilePathName.c_str());
	int i=0;
	RecoverReadTo(file,"*");
	file>>strflag;
	//判断选择的文件是否正确
	for(;strflag[i]!='\0';i++)
		{
			//如果不正确
			if(strflag[i]<'0'||strflag[i]>'9')
			{
				MessageBox("请确认三份文件中的记录全是怪物属性记录");
				return;
			}
			
		}
	file.close();
	file.open(FilePathNameSkill.c_str());
	//移动到第三个字符
	RecoverReadTo(file,"*");
	file>>strflag;
	file>>strflag;
	//不是这三个符号中的任何一个，则不是怪物技能文件
	if(strflag!=">"&&strflag!="#"&&strflag!="@")
		{
			MessageBox("请确认三份文件中的记录全是怪物技能记录");
			return;
		}
	file.close();
	CMonsterEditorDlg recdlg;
	recdlg.RecoverFile(FilePathName,FilePathNameSkill);
}
