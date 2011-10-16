class CRoleModel
{
	int			    m_iCurrentSelActionIndex;		//当前选择的动作索引
	string			m_sFilePathName;				//模型文件路径名
	CDisplayModel * m_pDisplayModel;				//角色的显示模型

public:
	CRoleModel();
	~CRoleModel();

	int				GetCurrentSelActionIndex()						{ return m_iCurrentSelActionIndex; }
	string		  & GetFilePathName()								{ return m_sFilePathName; }
	CDisplayModel * GetDisplayModel()								{ return m_pDisplayModel; }
	void			SetCurrentSelActionIndex(int iActionIndex)		{ m_iCurrentSelActionIndex = iActionIndex; }
	void			SetFilePathName(string & sPathName)				{ m_sFilePathName = sPathName; }
	void			SetDisplayModel(CDisplayModel * pDisplayModel)	{ m_pDisplayModel = pDisplayModel; }
};