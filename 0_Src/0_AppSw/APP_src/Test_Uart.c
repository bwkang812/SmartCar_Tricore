/******************************************************************************
**                                                                           **
** Copyright (C) Infineon Technologies (2013)                                **
**                                                                           **
** All rights reserved.                                                      **
**                                                                           **
** This document contains proprietary information belonging to Infineon      **
** Technologies. Passing on and copying of this document, and communication  **
** of its contents is not permitted without prior written authorization.     **
**                                                                           **
*******************************************************************************
**                                                                           **
**  $FILENAME   : Test_Uart.c $                                              **
**                                                                           **
**  $CC VERSION : \main\16 $                                                 **
**                                                                           **
**  $DATE       : 2013-07-03 $                                               **
**                                                                           **
**  AUTHOR      : DL-AUTOSAR-Engineering                                     **
**                                                                           **
**  VENDOR      : Infineon Technologies                                      **
**                                                                           **
**  DESCRIPTION : This file contains                                         **
**                - sample Demo Application for UART module                  **
**                                                                           **
**  MAY BE CHANGED BY USER [yes/no]: yes                                     **
**                                                                           **
******************************************************************************/
/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include <stdio.h>
#include <Ifx_Types.h>
#include <Asclin/Asc/IfxAsclin_Asc.h>
#include "Test_Irq.h"

#define ASC_TX_BUFFER_SIZE 64
#define ASC_RX_BUFFER_SIZE 64
#define UART_TEST_DATA_SIZE		10U

//****************************************************************************
// @Typedefs
//****************************************************************************
typedef struct
{
    uint8 tx[ASC_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
    uint8 rx[ASC_RX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
} AppAscBuffer;

/** \brief Asc information */
typedef struct
{
    AppAscBuffer ascBuffer;                     /**< \brief ASC interface buffer */
    struct
    {
        IfxAsclin_Asc asc0;                     /**< \brief ASC interface */
        IfxAsclin_Asc asc1;                     /**< \brief ASC interface */
    }         drivers;

    uint8 Uart_TxData[UART_TEST_DATA_SIZE];
    uint8 Uart_RxData[UART_TEST_DATA_SIZE];

	uint16	TxComplete		:16;
	uint16	RxComplete		:16;
	uint16	ErComplete		:16;

    Ifx_SizeT count;
} App_AsclinAsc;

/*******************************************************************************
**                      Private Variable Declarations                         **
*******************************************************************************/

App_AsclinAsc AsclinAsc; /**< \brief Demo information */

/*******************************************************************************
**                      Global Function Definitions                           **
*******************************************************************************/
void Uart_Initialization(void);
void Uart_Test(void);


//******************************************************************************
// @Function	 	void Uart_Initialization(void)
// @Description   	UART initialization for test with StarterKit
// @Returnvalue		None
// @Parameters    	None
//******************************************************************************
void Uart_Initialization(void)
{
	static uint8 i;

	/* disable interrupts */
    boolean              interruptState = IfxCpu_disableInterrupts();
    /* create module config */
    IfxAsclin_Asc_Config Uart_AscLin0;
    IfxAsclin_Asc_initModuleConfig(&Uart_AscLin0, &MODULE_ASCLIN0);

    /* set the desired baudrate */
    Uart_AscLin0.baudrate.prescaler    = 1;
    Uart_AscLin0.baudrate.baudrate     = 115200; /* FDR values will be calculated in initModule */
    Uart_AscLin0.baudrate.oversampling = IfxAsclin_OversamplingFactor_4;

    /* ISR priorities and interrupt target */
    Uart_AscLin0.interrupt.txPriority    = ISR_PRIORITY_ASCLIN0_TX;
    Uart_AscLin0.interrupt.rxPriority    = ISR_PRIORITY_ASCLIN0_RX;
    Uart_AscLin0.interrupt.erPriority    = ISR_PRIORITY_ASCLIN0_ER;
    Uart_AscLin0.interrupt.typeOfService = IfxSrc_Tos_cpu0;

    /* FIFO configuration */
    Uart_AscLin0.txBuffer     = AsclinAsc.ascBuffer.tx;
    Uart_AscLin0.txBufferSize = ASC_TX_BUFFER_SIZE;

    Uart_AscLin0.rxBuffer     = AsclinAsc.ascBuffer.rx;
    Uart_AscLin0.rxBufferSize = ASC_RX_BUFFER_SIZE;

	const IfxAsclin_Asc_Pins pins = {
        NULL,                     IfxPort_InputMode_pullUp,        /* CTS pin not used */
        &IfxAsclin0_RXB_P15_3_IN, IfxPort_InputMode_pullUp,        /* Rx pin */
        NULL,                     IfxPort_OutputMode_pushPull,     /* RTS pin not used */
        &IfxAsclin0_TX_P15_2_OUT, IfxPort_OutputMode_pushPull,     /* Tx pin */
        IfxPort_PadDriver_cmosAutomotiveSpeed1
    };
    Uart_AscLin0.pins = &pins;

	for(i=0; i<UART_TEST_DATA_SIZE; i++)
	{
		AsclinAsc.Uart_TxData[i] = i;
	}

    /* initialize module */
    IfxAsclin_Asc_initModule(&AsclinAsc.drivers.asc0, &Uart_AscLin0);

    /* enable interrupts again */
    IfxCpu_restoreInterrupts(interruptState);



} /* End of Uart_Initialization */


//******************************************************************************
// @Function	 	void Uart_Test(void)
// @Description   	UART communication test
// @Returnvalue		None
// @Parameters    	None
//******************************************************************************
void Uart_Test(void)
{ 

	static	uint8	i=0;

	    /* Transmit data */
	    AsclinAsc.count = UART_TEST_DATA_SIZE;
	    IfxAsclin_Asc_write(&AsclinAsc.drivers.asc0,AsclinAsc.Uart_TxData, &AsclinAsc.count, TIME_INFINITE);

	    /* Receive data */
	    IfxAsclin_Asc_read(&AsclinAsc.drivers.asc0, AsclinAsc.Uart_RxData, &AsclinAsc.count, 0xFF);


		for(i=0; i<UART_TEST_DATA_SIZE; i++)
	   {
		AsclinAsc.Uart_TxData[i] = AsclinAsc.Uart_TxData[i]+1;
		}


}	/* End of Kline_Test */


IFX_INTERRUPT(Uart_AscLin0_TxIsr, 0, ISR_PRIORITY_ASCLIN0_TX)
{
    IfxAsclin_Asc_isrTransmit(&AsclinAsc.drivers.asc0);
    AsclinAsc.TxComplete++;
}

/** \} */

/** \name Interrupts for Receive
 * \{ */

IFX_INTERRUPT(Uart_AscLin0_RxIsr, 0, ISR_PRIORITY_ASCLIN0_RX)
{
    IfxAsclin_Asc_isrReceive(&AsclinAsc.drivers.asc0);
    AsclinAsc.RxComplete++;
}

/** \} */

/** \name Interrupts for Error
 * \{ */

IFX_INTERRUPT(Uart_AscLin0_ErIsr, 0, ISR_PRIORITY_ASCLIN0_ER)
{
    IfxAsclin_Asc_isrError(&AsclinAsc.drivers.asc0);
    AsclinAsc.ErComplete++;

}
