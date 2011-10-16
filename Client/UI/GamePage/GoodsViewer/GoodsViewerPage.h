#pragma  once
#include "../../GamePage.h"

class GVEvent;
class GoodsViewerPage: public GamePage
{
public:
	GoodsViewerPage();
	~GoodsViewerPage();
	/////////////////////////////////////////////////
	// zhaohang  2010/3/15 
	//Override function
	void LoadPage();

	bool Open();

	bool Close();

	void ReleasePage();
	/////////////////////////////////////////////////
	
protected:
private:
	GVEvent* m_GVEvent;

};