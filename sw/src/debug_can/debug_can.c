// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "platform.h"
#include "sys_cmd.h"
#include "xparameters.h"
#include "xcanps.h"

/************************** Constant Definitions *****************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define CAN_DEVICE_ID	XPAR_XCANPS_0_DEVICE_ID

/*
 * Maximum CAN frame length in words.
 */
#define XCANPS_MAX_FRAME_SIZE_IN_WORDS (XCANPS_MAX_FRAME_SIZE / sizeof(u32))

#define FRAME_DATA_LENGTH 		8  /* Frame Data field length */

/*
 * Message Id Constant.
 */
#define ELMO_FRAME_ID           0x01030010
#define ELMO_STANDARD_FRAME_ID  (ELMO_FRAME_ID >> 18)
#define ELMO_EXTENDED_FRAME_ID  (ELMO_FRAME_ID & 0x7FFFF)
#define ELMO_IDVALUE ( (u32)XCanPs_CreateIdValue((u32)ELMO_STANDARD_FRAME_ID, 0, 1, (u32)ELMO_EXTENDED_FRAME_ID, 0) )

/*
 * The Baud Rate Prescaler Register (BRPR) and Bit Timing Register (BTR)
 * are setup such that CAN baud rate equals 40Kbps, assuming that the
 * the CAN clock is 24MHz. The user needs to modify these values based on
 * the desired baud rate and the CAN clock frequency. For more information
 * see the CAN 2.0A, CAN 2.0B, ISO 11898-1 specifications.
 */

/*
 * Timing parameters to be set in the Bit Timing Register (BTR).
 * These values are for a 40 Kbps baudrate assuming the CAN input clock
 frequency
 * is 24 MHz.
 */
#define TEST_BTR_SYNCJUMPWIDTH		3
#define TEST_BTR_SECOND_TIMESEGMENT	2
#define TEST_BTR_FIRST_TIMESEGMENT	15

/*
 * The Baud rate Prescalar value in the Baud Rate Prescaler Register (BRPR)
 * needs to be set based on the input clock  frequency to the CAN core and
 * the desired CAN baud rate.
 * This value is for a 40 Kbps baudrate assuming the CAN input clock frequency
 * is 24 MHz.
 */
#define TEST_BRPR_BAUD_PRESCALAR	29

/************************** Variable Definitions *****************************/

/*
 * Buffers to hold frames to send and receive. These are declared as global so
 * that they are not on the stack.
 * These buffers need to be 32-bit aligned
 */
static u32 TxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];
static u32 RxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];

/* Driver instance */
static XCanPs Can[2];
u32 dbg_MaskValue;
u32 dbg_IdValue;


/*-----------------------------------------------------------*/
int init_can(u16 DeviceId, XCanPs *CanInstPtr)
{
  int Status;
  XCanPs_Config *ConfigPtr;

	ConfigPtr = XCanPs_LookupConfig(DeviceId);

	if(CanInstPtr == NULL)
		return XST_FAILURE;

	Status = XCanPs_CfgInitialize(CanInstPtr, ConfigPtr, ConfigPtr->BaseAddr);

	if(Status != XST_SUCCESS)
		return XST_FAILURE;

	Status = XCanPs_SelfTest(CanInstPtr);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;

	XCanPs_SetBaudRatePrescaler(CanInstPtr, TEST_BRPR_BAUD_PRESCALAR);
	XCanPs_SetBitTiming(CanInstPtr, TEST_BTR_SYNCJUMPWIDTH, TEST_BTR_SECOND_TIMESEGMENT, TEST_BTR_FIRST_TIMESEGMENT);

  return EXIT_SUCCESS;
}

/*****************************************************************************/
/**
*
* Send a CAN frame.
*
* @param	InstancePtr is a pointer to the driver instance
*
* @return	XST_SUCCESS if successful, a driver-specific return code if not.
*
* @note
*
* This function waits until TX FIFO has room for at least one frame before
* sending a frame. So this function may block if the hardware is not built
* correctly.
*
******************************************************************************/
static int SendFrame(XCanPs *InstancePtr)
{
	u8 *FramePtr;
	int Index;
	int Status;

	TxFrame[0] = ELMO_IDVALUE;
	TxFrame[1] = (u32)XCanPs_CreateDlcValue((u32)FRAME_DATA_LENGTH);

  PRINTF_MACRO( "@@@ | TxFrame[0] = %8x \r\n", TxFrame[0]);

	FramePtr = (u8 *)(&TxFrame[2]);
	for (Index = 0; Index < FRAME_DATA_LENGTH; Index++) {
		*FramePtr++ = (u8)Index;
	}

	while (XCanPs_IsTxFifoFull(InstancePtr) == TRUE);

	return XCanPs_Send(InstancePtr, TxFrame);
}

