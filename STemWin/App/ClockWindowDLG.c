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
#include <stdlib.h>
#include "os_time.h"
#include<stdio.h>
#include "os_threads.h"
#include "app_alarm.h"
// USER END

#include "DIALOG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0     (GUI_ID_USER + 0x00)
#define ID_LISTWHEEL_0     (GUI_ID_USER + 0x01)
#define ID_LISTWHEEL_1     (GUI_ID_USER + 0x02)
#define ID_LISTWHEEL_2     (GUI_ID_USER + 0x03)
#define ID_TEXT_0     (GUI_ID_USER + 0x04)
#define ID_TEXT_1     (GUI_ID_USER + 0x05)
#define ID_LISTWHEEL_3     (GUI_ID_USER + 0x06)
#define ID_LISTWHEEL_4     (GUI_ID_USER + 0x07)
#define ID_BUTTON_0     (GUI_ID_USER + 0x08)
#define ID_BUTTON_1     (GUI_ID_USER + 0x09)


// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
void MoveToClockWindow(WM_HWIN hWin);
void ListWheelClickedEffect(GUI_HWIN hItem);
U8 ListWheelSelectededEffect(GUI_HWIN hItem);
void ListWheelMoveToEffect(GUI_HWIN hItem, uint8_t pos);



static int ListWheelArr[] = {ID_LISTWHEEL_0, ID_LISTWHEEL_1, ID_LISTWHEEL_2, ID_LISTWHEEL_3, ID_LISTWHEEL_4};

static char *_apYear[] = {
  "1990","1991", "1992", "1993", "1994", "1995", "1996",
  "1997", "1998", "1999", "2000", "2001", "2002", "2003",
  "2004", "2005", "2006", "2007", "2008", "2009", "2010",
  "2011", "2012", "2013", "2014", "2015", "2016", "2017",
  "2018", "2019", "2020", "2021", "2022", "2023", "2024"
};

static char *_apMonth[] = {
  "January",
  "February",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December",
};

static char *_apDay[] = {
  "01", "02", "03", "04",
  "05", "06", "07", "08",
  "09", "10", "11", "12",
  "13", "14", "15", "16",
  "17", "18", "19", "20",
  "21", "22", "23", "24",
  "25", "26", "27", "28",
  "29", "30", "31",
};

static char * _apHour[] = {
   "00","01", "02", "03", "04",
  "05", "06", "07", "08",
  "09", "10", "11", "12",
  "13", "14", "15", "16",
  "17", "18", "19", "20",
  "21", "22", "23",
};

