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
#include "os_threads.h"
#include <string.h>
#include <math.h>
// USER END

#include "DIALOG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_TEXT_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_0 (GUI_ID_USER + 0x02)
#define ID_BUTTON_1 (GUI_ID_USER + 0x03)
#define ID_TEXT_1 (GUI_ID_USER + 0x04)
#define ID_TEXT_2 (GUI_ID_USER + 0x05)
#define ID_TEXT_3 (GUI_ID_USER + 0x06)
#define ID_TEXT_4 (GUI_ID_USER + 0x07)
#define ID_TEXT_5 (GUI_ID_USER + 0x08)
#define ID_TEXT_6 (GUI_ID_USER + 0x09)
#define ID_TEXT_7 (GUI_ID_USER + 0x0A)
#define ID_TEXT_8 (GUI_ID_USER + 0x0B)
#define ID_TEXT_9 (GUI_ID_USER + 0x0C)
#define ID_DROPDOWN_0 (GUI_ID_USER + 0x0D)
#define ID_TEXT_10 (GUI_ID_USER + 0x0E)


// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
static void DrawSpectrum(uint16_t x, uint16_t y);
static void TimerEventHandler(WM_MESSAGE *pMsg);
static void PaintEventHandler();

extern float32_t fftResultMag[256];
extern uint16_t Freq_FFT_Ready;
extern uint16_t Freq_FFT_Points;

extern uint16_t Freq_MainFreq_Ready;
extern uint16_t Freq_MainFreq_Index;

extern GUI_HWIN hCurrentWindow;
extern GUI_HWIN hFreqAnalysisWindow;


