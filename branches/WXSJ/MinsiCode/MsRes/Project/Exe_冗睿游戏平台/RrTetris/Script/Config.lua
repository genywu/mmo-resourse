g_GameHeartbeatTime = 
{
1500, --游戏速度1级
1200,  --2级
1000,  --3级
900,  --4级
800,  --5级
700,  --6级
600,  --7级
500,  --8级
300,  --9级
200   --游戏速度10级
}
g_GameTollGateSize = 
{
0x000A0010,	--游戏关卡1级场景尺寸(十六进制前四位表示宽，后四位表示高)
0x00090011,	--2级
0x00070014,	--3级
0x00050015,	--4级
0x00100010,	--5级
0x0012000E,	--6级
0x0014000C,	--7级
0x00160010,	--8级
0x000F000F,	--9级
0x000A000A,	--游戏关卡10级场景尺寸
0x00190015	--游戏关卡通关场景尺寸
}

-- 获取背景音乐个数
function Sound_GetBackMusicCount( )
    return 0x07;
end

-- 获取背景音乐文件路径
function Sound_GetBackMusic( Index )
	if(0 == Index) then
		return "RrTetris\\music\\BackMusic01.mid";
	elseif(1 == Index) then
		return "RrTetris\\music\\BackMusic02.mid";
	elseif(2 == Index) then
		return "RrTetris\\music\\BackMusic03.mid";
	elseif(3 == Index) then
		return "RrTetris\\music\\BackMusic04.mid";
	elseif(4 == Index) then
		return "RrTetris\\music\\BackMusic05.mid";
	elseif(5 == Index) then
		return "RrTetris\\music\\BackMusic06.mid";
	elseif(6 == Index) then
		return "RrTetris\\music\\BackMusic07.mid";
	else
		return "RetError";
	end
end

-- 获取固定方块音乐文件路径
function Sound_GetFixationSound(  )
	return "RrTetris\\music\\固定.wav"
end

-- 获取爆炸音乐文件路径
function Sound_GetBlastSound(  )
	return "RrTetris\\music\\爆炸.wav"
end

-- 获取失败音乐文件路径
function Sound_GetDeadSound(  )
	return "RrTetris\\music\\挂了.wav"
end

-- 获取开始音乐文件路径
function Sound_GetReadyGoSound(  )
	return "RrTetris\\music\\准备开始.wav"
end

-- 获取旋转方块音乐文件路径
function Sound_GetDistortioSound(  )
	return "RrTetris\\music\\旋转.wav"
end

function Sound_Gethurray01Sound(  )
	return "RrTetris\\music\\Combo.ogg"
end

-- 获取二级欢呼音效文件路径
function Sound_Gethurray02Sound(  )
	return "RrTetris\\music\\Great.ogg"
end

-- 获取三级欢呼音效文件路径
function Sound_Gethurray03Sound(  )
	return "RrTetris\\music\\MaxCombo.ogg"
end

-- 获取游戏升级音效文件路径
function Sound_GetLevelUpSound(  )
	return "RrTetris\\music\\LevelUp.ogg"
end

-- 获取游戏升级完毕音效文件路径
function Sound_GetLevelCompleteSound(  )
	return "RrTetris\\music\\LevelComplete.ogg"
end

-- 获取不同游戏级别的心跳间隔
function GetHeartbeatTime( Index )
	if(Index > 10) then
		return "RetError";
	else
		return g_GameHeartbeatTime[Index];
	end
end

-- 获取不同游戏关卡级别的场景大小
function GetTollGateSize( Index )
	if(Index > g_GameTollGateSize[1]) then
		return "RetError";
	else
		return g_GameTollGateSize[Index];
	end
end

-- 获取每关升级所需积分
function GetEachLevelNeedIntegral(  )
	return 500;
end

-- 获取服务器IP地址与端口
function GetServerIPAndPort( Index )
	if(1 == Index) then
		return "192.168.111.111";		-- 指定服务器的IP地址
	elseif(2 == Index) then
		return 9999;								-- 指定服务器的端口
	elseif(3 == Index) then
	  return 1;										-- 指定是 "服务器 = 1" 还是 "客户端 = 0"
	else
		return "RetError";
	end
end
