// SprConvert.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FileEnumerate.h"
#include "SprConvert.h"

#define MAX_TEXTURE_SIZE		256				// 最大贴图尺寸
#define MIN_TEXTURE_SIZE		16				// 最小贴图尺寸
#define MIN_TEXTUREGROUP_SIZE	512				// 分组贴图的最大尺寸

int nTotleFileConvert=0;
int nTotleFileNotConvert=0;
float nTotleMemUseBefore = 0;
float nTotleMemUseNow = 0;
int	MaxSize = 0;
ofstream ofs;

int FitTextureSize(int nSize)
{
	if(nSize < 0 || nSize > MAX_TEXTURE_SIZE)
		return 0;

	for(int i=MIN_TEXTURE_SIZE; i<=MAX_TEXTURE_SIZE; i*= 2)
	{
		if(nSize <= i)
			return i;
	}
	return 0;
}

SprConvert::SprConvert()
{
	m_pHeader = NULL;
	m_pFileData = NULL;
	m_pOffset = NULL;
	m_pSprite = NULL;
	m_bReadOK = false;
	m_nTotleTexNum = 0;
}

SprConvert::~SprConvert()
{
	SAFE_DELETE_ARRAY(m_pFileData);
}

void SprConvert::ReadFile(char *szSprFile)
{
	KPakFile	File;
	PBYTE		pTemp;

	m_bReadOK = false;
//	cout<<szSprFile<<endl;

	// open the file
	if (!File.Open(szSprFile))
	{
//		cout<<"Can't open file!"<<endl;
		return;
	}

	m_pFileData = pTemp = new BYTE[File.Size()];
	if (!pTemp)
	{
//		cout<<"Not enough memory!"<<endl;
		return;
	}

	// read data from file
	File.Read(pTemp, File.Size());

	// check file header setup sprite member
	m_pHeader = (SPRHEAD*)pTemp;
	if (!g_MemComp(m_pHeader->Comment, "SPR", 3))
	{
//		cout<<"Not a spr file!"<<endl;
		SAFE_DELETE_ARRAY(m_pFileData);
		return;
	}

	// setup palette pointer
	pTemp += sizeof(SPRHEAD);


	// setup offset pointer
	pTemp += m_pHeader->Colors * sizeof(KPAL24);
	m_pOffset = (SPROFFS*)pTemp;

	// setup sprite pointer
	pTemp += m_pHeader->Frames * sizeof(SPROFFS);
	m_pSprite = (LPBYTE)pTemp; // 相对偏移

	if(m_pHeader->Frames <= 0)
	{
		SAFE_DELETE_ARRAY(m_pFileData);
		return;
	}

	bool bOutput = false;
	int i,j=0;
	for(i=0; i<m_pHeader->Frames; i++)
	{
		SPRFRAME* pFrame = (SPRFRAME*)(m_pSprite + m_pOffset->Offset);
		if(pFrame->Width > MaxSize || pFrame->Height > MaxSize)
		{
			if(!bOutput)
			{
				bOutput = true;
				cout<<szSprFile<<endl;
				ofs<<szSprFile<<endl;
			}
			if(j == m_pHeader->Frames-1)
			{
				cout<<"*"<<j<<" ";
				ofs<<"*"<<j<<" ";
			}
			else
			{
				cout<<j<<" ";
				ofs<<j<<" ";
			}
		}
		m_pOffset++;
		j++;
	}
	if(bOutput)
	{
		cout<<endl;
		ofs<<endl;
	}

	m_bReadOK = true;
}