// 频谱显示样式，0 -- 幅度谱， 1 -- 频率谱
uint16_t spectrumStyle = 0;
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "FreqAnalysisWindow", ID_WINDOW_0, 0, 0, 480, 242, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Title", ID_TEXT_0, 190, 0, 100, 20, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "Start", ID_BUTTON_0, 415, 105, 60, 40, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Style", ID_BUTTON_1, 415, 165, 60, 40, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "MainFreq", ID_TEXT_1, 0, 18, 130, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text1", ID_TEXT_2, 2, 225, 25, 15, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text2", ID_TEXT_3, 58, 225, 25, 15, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text7", ID_TEXT_4, 335, 225, 25, 15, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text3", ID_TEXT_5, 115, 225, 25, 15, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text4", ID_TEXT_6, 175, 225, 25, 15, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text5", ID_TEXT_7, 229, 225, 25, 15, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text6", ID_TEXT_8, 284, 225, 25, 15, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_9, 372, 225, 25, 15, 0, 0x64, 0 },
  { DROPDOWN_CreateIndirect, "Dropdown", ID_DROPDOWN_0, 414, 37, 60, 18, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "StyleText", ID_TEXT_10, 290, 18, 125, 20, 0, 0x64, 0 },
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
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'FreqAnalysisWindow'
    //
    hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, GUI_MAKE_COLOR(0x00EBEBE2));
    //
    // Initialization of 'Title'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetFont(hItem, GUI_FONT_20B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetText(hItem, "Spectrum");
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FF0000));
    //
    // Initialization of 'Start'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem, GUI_FONT_16B_ASCII);
    BUTTON_SetText(hItem, "Start");
    //
    // Initialization of 'Style'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetFont(hItem, GUI_FONT_16B_ASCII);
    BUTTON_SetText(hItem, "Style");
    //
    // Initialization of 'MainFreq'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x008000FF));
    TEXT_SetText(hItem, "Main Freq: 0 Hz");
    //
    // Initialization of 'Text1'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
    TEXT_SetText(hItem, "100");
    TEXT_SetFont(hItem, GUI_FONT_13_1);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FF9FF5));
    //
    // Initialization of 'Text2'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetText(hItem, "3k");
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FE56FA));
    TEXT_SetFont(hItem, GUI_FONT_13_1);
    //
    // Initialization of 'Text7'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetText(hItem, "18k");
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x004E284F));
    //
    // Initialization of 'Text3'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetText(hItem, "6k");
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00EB20F0));
    //
    // Initialization of 'Text4'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetText(hItem, "9k");
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00A812CD));
    //
    // Initialization of 'Text5'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_7);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetText(hItem, "12k");
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00B405B4));
    //
    // Initialization of 'Text6'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetText(hItem, "15k");
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x007E1F7C));
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_9);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetText(hItem, "20k");
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00361B3A));
    //
    // Initialization of 'Dropdown'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
    DROPDOWN_SetFont(hItem, GUI_FONT_16B_ASCII);
    DROPDOWN_AddString(hItem, "128");
    DROPDOWN_AddString(hItem, "64");
    DROPDOWN_AddString(hItem, "32");
    DROPDOWN_SetListHeight(hItem, 50);
    //
    // Initialization of 'StyleText'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_10);
    TEXT_SetFont(hItem, GUI_FONT_13HB_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FF8080));
    TEXT_SetText(hItem, "@Amplitude Spec");
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_0: // Notifications sent by 'Start'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        #ifdef CMSIS_V1
        xTaskCreate(app_freqAnalysis_thread, "FreqAnalysis Task", 256, NULL, osPriorityNormal, &app_freqAnalysisTaskHandle);
        WM_CreateTimer(pMsg->hWin, 0, 100, 0);
        #endif

        #ifdef CMSIS_V2
        app_freqAnalysisTaskHandle = osThreadNew(app_freqAnalysis_thread, NULL, &app_audioTask_attributes);
        WM_CreateTimer(pMsg->hWin, 0, 200000, 0);
        #endif
        WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, Id));
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
    case ID_BUTTON_1: // Notifications sent by 'Style'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        spectrumStyle = !spectrumStyle;
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_10);
        if(spectrumStyle)
          TEXT_SetText(hItem, "@Logarithmic Spec");
        else
          TEXT_SetText(hItem, "@Amplitude Spec");
        //WM_Paint(hItem);
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
    case ID_DROPDOWN_0: // Notifications sent by 'Dropdown'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        hItem = WM_GetDialogItem(pMsg->hWin, Id);
        switch (DROPDOWN_GetSel(hItem))
        {
        case 0:
          Freq_FFT_Points = 128;
          break;
        case 1:
          Freq_FFT_Points = 64;
          break;
        case 2:
          Freq_FFT_Points = 32;
          break;
        default:
          Freq_FFT_Points = 128;
          break;
        }
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
    TimerEventHandler(pMsg);
    break;
  case WM_PRE_PAINT:
    GUI_MULTIBUF_Begin();
    break;
  case WM_PAINT:
    PaintEventHandler();
    break;
  case WM_POST_PAINT:
    GUI_MULTIBUF_End();
    break;
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
*       CreateFreqAnalysisWindow
*/
WM_HWIN CreateFreqAnalysisWindow(void);
WM_HWIN CreateFreqAnalysisWindow(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
void MoveToFreqAnalysisWindow(WM_HWIN hWin)
{
  WM_EnableWindow(WM_GetDialogItem(hWin, ID_BUTTON_0));
}



/**
 * @brief  处理 WM_TIMER 事件
 * @param  WM_MESSAGE* pMsg
 * @retval None
 */
static void TimerEventHandler(WM_MESSAGE *pMsg)
{
  static const GUI_RECT DrawingRect = {0, 40, 410, 242};
  static WM_HWIN               hItem;
  static char           mainFreqString[30] = "Main Freq: 0 Hz";


  // 显示主频率
  if (Freq_MainFreq_Ready)
  {
    Freq_MainFreq_Ready = 0;
    sprintf(mainFreqString, "Main Freq: %d Hz", 40000 * Freq_MainFreq_Index / 1024);
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    WM_Paint(hItem);
    TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x008000FF));
    TEXT_SetText(hItem, mainFreqString);
  }

  // 清空频谱绘制区域
  if (Freq_FFT_Ready)
    WM_InvalidateRect(pMsg->hWin, &DrawingRect);


  if (hCurrentWindow == hFreqAnalysisWindow)
    #ifdef CMSIS_V1
    WM_RestartTimer(pMsg->Data.v, 1);
    #endif
    #ifdef CMSIS_V2
    WM_RestartTimer(pMsg->Data.v, 10000);
    #endif
  else
  {
    WM_InvalidateRect(pMsg->hWin, &DrawingRect);
    WM_DeleteTimer(pMsg->Data.v);
  }
}



