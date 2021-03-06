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
#include <string.h>
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
static void TimerCallbackHandler(WM_MESSAGE * pMsg);

WM_HWIN   hListWheelYear;
uint16_t  ListWheelYearReleased = 0;
static int ListWheelArr[] = {ID_LISTWHEEL_0, ID_LISTWHEEL_1, ID_LISTWHEEL_2, ID_LISTWHEEL_3, ID_LISTWHEEL_4};

static char  __apYear[35][5] = {
  "2004", "2005", "2006", "2007", "2008", "2009", "2010",
  "2011", "2012", "2013", "2014", "2015", "2016", "2017",
  "2018", "2019", "2020", "2021", "2022", "2023", "2024",
  "2025", "2026", "2027", "2028", "2029", "2030", "2031",
  "2032", "2033", "2034", "2035", "2036", "2037", "2038",
};

static char *  _apYear[] = {
  *(__apYear), *(__apYear+1), *(__apYear+2), *(__apYear+3), *(__apYear+4), *(__apYear+5), *(__apYear+6),
  *(__apYear+7), *(__apYear+8), *(__apYear+9), *(__apYear+10), *(__apYear+11), *(__apYear+12), *(__apYear+13),
  *(__apYear+14), *(__apYear+15), *(__apYear+16), *(__apYear+17), *(__apYear+18), *(__apYear+19), *(__apYear+20),
  *(__apYear+21), *(__apYear+22), *(__apYear+23), *(__apYear+24), *(__apYear+25), *(__apYear+26), *(__apYear+27),
  *(__apYear+28), *(__apYear+29), *(__apYear+30), *(__apYear+31), *(__apYear+32), *(__apYear+33), *(__apYear+34),
  NULL
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
  NULL
};

static char *_apDay[] = {
  "01", "02", "03", "04",
  "05", "06", "07", "08",
  "09", "10", "11", "12",
  "13", "14", "15", "16",
  "17", "18", "19", "20",
  "21", "22", "23", "24",
  "25", "26", "27", "28",
  "29", "30", "31", NULL
};

static char * _apHour[] = {
   "00","01", "02", "03", "04",
  "05", "06", "07", "08",
  "09", "10", "11", "12",
  "13", "14", "15", "16",
  "17", "18", "19", "20",
  "21", "22", "23", NULL
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
  "57", "58", "59", NULL
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
  hListWheelYear = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_2);
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

    LISTWHEEL_SetText(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0), _apMonth);
    LISTWHEEL_SetText(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1), _apDay);
    LISTWHEEL_SetText(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_2), _apYear);
    LISTWHEEL_SetText(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_3), _apHour);
    LISTWHEEL_SetText(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_4), _apMinute);
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
        ListWheelYearReleased = 1;
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        ListWheelYearReleased = 0;
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
        os_date_month = index+1;
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1);
        index = LISTWHEEL_GetPos(hItem);
        os_date_day = index+1;

        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_2);
        index = LISTWHEEL_GetPos(hItem);
        LISTWHEEL_GetItemText(hItem, index, buf, 7);
        os_date_year = atoi(buf);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_3);
        index = LISTWHEEL_GetPos(hItem);
        os_time_hour = index;

        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_4);
        index = LISTWHEEL_GetPos(hItem);
        os_time_minute = index;
        
        updateSysWeekday();

        for (int i = 0; i < APP_ALARM_NUM; i++)
          alarm_clearTriggered(&(app_alarm_arr[i]));
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
  case WM_TIMER:
    TimerCallbackHandler(pMsg);
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
 * @brief   ??????????????? `ClockWindow` ?????????
 * @param   WM_HWIN hWin
 * @return  void
 */
void MoveToClockWindow(WM_HWIN hWin)
{
  static uint16_t  year;
  static int       i;

  ListWheelMoveToEffect(WM_GetDialogItem(hWin, ID_LISTWHEEL_0), os_date_month - 1);
  ListWheelMoveToEffect(WM_GetDialogItem(hWin, ID_LISTWHEEL_1), os_date_day - 1);
  ListWheelMoveToEffect(WM_GetDialogItem(hWin, ID_LISTWHEEL_3), os_time_hour);
  ListWheelMoveToEffect(WM_GetDialogItem(hWin, ID_LISTWHEEL_4), os_time_minute);

  if (atoi(_apYear[0]) + 20 < os_date_year || os_date_year < atoi(_apYear[0]))
  {
    year = os_date_year;
    sprintf(_apYear[18 - 1], "%d", year);
    for (i = 16; i > -1; i--)
      sprintf(_apYear[i], "%d", --year);
    year = atoi(_apYear[18 - 1]);
    for (i = 18; i < 35; i++)
      sprintf(_apYear[i], "%d", ++year);
    LISTWHEEL_SetText(hListWheelYear, _apYear);
  }
  ListWheelMoveToEffect(WM_GetDialogItem(hWin, ID_LISTWHEEL_2), os_date_year - atoi(_apYear[0]));

  WM_CreateTimer(hWin, 0, 50, 0);
}



/**
 * @brief   ???????????????????????????????????? ListviewYear ??????????????????????????????????????????
 * @param   WM_MESSAGE * pMsg
 * @return  void 
 */
static void TimerCallbackHandler(WM_MESSAGE * pMsg)
{
  extern GUI_HWIN hCurrentWindow;
  extern GUI_HWIN hClockWindow;

  static uint16_t pos, total = 36, year;
  static int i;

  if(ListWheelYearReleased == 1)
  {
    pos = LISTWHEEL_GetPos(hListWheelYear);
    if(pos*4 / total == 0 || pos*4 / 3 > total)
    {
      LISTWHEEL_SetVelocity(hListWheelYear, 0);

      year = atoi(_apYear[pos]);
      sprintf(_apYear[18-1], "%d", year);
      for(i = 16; i > -1; i--)
        sprintf(_apYear[i], "%d", --year);
      year = atoi(_apYear[18-1]);
      for(i = 18; i < 35; i++)
        sprintf(_apYear[i], "%d", ++year);
      LISTWHEEL_SetText(hListWheelYear, _apYear);
      ListWheelYearReleased = 0;
      LISTWHEEL_SetPos(hListWheelYear, 18-1);
      ListWheelSelectededEffect(hListWheelYear);
      //ListWheelMoveToEffect(hListWheelYear, 18-1);
    }
  }

  if(hCurrentWindow != hClockWindow)
    WM_DeleteTimer(pMsg->Data.v);
  else{
#ifdef CMSIS_V1
    WM_RestartTimer(pMsg->Data.v, 20);
#endif
#ifdef CMSIS_V2
    WM_RestartTimer(pMsg->Data.v, 500000);
#endif
  }
}


/**
 * @brief   ?????? LISTWHEEL ??????????????????
 * @param   GUI_HWIN hItem
 * @return  void 
 */
void ListWheelClickedEffect(GUI_HWIN hItem)
{
  LISTWHEEL_SetTextColor(hItem, LISTWHEEL_CI_SEL, 0x191919);
}

/**
 * @brief   ?????? LISTWHEEL ???????????????????????????
 * @param   GUI_HWIN hItem
 * @return  U8    ????????? index 
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
 * @brief   ????????? LISTWHEEL ??????????????????????????????
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