int SprConvert::DoGroup(int nFrameNum, int nFrameWidth, int nFrameHeight, TextureSize *pTexSize)
{
	// 贴图分组
	int nTexNum = 0;
	int nFrameLeft = nFrameNum;
	while(nFrameLeft)
	{
		bool bNewTex = false;
		if(nTexNum == 0 || (pTexSize[nTexNum-1].nWidth == MIN_TEXTUREGROUP_SIZE && pTexSize[nTexNum-1].nHeight == MIN_TEXTUREGROUP_SIZE))
			bNewTex = true;
		if(nTexNum != 0 && nFrameLeft <= (pTexSize[nTexNum-1].nFrameNum *3 / 4))
			bNewTex = true;

		if(bNewTex)
		{
			// 开一张新的贴图
			nTexNum++;
			if(nFrameLeft >= 2)
			{
				pTexSize[nTexNum-1].nWidth = FitTextureSize(nFrameWidth) * 2;
				pTexSize[nTexNum-1].nHeight = FitTextureSize(nFrameHeight);
			}
			else
			{
				pTexSize[nTexNum-1].nWidth = FitTextureSize(nFrameWidth);
				pTexSize[nTexNum-1].nHeight = FitTextureSize(nFrameHeight);
			}

			pTexSize[nTexNum-1].nFrameWidth = nFrameWidth;
			pTexSize[nTexNum-1].nFrameHeight = nFrameHeight;

			pTexSize[nTexNum-1].nFrameNum = (pTexSize[nTexNum-1].nWidth / nFrameWidth) * (pTexSize[nTexNum-1].nHeight / nFrameHeight);
			if(pTexSize[nTexNum-1].nFrameNum <= nFrameLeft)
			{
				pTexSize[nTexNum-1].nRealFrameNum = pTexSize[nTexNum-1].nFrameNum;
				nFrameLeft -= pTexSize[nTexNum-1].nFrameNum;
			}
			else
			{
				pTexSize[nTexNum-1].nRealFrameNum = nFrameLeft;
				nFrameLeft = 0;
			}
		}
		else
		{
			int n1=0, n2=0;
			// 扩展贴图尺寸
			if(pTexSize[nTexNum-1].nWidth < MIN_TEXTUREGROUP_SIZE)
				n1 = (pTexSize[nTexNum-1].nWidth*2 / nFrameWidth) * (pTexSize[nTexNum-1].nHeight / nFrameHeight);
			if(pTexSize[nTexNum-1].nHeight < MIN_TEXTUREGROUP_SIZE)
				n2 = (pTexSize[nTexNum-1].nWidth / nFrameWidth) * (pTexSize[nTexNum-1].nHeight*2 / nFrameHeight);

			if(n1 >= n2)
				pTexSize[nTexNum-1].nWidth *= 2;
			else
				pTexSize[nTexNum-1].nHeight *= 2;

			// 计算新尺寸能放下的帧数和剩余帧数
			int nFrameNumPre = pTexSize[nTexNum-1].nFrameNum;
			pTexSize[nTexNum-1].nFrameNum = (pTexSize[nTexNum-1].nWidth / nFrameWidth) * (pTexSize[nTexNum-1].nHeight / nFrameHeight);

			if(pTexSize[nTexNum-1].nFrameNum - nFrameNumPre <= nFrameLeft)
			{
				pTexSize[nTexNum-1].nRealFrameNum = pTexSize[nTexNum-1].nFrameNum;
				nFrameLeft -= pTexSize[nTexNum-1].nFrameNum - nFrameNumPre;
			}
			else
			{
				pTexSize[nTexNum-1].nRealFrameNum = nFrameNumPre + nFrameLeft;
				nFrameLeft = 0;
			}
		}
	}

	return nTexNum;
}