/*****************************************************************************/
/**
*
* This function receives a frame and verifies its contents.
*
* @param	InstancePtr is a pointer to the driver instance.
*
* @return	XST_SUCCESS if successful, a driver-specific return code if not.
*
* @note
*
* This function waits until RX FIFO becomes not empty before reading a frame
* from it. So this function may block if the hardware is not built
* correctly.
*
******************************************************************************/
static int RecvFrame(XCanPs *InstancePtr)
{
	u8 *FramePtr;
	int Status;

  PRINTF_MACRO( "XCanPs_Recv() | Wait until a frame is received. \r\n");
	while (XCanPs_IsRxEmpty(InstancePtr) == TRUE);

	Status = XCanPs_Recv(InstancePtr, RxFrame);
  PRINTF_MACRO( "XCanPs_Recv() | %d | \r\n", Status );

	if (Status == XST_SUCCESS) {
    PRINTF_MACRO( "XCanPs_Recv() | Verify Identifier and Data Length Code \r\n");
    PRINTF_MACRO( "XCanPs_Recv() | %x || %x |\r\n", RxFrame[0], ELMO_IDVALUE);

		if( RxFrame[0] != ELMO_IDVALUE)
    {
      PRINTF_MACRO( "XST_LOOPBACK_ERROR | 0 | \r\n" );
			return XST_LOOPBACK_ERROR;
    }

		if ((RxFrame[1] & ~XCANPS_DLCR_TIMESTAMP_MASK) != TxFrame[1])
    {
      PRINTF_MACRO( "XST_LOOPBACK_ERROR | 1 | \r\n" );
			return XST_LOOPBACK_ERROR;
    }

		/*
		 * Verify Data field contents.
		 */
    PRINTF_MACRO( "XCanPs_Recv() | Verify Data field contents \r\n");

		FramePtr = (u8 *)(&RxFrame[2]);
    PRINTF_MACRO("$$$ [");
		for (int i = 0; i < FRAME_DATA_LENGTH; i++)
    {
      PRINTF_MACRO("%2X ", *FramePtr);

			if (*FramePtr++ != (u8)i)
      {
        PRINTF_MACRO( "XST_LOOPBACK_ERROR | 2 | \r\n" );
				return XST_LOOPBACK_ERROR;
      }
		}

    PRINTF_MACRO("]\r\n");
	}

	return Status;
}

/*-----------------------------------------------------------*/
char func_can( const unsigned char argc, const char *argv[] )
{
  int Status;
	XCanPs *CanInstPtr[2];
	CanInstPtr[0] = &Can[0];
	CanInstPtr[1] = &Can[1];

  // unsigned int value = strtoul( argv[2], (char **)NULL, 16 );

  PRINTF_MACRO( "running func_can\r\n" );

  // for(int i = 0; i < 5; i++ )
  // {
    // sleep(1);
    // PRINTF_MACRO("... sleep test[%d]\r\n", i);
  // }

  Status = init_can(XPAR_PS7_CAN_0_DEVICE_ID, CanInstPtr[0]);
	if (Status != XST_SUCCESS) {
    PRINTF_MACRO( "init_can() FAILED!!!\r\n" );
		return Status;
	}

	XCanPs_EnterMode(CanInstPtr[0], XCANPS_MODE_NORMAL);
	while(XCanPs_GetMode(CanInstPtr[0]) != XCANPS_MODE_NORMAL);

  Status = init_can(XPAR_PS7_CAN_1_DEVICE_ID, CanInstPtr[1]);
	if (Status != XST_SUCCESS) {
    PRINTF_MACRO( "init_can() FAILED!!!\r\n" );
		return Status;
	}

	XCanPs_EnterMode(CanInstPtr[1], XCANPS_MODE_NORMAL);
	while(XCanPs_GetMode(CanInstPtr[1]) != XCANPS_MODE_NORMAL);

	Status = SendFrame(CanInstPtr[0]);
	if (Status != XST_SUCCESS) {
    PRINTF_MACRO( "SendFrame() FAILED!!!\r\n" );
		return Status;
	}

  XCanPs_AcceptFilterDisable(CanInstPtr[1], XCANPS_AFR_UAF1_MASK);
  while( XCanPs_IsAcceptFilterBusy(CanInstPtr[1]) );

  Status = XCanPs_AcceptFilterSet(CanInstPtr[1], XCANPS_AFR_UAF1_MASK, 0xFFF80000, ELMO_IDVALUE );
	if (Status != XST_SUCCESS) {
    PRINTF_MACRO( "XCanPs_AcceptFilterDisable() FAILED!!!\r\n" );
		return Status;
	}

  while( XCanPs_IsAcceptFilterBusy(CanInstPtr[1]) );
  XCanPs_AcceptFilterEnable(CanInstPtr[1], XCANPS_AFR_UAF1_MASK);

  XCanPs_AcceptFilterGet(CanInstPtr[1], XCANPS_AFR_UAF1_MASK, &dbg_MaskValue, &dbg_IdValue);
  PRINTF_MACRO( "@@@ | MaskValue = %8x | IdValue = %8x |\r\n", dbg_MaskValue,  dbg_IdValue );

	Status = RecvFrame(CanInstPtr[1]);
	if (Status != XST_SUCCESS) {
    PRINTF_MACRO( "RecvFrame() FAILED!!!\r\n" );
		return Status;
	}

  XCanPs_Reset(CanInstPtr[0]);
  XCanPs_Reset(CanInstPtr[1]);

  return EXIT_SUCCESS;
}

