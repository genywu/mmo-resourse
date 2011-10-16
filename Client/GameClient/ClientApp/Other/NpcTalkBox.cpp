#include "stdafx.h"
#include "NpcTalkBox.h"

void NpcTalkBox::LoadTalkBoxImageRes()
{
   CRFile *file = rfOpen("datafiles/imagesets/talkbox/imageList.ini");
   if( file )
   {
       stringstream stream;
       file->ReadToStream(stream);
       rfClose(file);
       while (ReadTo(stream, "*"))
       {
           PicInfo info;
           stream >> info.lID >> info.fileName;
           PicResList.push_back(info);
       }
   }
}