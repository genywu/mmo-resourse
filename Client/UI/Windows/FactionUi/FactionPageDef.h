
#pragma  once 

#define  UpFactionPageList(mbox,text,col,row) \
    { \
        CEGUI::ListboxTextItem* ltbox = new CEGUI::ListboxTextItem(text); \
        ltbox->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME); \
        mbox->setItem(ltbox,col,row);\
    }
#define  UpFactionPageListAppData(mbox,text,col,row,data) \
    { \
        CEGUI::ListboxTextItem* ltbox = new CEGUI::ListboxTextItem(text); \
        ltbox->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME); \
        ltbox->setUserData((void*)data);\
        mbox->setItem(ltbox,col,row);\
    }