static char * _apMinute[] = {
  "00","01", "02", "03", "04",
  "05", "06", "07", "08",
  "09", "10", "11", "12",
  "13", "14", "15", "16",
  "17", "18", "19", "20",
  "21", "22", "23", "24",
  "25", "26", "27", "28",
  "29", "30", "31", "32",
  "33", "34", "35", "36",
  "37", "38", "39", "40",
  "41", "42", "43", "44",
  "45", "46", "47", "48",
  "49", "50", "51", "52",
  "53", "54", "55", "56",
  "57", "58", "59", 
};
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "ClockWindow", ID_WINDOW_0, 0, 0, 480, 242, 0, 0x0, 0 },
  { LISTWHEEL_CreateIndirect, "ListwheelMon", ID_LISTWHEEL_0, 10, 20, 100, 178, 0, 0x0, 0 },
  { LISTWHEEL_CreateIndirect, "ListwheelDay", ID_LISTWHEEL_1, 120, 20, 40, 178, 0, 0x0, 0 },
  { LISTWHEEL_CreateIndirect, "ListwheelYear", ID_LISTWHEEL_2, 170, 20, 60, 178, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "DateText", ID_TEXT_0, 83, 5, 80, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "TimeText", ID_TEXT_1, 298, 5, 80, 20, 0, 0x64, 0 },
  { LISTWHEEL_CreateIndirect, "ListwheelHour", ID_LISTWHEEL_3, 287, 20, 40, 178, 0, 0x0, 0 },
  { LISTWHEEL_CreateIndirect, "ListwheelMinute", ID_LISTWHEEL_4, 342, 20, 40, 178, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "ConfirmButton", ID_BUTTON_0, 400, 50, 70, 40, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "DiscardButton", ID_BUTTON_1, 400, 124, 70, 40, 0, 0x0, 0 },
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
static int _OwnerDraw(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
  GUI_RECT aRect;

  switch (pDrawItemInfo->Cmd) {
  case WIDGET_ITEM_DRAW_OVERLAY:
    aRect.x0 = pDrawItemInfo->x0;
    aRect.x1 = pDrawItemInfo->x1;
    aRect.y1 = pDrawItemInfo->y1;
    GUI_SetColor(GUI_GRAY_E7);
    GUI_DrawLine(aRect.x0, (aRect.y1-19-16)/2, aRect.x1, (aRect.y1-19-16)/2);
    GUI_DrawLine(aRect.x0, (aRect.y1+19+16)/2, aRect.x1, (aRect.y1+19+16)/2);
    break;
  default:
    return LISTWHEEL_OwnerDraw(pDrawItemInfo);
  }
  return 0;
}


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
  WM_HWIN hListWheelMonth = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
  WM_HWIN hListWheelDay = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1);
  WM_HWIN hListWheelYear = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_2);
  char    buf[8];
  U8      index;
  U32     year;
  U32     month;
  U8      dayErrorIndicator = 0;
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'DateText'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetText(hItem, "Set Date");
    TEXT_SetFont(hItem, GUI_FONT_13HB_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x008000FF));
    //
    // Initialization of 'TimeText'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x008000FF));
    TEXT_SetFont(hItem, GUI_FONT_13HB_ASCII);
    TEXT_SetText(hItem, "Set Time");
    //
    // Initialization of 'ConfirmButton'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetText(hItem, "Confirm");
    BUTTON_SetFont(hItem, GUI_FONT_13B_ASCII);
    //
    // Initialization of 'DiscardButton'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetText(hItem, "Discard");
    BUTTON_SetFont(hItem, GUI_FONT_13B_ASCII);
    // USER START (Optionally insert additional code for further widget initialization)
    hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, GUI_MAKE_COLOR(0x00E3F5D8));

    
    for (int i = 0; i < 5; i++)
    {
      hItem = WM_GetDialogItem(pMsg->hWin, ListWheelArr[i]);
      LISTWHEEL_SetLineHeight(hItem, 34);
      LISTWHEEL_SetSnapPosition(hItem, (178 - 34) / 2);
      LISTWHEEL_SetFont(hItem, GUI_FONT_16B_ASCII);
      LISTWHEEL_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
      LISTWHEEL_SetTextColor(hItem, LISTWHEEL_CI_UNSEL, 0x191919);
      LISTWHEEL_SetTextColor(hItem, LISTWHEEL_CI_SEL, 0x007dfe);
      LISTWHEEL_SetDeceleration(hItem, 35);
      LISTWHEEL_SetOwnerDraw(hItem, _OwnerDraw);
      LISTWHEEL_SetSel(hItem, 0);
    }

    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
    for (int i = 0; i < GUI_COUNTOF(_apMonth); i++)
      LISTWHEEL_AddString(hItem, *(_apMonth + i));
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1);
    for (int i = 0; i < GUI_COUNTOF(_apDay); i++)
      LISTWHEEL_AddString(hItem, *(_apDay + i));
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_2);
    for (int i = 0; i < GUI_COUNTOF(_apYear); i++)
      LISTWHEEL_AddString(hItem, *(_apYear + i));
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_3);
    for (int i = 0; i < GUI_COUNTOF(_apHour); i++)
      LISTWHEEL_AddString(hItem, *(_apHour + i));
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_4);
    for (int i = 0; i < GUI_COUNTOF(_apMinute); i++)
      LISTWHEEL_AddString(hItem, *(_apMinute + i));
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_LISTWHEEL_0: // Notifications sent by 'ListwheelMon'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        ListWheelClickedEffect(hListWheelMonth);
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        month = LISTWHEEL_GetPos(hListWheelMonth) + 1;
        index = LISTWHEEL_GetPos(hListWheelYear);
        LISTWHEEL_GetItemText(hListWheelYear, index, buf, 7);
        year = atoi(buf);
        switch(month){
          case 1:
          case 3:
          case 5:
          case 7:
          case 8:
          case 10:
          case 12:
            break;
          case 2:
            if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)){
              if(LISTWHEEL_GetPos(hListWheelDay) > 29-1){
                dayErrorIndicator = 1;
                LISTWHEEL_MoveToPos(hListWheelDay, 0);
              }
            }
            else if(LISTWHEEL_GetPos(hListWheelDay) > 28-1){
              dayErrorIndicator = 1;
              LISTWHEEL_MoveToPos(hListWheelDay, 0);
            }
            break;
          default:
            if(LISTWHEEL_GetPos(hListWheelDay) > 30-1){
              dayErrorIndicator = 1;
              LISTWHEEL_MoveToPos(hListWheelDay, 0);
            }
            break;
        }
        if(dayErrorIndicator){
          LISTWHEEL_SetSel(hListWheelDay, 0);
          LISTWHEEL_SetTextColor(hListWheelDay, LISTWHEEL_CI_SEL, 0x007dfe);
        }
        ListWheelSelectededEffect(hListWheelMonth);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_LISTWHEEL_1: // Notifications sent by 'ListwheelDay'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        ListWheelClickedEffect(hListWheelDay);
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        month = LISTWHEEL_GetPos(hListWheelMonth) + 1;
        index = LISTWHEEL_GetPos(hListWheelYear);
        LISTWHEEL_GetItemText(hListWheelYear, index, buf, 7);
        year = atoi(buf);
        index = LISTWHEEL_GetPos(hListWheelDay) + 1;
        switch(month){
          case 1:
          case 3:
          case 5:
          case 7:
          case 8:
          case 10:
          case 12:
            break;
          case 2:
            if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)){
              if(index > 29){
                dayErrorIndicator = 1;
                LISTWHEEL_MoveToPos(hListWheelDay, 0);
              }
            }
            else if(index > 28){
              dayErrorIndicator = 1;
              LISTWHEEL_MoveToPos(hListWheelDay, 0);
            }
            break;
          default:
            if(index > 30){
              dayErrorIndicator = 1;
              LISTWHEEL_MoveToPos(hListWheelDay, 0);
            }
            break;
        }
        if(dayErrorIndicator){
          LISTWHEEL_SetSel(hListWheelDay, 0);
          LISTWHEEL_SetTextColor(hListWheelDay, LISTWHEEL_CI_SEL, 0x007dfe);
        }
        else 
          ListWheelSelectededEffect(hListWheelDay);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_LISTWHEEL_2: // Notifications sent by 'ListwheelYear'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        ListWheelClickedEffect(hListWheelYear);
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        month = LISTWHEEL_GetPos(hListWheelMonth) + 1;
        index = LISTWHEEL_GetPos(hListWheelYear);
        LISTWHEEL_GetItemText(hListWheelYear, index, buf, 7);
        year = atoi(buf);
        index = LISTWHEEL_GetPos(hListWheelDay) + 1;
        if(!((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) && month == 2 && index >= 29){
          LISTWHEEL_MoveToPos(hListWheelDay, 0);
          LISTWHEEL_SetSel(hListWheelDay, 0);
          LISTWHEEL_SetTextColor(hListWheelDay, LISTWHEEL_CI_SEL, 0x007dfe);
        }
        ListWheelSelectededEffect(hListWheelYear);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_LISTWHEEL_3: // Notifications sent by 'ListwheelHour'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        ListWheelClickedEffect(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_3));
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        index = ListWheelSelectededEffect(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_3));
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_LISTWHEEL_4: // Notifications sent by 'ListwheelMinute'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        ListWheelClickedEffect(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_4));
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        index = ListWheelSelectededEffect(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_4));
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_0: // Notifications sent by 'ConfirmButton'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
        index = LISTWHEEL_GetPos(hItem);
        date_month = index+1;
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1);
        index = LISTWHEEL_GetPos(hItem);
        date_day = index+1;

        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_2);
        index = LISTWHEEL_GetPos(hItem);
        LISTWHEEL_GetItemText(hItem, index, buf, 7);
        date_year = atoi(buf);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_3);
        index = LISTWHEEL_GetPos(hItem);
        time_hour = index;

        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_4);
        index = LISTWHEEL_GetPos(hItem);
        time_minute = index;
        
        updateSysWeekday();

        for (int i = 0; i < APP_ALARM_NUM; i++)
          alarm_clearTriggered(&(app_alarm_arr[i]));
        alarm_triggered = 0;
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
    case ID_BUTTON_1: // Notifications sent by 'DiscardButton'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        MoveToClockWindow(pMsg->hWin);
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
*       CreateClockWindow
*/
WM_HWIN CreateClockWindow(void);
WM_HWIN CreateClockWindow(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)

