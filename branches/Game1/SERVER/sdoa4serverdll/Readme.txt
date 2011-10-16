osapdll文件及目录说明：
osapdll
      --sdoa4server.ini			文件 提供了配置文件的样例，需要修改相应的IP和SDK日志路径
      
      --include				目录 存放sdoa4server所需头文件
	--sndabase.h			文件 定义了一些常用的的宏用于统一函数定义
	--sdoa4server.h 		文件 sdk的头文件，里边包含认证、和计费的相关接口定义
      
      --windows				目录 包含windows中sdk、com组件
	--COM				目录 存放com组建
	  --SDOA4ServerCOM.dll  	文件 com组件
	
	--DLL				目录 存放那个dll文件和相应的lib文件
	  --SDOA4ServerDLL.dll		文件 release版动态库
	  --SDOA4ServerDLL.lib  	文件 release版动态库静态链接库
	  --SDOA4ServerDLLd.dll 	文件 debug版动态库
	  --SDOA4ServerDLLd.lib 	文件 debug版动态库静态链接库
     
     --linux				目录提供了linux下gcc3.23编译的.so和.a文件
	
	--GCC3				目录 gcc3.x编译的.so和.a
	  --libSDOA4Server.so   	文件 动态库
	  --libSDOA4Serverlib.a 	文件 静态库
	
	--GCC4				目录 gcc4.x编译的.so和.a
	  --libSDOA4Server.so   	文件 动态库
	  --libSDOA4Serverlib.a 	文件 静态库
	
	--JAVA				目录 jar包
	  --
	
	--PHP				目录 php扩展
	  --


