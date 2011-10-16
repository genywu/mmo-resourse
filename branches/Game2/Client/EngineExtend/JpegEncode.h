#pragma once

namespace jpeg
{
	class JpegEncode
	{

	public:
		FILE  *m_pOutFile;

		int    buffWidth;
		int	   buffHeight;

		int    m_iWidth; //原始位图宽度
		int    m_iHeight;//原始位图高度

		struct tagRGB
		{
			BYTE r,g,b;
		};

        tagRGB *m_pData;//原始位图数据 rgb

		void CreateBitmap(int iWidth,int iHeight);
		void SetPixel(int iX,int iY,DWORD dwColor);
		void DestroyBitmap();

		void Write(const char *pszFileName, int Q);

		JpegEncode(void);
		~JpegEncode(void);

	};
}