/**
 * @brief   完成跳转到 `ClockWindow` 的工作
 * @param   WM_HWIN hWin
 * @return  void
 */
void MoveToClockWindow(WM_HWIN hWin)
{
  WM_HWIN hItem;

  hItem = WM_GetDialogItem(hWin, ID_LISTWHEEL_0);
  LISTWHEEL_MoveToPos(hItem, date_month - 1);
  LISTWHEEL_SetSel(hItem, date_month - 1);

  hItem = WM_GetDialogItem(hWin, ID_LISTWHEEL_1);
  LISTWHEEL_MoveToPos(hItem, date_day - 1);
  LISTWHEEL_SetSel(hItem, date_day - 1);

  hItem = WM_GetDialogItem(hWin, ID_LISTWHEEL_2);
  LISTWHEEL_MoveToPos(hItem, date_year - 1990);
  LISTWHEEL_SetSel(hItem, date_year - 1990);

  hItem = WM_GetDialogItem(hWin, ID_LISTWHEEL_3);
  LISTWHEEL_MoveToPos(hItem, time_hour);
  LISTWHEEL_SetSel(hItem, time_hour);

  hItem = WM_GetDialogItem(hWin, ID_LISTWHEEL_4);
  LISTWHEEL_MoveToPos(hItem, time_minute);
  LISTWHEEL_SetSel(hItem, time_minute);

  for (int i = 0; i < 5; i++){
    hItem = WM_GetDialogItem(hWin, ListWheelArr[i]);
    LISTWHEEL_SetTextColor(hItem, LISTWHEEL_CI_UNSEL, 0x191919);
    LISTWHEEL_SetTextColor(hItem, LISTWHEEL_CI_SEL, 0x007dfe);
  }
}



