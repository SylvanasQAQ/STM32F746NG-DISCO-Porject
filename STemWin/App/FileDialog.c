// USER START (Optionally insert additional includes)
#include "main.h"
#include "fatfs.h"
// USER END

#include "DIALOG.h"
#include "string.h"


#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_TREEVIEW_0 (GUI_ID_USER + 0x01)
#define ID_TREEVIEW_1 (GUI_ID_USER + 0x02)


const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 30, 480, 242, 0, 0x0, 0 },
  { TREEVIEW_CreateIndirect, "Treeview", ID_TREEVIEW_0, 0, 0, 240, 232, 0, 0x0, 0 },
  { TREEVIEW_CreateIndirect, "Treeview", ID_TREEVIEW_1, 240, 0, 480, 232, 0, 0x0, 0 },

};

/************************************************************************************
*函数:  ScanFiles
*作者:  alan
*日期:  2014.07.07
*参数:  path:遍历路径名
        hFile:记录文件路径的文件指针结构体
*返回:  None
*描述:  遍历此文件夹及子文件夹下所有文件
************************************************************************************/
static FRESULT ScanFiles(char* path,WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    UINT rw;
    int i;
    char *fn;   /* This function is assuming non-Unicode cfg. */
    TREEVIEW_ITEM_Handle hItem;

#if _USE_LFN
    static char lfn[FILE_PATH_MAX]={0};   /* Buffer to store the LFN */
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif

    res = f_opendir(&dir, (const TCHAR*)path);                       /* Open the directory */
    if (res == FR_OK)
    {
        i = strlen((const char*)path);
        for (;;)
        {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
           // if (fno.fname[0] == '.') continue;             /* Ignore dot entry */ //忽略上级目录
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) /* It is a directory */
            {
                if(hTree != NULL &&        hNode != NULL)
                {
                    //目录，创建结点
                    hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE,fn,0);
                    //把结点加入到目录树中
                    TREEVIEW_AttachItem(hTree,hItem,hNode,TREEVIEW_INSERT_FIRST_CHILD);
                }
                //在路径最后添加文件夹路径名
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
				printf("%s\r\n",path);
                //遍历此新文件夹下的文件
                res = ScanFiles(path,hTree,hItem);
                if (res != FR_OK) break;//打开失败，跳出循环
                 path[i] = 0;
            }
            else /* It is a file. */
            {
                /* 根据要求是否创建目录树 */
                if(hTree != NULL &&        hNode != NULL)//创建目录树
                {
                    hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_LEAF,fn,0);//文件，创建树叶
                    TREEVIEW_AttachItem(hTree,hItem,hNode,TREEVIEW_INSERT_FIRST_CHILD);//把树叶添加到目录树
					//            (*fileNum)++;//记录文件数目
                }

            }

        }
        f_closedir(&dir);
    }
    return res;
}



void _cbDialog(WM_MESSAGE * pMsg) {
  TREEVIEW_ITEM_Handle   hTreeItemCur;
  TREEVIEW_ITEM_Handle   hTreeItemNew;
  WM_HWIN                hTree,hItem;
  int                    NCode;
  int                    Id;
	TREEVIEW_ITEM_Handle   hTreeItem_SDCard;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Treeview'
    //
    hTree = WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0);
    TREEVIEW_SetAutoScrollV(hTree, 1);    //使用自动滑动条
    TREEVIEW_SetAutoScrollH(hTree, 1);    //使用自动滑动条
    TREEVIEW_SetSelMode(hTree, TREEVIEW_SELMODE_ROW);

    hTreeItemCur = TREEVIEW_GetItem(hTree, 0, TREEVIEW_GET_LAST);
    hTreeItem_SDCard = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE, "SDCard[0:]", 0);
    TREEVIEW_AttachItem(hTree, hTreeItem_SDCard, hTreeItemCur, TREEVIEW_INSERT_BELOW);
  
  
	char buf[200];
	memset(buf,0x00,sizeof(buf));
	strcat(buf, "");
	ScanFiles(buf,hTree, hTreeItem_SDCard);
  
  
  TREEVIEW_ITEM_Expand(TREEVIEW_GetSel(hTree));
  TREEVIEW_ITEM_ExpandAll(TREEVIEW_GetSel(hTree));
  
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_1);
    hTreeItemCur = TREEVIEW_GetItem(hItem, 0, TREEVIEW_GET_LAST);
    hTreeItemNew = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE, "U DISK", 0);
    TREEVIEW_AttachItem(hItem, hTreeItemNew, hTreeItemCur, TREEVIEW_INSERT_BELOW);
	TREEVIEW_SetIndent(hItem,20);
	TREEVIEW_SetAutoScrollH(hItem,1);		
	TREEVIEW_SetAutoScrollV(hItem,1);
	TREEVIEW_SetFont(hItem,GUI_FONT_16B_ASCII);
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_TREEVIEW_0: // Notifications sent by 'Treeview'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_MOVED_OUT:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_TREEVIEW_1: // Notifications sent by 'Treeview'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_MOVED_OUT:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;
  // USER START (Optionally insert additional message handling)
  // USER END
  default:
    WM_DefaultProc(pMsg);
  }
}


WM_HWIN CreateFileDialog(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}
