/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.44                          *
*        Compiled Nov 10 2017, 08:53:57                              *
*        (c) 2017 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
#include "fatfs.h"
#include <stdio.h>
// USER END

#include "DIALOG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_TREEVIEW_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_0 (GUI_ID_USER + 0x02)
#define ID_BUTTON_1 (GUI_ID_USER + 0x03)
#define ID_BUTTON_2 (GUI_ID_USER + 0x04)


// USER START (Optionally insert additional defines)
static FRESULT ScanFiles(char* path,WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode);
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "FileFramewin", ID_FRAMEWIN_0, 0, 0, 320, 198, 0, 0x64, 0 },
  { TREEVIEW_CreateIndirect, "Treeview", ID_TREEVIEW_0, 0, 0, 309, 150, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "OKBtn", ID_BUTTON_0, 15, 150, 30, 25, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "QuitBtn", ID_BUTTON_1, 253, 150, 40, 25, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "NOBtn", ID_BUTTON_2, 56, 150, 30, 25, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
  TREEVIEW_ITEM_Handle   hTreeItemCur;
  TREEVIEW_ITEM_Handle   hTreeItem_SDCard;
  WM_HWIN                hTree;
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'FileFramewin'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    FRAMEWIN_SetFont(hItem, GUI_FONT_13B_ASCII);
    FRAMEWIN_SetText(hItem, "File Selector");
    FRAMEWIN_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FF0080));
    //
    // Initialization of 'OKBtn'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem, GUI_FONT_16B_ASCII);
    BUTTON_SetText(hItem, "O");
    //
    // Initialization of 'QuitBtn'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetFont(hItem, GUI_FONT_16B_ASCII);
    BUTTON_SetText(hItem, "Quit");
    //
    // Initialization of 'NOBtn'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
    BUTTON_SetText(hItem, "X");
    BUTTON_SetFont(hItem, GUI_FONT_16B_ASCII);
    // USER START (Optionally insert additional code for further widget initialization)
    hTree = WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0);
    TREEVIEW_SetAutoScrollV(hTree, 1);    
    TREEVIEW_SetAutoScrollH(hTree, 1);    
    TREEVIEW_SetSelMode(hTree, TREEVIEW_SELMODE_ROW);
    hTreeItemCur = TREEVIEW_GetItem(hTree, 0, TREEVIEW_GET_LAST);
    hTreeItem_SDCard = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE, "SDCard[0:]", 0);
    TREEVIEW_AttachItem(hTree, hTreeItem_SDCard, hTreeItemCur, TREEVIEW_INSERT_BELOW);

    char buf[200];
    memset(buf, 0x00, sizeof(buf));
    strcat(buf, "");
    ScanFiles(buf, hTree, hTreeItem_SDCard);

    TREEVIEW_ITEM_Expand(TREEVIEW_GetSel(hTree));
    TREEVIEW_ITEM_ExpandAll(TREEVIEW_GetSel(hTree));

    hItem = WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0);
    TREEVIEW_SetIndent(hItem, 20);
    TREEVIEW_SetAutoScrollH(hItem, 1);
    TREEVIEW_SetAutoScrollV(hItem, 1);
    TREEVIEW_SetFont(hItem, GUI_FONT_16B_ASCII);
    // USER END
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
    case ID_BUTTON_0: // Notifications sent by 'OKBtn'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_1: // Notifications sent by 'QuitBtn'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        GUI_EndDialog(pMsg->hWin, 0);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_2: // Notifications sent by 'NOBtn'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
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
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateFileFramewin
*/
WM_HWIN CreateFileFramewin(void);
WM_HWIN CreateFileFramewin(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
WM_HWIN CreateFileDialog(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 80, 40);
  WM_MakeModal(hWin);
  WM_SetStayOnTop(hWin, 1);
  return hWin;
}

static FRESULT ScanFiles(char *path, WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode)
{
  FRESULT res;
  FILINFO fno;
  DIR dir;
  char *fn;                   /* This function is assuming non-Unicode cfg. */
  char path_other[255] = {0}; //目录 长度
  TREEVIEW_ITEM_Handle hItem;

#if _USE_LFN
  static char lfn[FILE_PATH_MAX] = {0}; /* Buffer to store the LFN */
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
#endif

  res = f_opendir(&dir, path); /* Open the directory */
  if (res == FR_OK)
  {
    for (;;)
    {
      res = f_readdir(&dir, &fno); /* Read a directory item */
      if (res != FR_OK || fno.fname[0] == 0)
        break; 

      strcpy(path_other, fno.fname);
      path_other[6] = '\0';
      if (fno.fname[0] == '.' || !strcmp(path_other, "SYSTEM") || !strcmp(path_other, "SPOTLI") || !strcmp(path_other, "FSEVEN"))
        continue;

#if _USE_LFN
      fn = *fno.lfname ? fno.lfname : fno.fname;
#else
      fn = fno.fname;
#endif

      if ((fno.fattrib & AM_DIR) && fn[0] != '.') /* It is a directory */
      {
        if (hTree != NULL && hNode != NULL)
        {
          //目录，创建结点
          hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE, fn, 0);
          //把结点加入到目录树中
          TREEVIEW_AttachItem(hTree, hItem, hNode, TREEVIEW_INSERT_FIRST_CHILD);
        }

        //在路径最后添加文件夹路径名
        memset(path_other, 0, sizeof(path_other));
        sprintf(path_other, "%s/%s", path, fno.fname);
        printf("%s/%s--%ld\n", path, fn, fno.fsize);

        //遍历此新文件夹下的文件
        res = ScanFiles(path_other, hTree, hItem);
        if (res != FR_OK)
          break;
      }
      else /* It is a file. */
      {
        if (hTree != NULL && hNode != NULL) //创建目录树
        {
          hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_LEAF, fn, 0);          //文件，创建树叶
          TREEVIEW_AttachItem(hTree, hItem, hNode, TREEVIEW_INSERT_FIRST_CHILD); //把树叶添加到目录树
        }
        printf("%s/%s--%ld\n", path, fn, fno.fsize);
      }
    }
    f_closedir(&dir);
  }
  return res;
}
// USER END

/*************************** End of file ****************************/