bool SprConvert::Output()
{
	if(!m_bReadOK)
		return false;

/*	int i,j;

	int nMaxGrpWidth[8];
	int nMaxGrpHeight[8];
	int nMinGrpWidth[8];
	int nMinGrpHeight[8];
	int nMaxWidth = 0;
	int nMaxHeight = 0;
	int nTexMemSize = 0;
	for(i=0; i<8; i++)
	{
		nMaxGrpWidth[i] = 0;
		nMaxGrpHeight[i] = 0;
		nMinGrpWidth[i] = 10000;
		nMinGrpHeight[i] = 10000;
		for(j=0; j<m_pHeader->Frames/8; j++)
		{
			SPRFRAME* pFrame = (SPRFRAME*)(m_pSprite + m_pOffset->Offset);
			if(pFrame->Width > nMaxGrpWidth[i])
				nMaxGrpWidth[i] = pFrame->Width;
			if(pFrame->Height > nMaxGrpHeight[i])
				nMaxGrpHeight[i] = pFrame->Height;

			if(pFrame->Width < nMinGrpWidth[i])
				nMinGrpWidth[i] = pFrame->Width;
			if(pFrame->Height < nMinGrpHeight[i])
				nMinGrpHeight[i] = pFrame->Height;

			if(pFrame->Width > nMaxWidth)
				nMaxWidth = pFrame->Width;
			if(pFrame->Height > nMaxHeight)
				nMaxHeight = pFrame->Height;
			int nTexWidth = FitTextureSize(pFrame->Width);
			int nTexHeight = FitTextureSize(pFrame->Height);
			nTexMemSize += nTexWidth * nTexHeight * 2;
			m_pOffset++;
		}
//		cout<<"Direction"<<i<<" : "<<"MaxWidth "<<nMaxGrpWidth[i]<<" MaxHeight "<<nMaxGrpHeight[i];
//		cout<<" MinWidth "<<nMinGrpWidth[i]<<" MinHeight "<<nMinGrpHeight[i]<<endl;
	}
	if(nMaxWidth > MAX_TEXTURE_SIZE || nMaxHeight > MAX_TEXTURE_SIZE)
	{
//		cout<<"MaxFrameSize is too large!"<<endl;
		SAFE_DELETE_ARRAY(m_pFileData);
		return false;
	}

	//cout<<"Totle : "<<"MaxWidth "<<nMaxWidth <<" MaxHeight "<<nMaxHeight<<endl;
//	cout<<endl;

	TextureSize *pTexArray = m_TexSize;
	m_nTotleTexNum = 0;
	int nGroupTexMemSize = 0;
	for(i=0; i<8; i++)
	{
		int nTexNum = DoGroup(m_pHeader->Frames/8, nMaxGrpWidth[i], nMaxGrpHeight[i], pTexArray);
		m_nTotleTexNum += nTexNum;
		for(int j=0; j<nTexNum; j++)
		{
//			cout<<"TexWidth "<<m_TexSize[j].nWidth<<" TexHeight "<<m_TexSize[j].nHeight<<endl;
			nGroupTexMemSize += m_TexSize[j].nWidth * m_TexSize[j].nHeight * 2;
		}
		pTexArray += nTexNum;
	}
//	cout<<"Texture number : "<<m_nTotleTexNum<<endl;
//	cout<<endl;

//	cout<<"Texture memory size       : "<<nTexMemSize<<endl;
//	cout<<"Group Texture memory size : "<<nGroupTexMemSize<<endl;
//	cout<<endl;

//	cout<<"Totle Frames : "<<m_pHeader->Frames<<endl;
//	cout<<"Frames in one Direction : "<<m_pHeader->Frames/8<<endl;

	if(nGroupTexMemSize > nTexMemSize)
	{
		nTotleFileNotConvert++;
		return false;
	}

	nTotleMemUseBefore += nTexMemSize;
	nTotleMemUseNow += nGroupTexMemSize;
	if(nTotleMemUseBefore < nTotleMemUseNow)
		i++;
*/
	return true;
}

void SprConvert::WriteFile(char *szSprFile)
{
	int i, j;
	if(!m_bReadOK)
		return;

	char* p = strrchr(szSprFile, '.');
	if(!p)
		return;

	*(p+1) = 0;
	strcat(szSprFile, "sti");
	ofstream ofs(szSprFile, ios::binary);

	SprTexInfoHead head;
	strcpy((char*)head.Comment,"STI");
	head.nFrameNum = m_pHeader->Frames;
	head.nTexNum = m_nTotleTexNum;
	ofs.write((const char *)&head, sizeof(SprTexInfoHead));

	for(i=0; i<m_nTotleTexNum; i++)
	{
		TexSize tex;
		tex.nWidth = m_TexSize[i].nWidth;
		tex.nHeight = m_TexSize[i].nHeight;
		ofs.write((const char *)&tex, sizeof(TexSize));
	}

	int nFrameCount = 0;
	for(i=0; i<m_nTotleTexNum; i++)
	{
		for(j=0; j<m_TexSize[i].nRealFrameNum; j++)
		{
			int nFramePerLine = m_TexSize[i].nWidth / m_TexSize[i].nFrameWidth;

			FrameTexInfo fti;
			fti.nTexIndex = i;
			fti.nTexX = (j % nFramePerLine) * m_TexSize[i].nFrameWidth;
			fti.nTexY = (j / nFramePerLine) * m_TexSize[i].nFrameHeight;
			ofs.write((const char *)&fti, sizeof(FrameTexInfo));
			nFrameCount++;
		}
	}
	assert(nFrameCount == m_pHeader->Frames);
}

void ConvertOneSpr(char *SzSprFile)
{
	SprConvert spr;
	spr.ReadFile(SzSprFile);
	spr.Output();
//	if(spr.Output())
//	{
//		spr.WriteFile(SzSprFile);
//		nTotleFileConvert++;
//	}
}

void main(int argc, char* argv[])
{
	if(argc != 3)
		return;
	MaxSize = atoi(argv[2]);
	if(MaxSize<=0)
		return;
	ofs.open("ttt.txt");
	DirectorySearch(argv[1], ConvertOneSpr);

//	cout<<endl;
//	cout<<"TotleFileConvert     : "<<nTotleFileConvert<<endl;
//	cout<<"TotleFileNotConvert  : "<<nTotleFileNotConvert<<endl;
//	cout<<endl;
//	cout<<"TotleMemUseBefore : "<<nTotleMemUseBefore<<endl;
//	cout<<"TotleMemUseNow    : "<<nTotleMemUseNow<<endl;
}

