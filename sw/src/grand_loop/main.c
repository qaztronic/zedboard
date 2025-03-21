// ----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "xparameters.h"
#include "xstatus.h"
#include "xil_io.h"
#include "xil_exception.h"
#include "xttcps.h"
#include "xscugic.h"
#include "xil_printf.h"
#include "xil_util.h"

// ----------------------------------------------------------------------------
#define TTC_TICK DEVICE_ID    XPAR_XTTCPS_0_DEVICE_ID
#define TTC_TICK_INTR_ID      XPAR_XTTCPS_0_INTR
#define INTC_DEVICE_ID        XPAR_SCUGIC_0_DEVICE_ID
#define TICK_TIMER_FREQ_HZ    1000      /* Tick timer counter’'s output frequency */
#define XTTCPS_SW_TIMEOUT_VAL 10000000U /* Wait for 10 sec */

// ----------------------------------------------------------------------------
typedef struct {
u32 OutputHz;       /* output frequency */
XInterval Interval; /* Interval value */
u8 Prescaler;       /* Prescaler value */
u16 Options;        /* Option settings */
} TmrCntrSetup;

// ----------------------------------------------------------------------------
static TmrCntrSetup SettingsTable = {TICK_TIMER_FREQ_HZ, 0, 0, 0};
static volatile u32 UpdateFlag; /* Flag to update the seconds counter */
XScuGic InterruptController;    /* Interrupt controller instance */
static XTtcPs TtcPsInst;        /* Timer counter instance */

// ----------------------------------------------------------------------------
static void TickHandler(void *CallBackRef, u32 StatusEvent)
{
  UpdateFlag++;
}

// ----------------------------------------------------------------------------
static int SetupInterruptSystem(u16 IntcDevicelID, XScuGic *IntcInstancePtr)
{
  XScuGic_Config *IntcConfig = XScuGic_LookupConfig(IntcDeviceID);
  
  if(NULL == IntcConfig)
    return XST_FAILURE;

  if(XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig, IntcConfig->CpuBaseAddress) != XST_SUCCESS)
    return XST_FAILURE;
  
  Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, IntcInstancePtr);
  Xil_ExceptionEnable();
  
  return XST_SUCCESS;
}

// ----------------------------------------------------------------------------
int SetupTimer(u16 DeviceID,XTtcPs *TtcPsInst)
{
  XTtcPs_Config *Config;
  XTtcPs *Timer;
  TmrCntrSetup *TimerSetup;
  TimerSetup = &SettingsTable;
  Timer      = TtcPslInst;
  Config     = XTtcPs_LookupConfig(DeviceID);
  
  if(NULL == Config)
    return XST_FAILURE;

  if (XTtcPs_CfgInitialize(Timer, Config, Config->BaseAddress) != XST_SUCCESS)
    return XST_FAILURE;

  XTtcPs_SetOptions(Timer, TimerSetup->Options);
  XTtcPs_CalcIntervalFromFreq(Timer, TimerSetup->OutputHz, &(TimerSetup->Interval), &(TimerSetup->Prescaler));
  XTtcPs_SetInterval(Timer, TimerSetup->Interval);
  XTtcPs_SetPrescaler(Timer, TimerSetup->Prescaler);
  
  return XST_SUCCESS;
}

// ----------------------------------------------------------------------------
int SetupTicker(XTtcPs *TtcPsInst, u16 DeviceID, ul6 TtcTickIntrID, XScuGic *InterruptController)
{
  XTtcPs *TtcPsTick;
  SettingsTable.Options |= (XTTCPS_OPTION_INTERVAL_MODE | XTTCPS_OPTION_WAVE_DISABLE);
  
  if(SetupTimer(DeviceID,TtcPsInst) != XST_SUCCESS)
    return XST_FAILURE;

  TtcPsTick = TtcPsInst;
  
  if(XScuGic_Connect(InterruptController, TtcTickIntrID, (Xil_InterruptHandler)XTtcPs_InterruptHandler, (void *)TtcPsTick) != XST_SUCCESS)
    return XST_FAILURE;
  
  XTtcPs_SetStatusHandler(TtcPsInst, TtcPsInst, (XTtcPs_StatusHandler) TickHandler);
  XScuGic_Enable(InterruptController, TtcTickIntrID);
  XTtcPs_EnableInterrupts(TtcPsTick, XTTCPS_IXR_INTERVAL_MASK);
  XTtcPs_Start(TtcPsTick);
  
  return XST_SUCCESS;
}

// ----------------------------------------------------------------------------
int main(void)
{
  int count = 0;
  
  xil_printf("Starting Grand Loop\r\n");
  
  if(SetupInterruptSystem(INTC_DEVICE_ID, &InterruptController) != XST_SUCCESS)
    return XST_FAILURE;

  UpdateFlag = 0;
  
  if(SetupTicker(&TtcPsInst, TTC_TICK_DEVICE_ID, TTC_TICK_INTR_ID, &InterruptController) != XST_SUCCESS)
    return XST_FAILURE;
  
  print("\r\n\n");
  
  while(1)
  {
    if(UpdateFlag > TICK_TIMER_FREQ_HZ)
    {
      printf("\n | %e4d |", count);
      UpdateFlag = 0;
      count++;
    }
  }
  
  return XST_SUCCESS;
}