/**
 * @brief  处理 WM_PAINT 事件
 * @param  None
 * @retval None
 */
static void PaintEventHandler()
{
  if (Freq_FFT_Ready)
  {
    GUI_Clear();
    GUI_SetColor(GUI_WHITE);
    GUI_DrawRoundedFrame(0, 40, 410, 238, 30, 2);    
    DrawSpectrum(10, 232 - 180 - 10);

    Freq_FFT_Ready = 0;
  }
}




/**
 * @brief  频谱显示
 * @param  uint16_t x   频谱x坐标
 * @param  uint16_t y   频谱y坐标
 * @retval None
 */
static void DrawSpectrum(uint16_t x, uint16_t y)
{
  static uint16_t       topValueArr[128] = {0}; /* 频谱顶值表 */
  static uint16_t       curValueArr[128] = {0}; /* 频谱当前值表 */
  static uint8_t        timeArr[128] = {0};      /* 顶值停留时间表 */
  static const uint16_t maxVal = 180;     /* 高度固定为128个像素 */
  static uint16_t       i;
  static uint16_t       temp;
  static float32_t      ufTempValue;
  static uint16_t       deltaX;

  deltaX = (128 / Freq_FFT_Points) * 3;
  /* 显示32条频谱 */
  for (i = 0; i < Freq_FFT_Points; i++)
  {
    if (spectrumStyle == 1)
    {
      /* 对数谱，对幅值取对数 */
      ufTempValue = fftResultMag[i] / 128 * (128 / Freq_FFT_Points);
      if (ufTempValue < 1)
        ufTempValue = 0;
      temp = (uint16_t)(40 * log10f(ufTempValue));
    }
    else // 幅值谱
      temp = (uint16_t)fftResultMag[i] / 16 * (128 / Freq_FFT_Points);

    /* 2. 更新频谱数值 */
    if (curValueArr[i] < temp)
      curValueArr[i] = temp;
    else
    {
      if (curValueArr[i] > 1)
        curValueArr[i] -= 2;
      else
        curValueArr[i] = 0;
    }

    /* 3. 更新频谱顶值 */
    if (timeArr[i])
      timeArr[i]--;
    else if (topValueArr[i])
      topValueArr[i]--;

    /* 4. 重设频谱顶值 */
    if (curValueArr[i] > topValueArr[i])
    {
      topValueArr[i] = curValueArr[i];
      timeArr[i] = 10; /* 重设峰值停顿时间 */
    }

    /* 5. 防止超出频谱值和顶值范围，高度固定为128个像素 */
    if (curValueArr[i] > maxVal)
      curValueArr[i] = maxVal;

    if (topValueArr[i] > maxVal)
      topValueArr[i] = maxVal;
  }

  /* 6. 绘制得到的频谱 */
  for (i = 0; i < Freq_FFT_Points; i++)
  {
    /* 显示频谱 */
    GUI_DrawGradientV(x,
                      y + maxVal - curValueArr[i],
                      x + deltaX,
                      y + maxVal,
                      GUI_YELLOW,
                      GUI_GREEN);

    /* 显示顶值 */
    GUI_SetColor(GUI_RED);
    GUI_DrawHLine(y + maxVal - topValueArr[i] - 1, x, x + deltaX);
    x += deltaX;
  }
}
// USER END

/*************************** End of file ****************************/
