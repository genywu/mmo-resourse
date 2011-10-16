#include "StdAfx.h"
#include "changebmp.h"

class CGetBitmaps : public CBitmapButton
{
	CBitmapButton *btn;
public:
	CGetBitmaps(CBitmapButton *button)
	{
		btn=button;
	}
	inline CBitmap * Nor(){ //normal image (REQUIRED)
		return (CBitmap *)(PCHAR(btn)+(ULONG)(PCHAR (&m_bitmap)-PCHAR(this)));//not PTCHAR, butPCHAR
	}
	inline CBitmap * Sel(){ // selected image (OPTIONAL)
		return (CBitmap *)(PCHAR(btn)+(ULONG)(PCHAR (&m_bitmapSel)-PCHAR(this)));//not PTCHAR, butPCHAR
	}
	inline CBitmap * Foc(){ // focused but not selected (OPTIONAL)
		return (CBitmap *)(PCHAR(btn)+(ULONG)(PCHAR (&m_bitmapFocus)-PCHAR(this)));//not PTCHAR, butPCHAR
	}
	inline CBitmap * Dis(){ // disabled bitmap (OPTIONAL)
		return (CBitmap *)(PCHAR(btn)+(ULONG)(PCHAR (&m_bitmapDisabled)-PCHAR(this)));//not PTCHAR, butPCHAR
	}

};

BOOL ChangeBitmapBtnImages(CBitmapButton &button,LPCTSTR lpszFilename)// 
{

	CDC   srcDC;   
	srcDC.CreateCompatibleDC(NULL); 

	CDC   memDC;     
	memDC.CreateCompatibleDC(NULL);   

	CBitmap  src;

	HBITMAP hbm = (HBITMAP) ::LoadImage (NULL, lpszFilename, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	if (hbm == NULL)
	{
		return FALSE;
	}
	src.Attach(hbm);

	//得到原指针，
	CBitmap*  pOldBitmap1   =   srcDC.SelectObject(&src);
	srcDC.SelectObject(&pOldBitmap1);  

	//CBitmap* pOldBitmap2   =   memDC.SelectObject(&src); //ERROR ?
	//memDC.SelectObject(pOldBitmap2); //save pOldBitmap2!!


	BITMAP   bmpinfo;   
	src.GetBitmap(&bmpinfo);   
	int   bmpWidth   =   bmpinfo.bmWidth ;// / 4;//!!attention!!
	int   bmpHeight   =   bmpinfo.bmHeight;   

	CGetBitmaps gbitmap(&button);//class which  we defined
	CBitmap * pbitmap[4];
	pbitmap[0]=gbitmap.Nor();
	pbitmap[1]=gbitmap.Sel();
	pbitmap[2]=gbitmap.Foc();
	pbitmap[3]=gbitmap.Dis();

	pbitmap[0]->DeleteObject();
	pbitmap[1]->DeleteObject();
	pbitmap[2]->DeleteObject();
	pbitmap[3]->DeleteObject();

	BOOL Rz=TRUE;

	for(int i=0;i<4;i++)
	{
		pbitmap[i]->CreateCompatibleBitmap(&srcDC,bmpWidth,bmpHeight);   
		memDC.SelectObject(pbitmap[i]);   
		if(   !memDC.BitBlt(0,0,bmpWidth, bmpHeight, &srcDC,bmpWidth*i,0,SRCCOPY)  ) 
		{
			Rz=FALSE;
			break;
		}
	}

	srcDC.SelectObject(pOldBitmap1);   
	//memDC.SelectObject(pOldBitmap2);   

	srcDC.DeleteDC();   
	memDC.DeleteDC(); 

	src.DeleteObject();
	return Rz;
}


BOOL SetBitmapBtnImage(CBitmapButton &button,LPCTSTR lpszFilenameNor,int iState)
{
	CDC   srcDC;   
	srcDC.CreateCompatibleDC(NULL); 

	CDC   memDC;     
	memDC.CreateCompatibleDC(NULL);   

	CBitmap  src;

	HBITMAP hbm = (HBITMAP) ::LoadImage (NULL, lpszFilenameNor, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	if (hbm == NULL)
	{
		return FALSE;
	}
	src.Attach(hbm);

	//得到原指针，
	CBitmap*  pOldBitmap1   =   srcDC.SelectObject(&src);
	srcDC.SelectObject(&pOldBitmap1);  

	//CBitmap* pOldBitmap2   =   memDC.SelectObject(&src); //ERROR ?
	//memDC.SelectObject(pOldBitmap2); //save pOldBitmap2!!


	BITMAP   bmpinfo;   
	src.GetBitmap(&bmpinfo);   
	int   bmpWidth   =   bmpinfo.bmWidth ;// / 4;//!!attention!!
	int   bmpHeight   =   bmpinfo.bmHeight;   

	CGetBitmaps gbitmap(&button);//class which  we defined
	CBitmap * pbitmap = NULL;
	switch(iState)
	{
	case 0:
		{
		 pbitmap=gbitmap.Nor();
		}
		break;
	case 1:
		{
		 pbitmap=gbitmap.Sel();
		}
		break;
	case 2:
		{
		 pbitmap=gbitmap.Foc();
		}
		break;
	case 3:
		{
         pbitmap=gbitmap.Dis();
		}
		break;
	}
	pbitmap->DeleteObject();

	BOOL Rz=TRUE;

	{
		pbitmap->CreateCompatibleBitmap(&srcDC,bmpWidth,bmpHeight);   
		memDC.SelectObject(pbitmap);   
		if(   !memDC.BitBlt(0,0,bmpWidth, bmpHeight, &srcDC,0,0,SRCCOPY)  ) 
		{
			Rz=FALSE;
		}
	}

	srcDC.SelectObject(pOldBitmap1);     

	srcDC.DeleteDC();   
	memDC.DeleteDC(); 

	src.DeleteObject();
	return Rz;
}