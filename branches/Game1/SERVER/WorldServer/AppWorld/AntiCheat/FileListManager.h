///
/// @file FileNameListManager.h
/// @author Kevin Lynx
/// @brief to manage some file buffer lists in AntiCheat module.
///
#ifndef ___FILE_LIST_MANAGER_H_
#define ___FILE_LIST_MANAGER_H_

#include <vector>
#include <string>

namespace AntiCheat
{
	// set record file name flag
#define RECORD_NAME

	class CFileListManager
	{
	public:
		struct Buffer
		{
			void *buf;
			size_t size;
#ifdef RECORD_NAME
			std::string name;
#endif
		};
		typedef std::vector<Buffer> FileBufferList;
	public:
		/// 装载所有图片资源并建立列表
		bool Init();

		/// 清除所有文件列表
		void Release();

		/// 重读，先清除再获取
		bool ReInit();

		/// 随机获取背景图文件
		const Buffer &GetRandBack() const;

		/// 随机获取小图文件
		const Buffer &GetRandSmall() const;

		/// 随机获取‘X’图片文件
		const Buffer &GetRandX() const;

		/// 获取‘A’‘B’‘C’‘D’符号文件名
		/// sel : 1-A, 2-B, 3-C, 4-D
		const Buffer &GetSelSign( int sel ) const;

		/// 获取ALPHA通道图
		const Buffer &GetAlpha() const;

	private:
		/// 装载一个文件并保存进指定的容器
		bool LoadFile( const std::string &file, FileBufferList &file_list );

		/// 装载一批文件
		bool LoadFileList( const std::list<std::string> &file_name_list, FileBufferList &file_list );

		/// 清除一个文件列表
		void ReleaseFileList( FileBufferList &file_list );

	private:
		FileBufferList m_BackList;
		FileBufferList m_SmallList;
		FileBufferList m_XList;
		FileBufferList m_SelList;
		FileBufferList m_AlphaList;
	};

}

#endif