/**
 * @brief   点击 LISTWHEEL 控件时的效果
 * @param   GUI_HWIN hItem
 * @return  void 
 */
void ListWheelClickedEffect(GUI_HWIN hItem)
{
  LISTWHEEL_SetTextColor(hItem, LISTWHEEL_CI_SEL, 0x191919);
}

/**
 * @brief   选中 LISTWHEEL 控件某个项时的效果
 * @param   GUI_HWIN hItem
 * @return  U8    选中的 index 
 */
U8 ListWheelSelectededEffect(GUI_HWIN hItem)
{
  U8        index;

  index = LISTWHEEL_GetPos(hItem);
  LISTWHEEL_SetTextColor(hItem, LISTWHEEL_CI_UNSEL, 0x191919);
  LISTWHEEL_SetTextColor(hItem, LISTWHEEL_CI_SEL, 0x191919);
  LISTWHEEL_SetSel(hItem, index);
  LISTWHEEL_SetTextColor(hItem, LISTWHEEL_CI_SEL, 0x007dfe);

  return index;
}



/**
 * @brief   跳转到 LISTWHEEL 控件某个位置时的效果
 * @param   GUI_HWIN hItem
 * @param   uint8_t pos
 * @return  void 
 */
void ListWheelMoveToEffect(GUI_HWIN hItem, uint8_t pos)
{
  LISTWHEEL_MoveToPos(hItem, pos);
  LISTWHEEL_SetTextColor(hItem, LISTWHEEL_CI_UNSEL, 0x191919);
  LISTWHEEL_SetTextColor(hItem, LISTWHEEL_CI_SEL, 0x191919);
  LISTWHEEL_SetSel(hItem, pos);
  LISTWHEEL_SetTextColor(hItem, LISTWHEEL_CI_SEL, 0x007dfe);
}
// USER END

/*************************** End of file ****************************/
