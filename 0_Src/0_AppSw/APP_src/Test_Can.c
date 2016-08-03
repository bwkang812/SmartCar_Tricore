// ------------------------------------- DISCLAIMER -----------------------------------------//
// THE INFORMATION GIVEN IN THE DOCUMENTS (APPLICATION NOTE, SOFTWARE PROGRAM ETC.)
// IS GIVEN AS A HINT FOR THE IMPLEMENTATION OF THE INFINEON TECHNOLOGIES COMPONENT ONLY
// AND SHALL NOT BE REGARDED AS ANY DESCRIPTION OR WARRANTY OF A CERTAIN FUNCTIONALITY,
// CONDITION OR QUALITY OF THE INFINEON TECHNOLOGIES COMPONENT.
// YOU MUST VERIFY ANY FUNCTION DESCRIBED IN THE DOCUMENTS IN THE REAL APPLICATION.
// INFINEON TECHNOLOGIES AG HEREBY DISCLAIMS ANY AND ALL WARRANTIES AND LIABILITIES OF ANY KIND
// (INCLUDING WITHOUT LIMITATION WARRANTIES OF NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS
// OF ANY THIRD PARTY) WITH RESPECT TO ANY AND ALL INFORMATION GIVEN IN THE DOCUMENTS.
// ------------------------------------------------ -----------------------------------------//
/*******************************************************************************
**                                                                            **
** Copyright (C) Infineon Technologies (2013)                                 **
**                                                                            **
** All rights reserved.                                                       **
**                                                                            **
** This document contains proprietary information belonging to Infineon       **
** Technologies. Passing on and copying of this document, and communication   **
** of its contents is not permitted without prior written authorization.      **
**                                                                            **
********************************************************************************
**                                                                            **
**  FILENAME  : Test_CAN.c                                                    **
**                                                                            **
**  VERSION   : 0.0.1                                                         **
**                                                                            **
**  DATE      : 2013-01-17                                                    **
**                                                                            **
**  VARIANT   : VariantPB                                                     **
**                                                                            **
**  PLATFORM  : Aurix                                                         **
**                                                                            **
**  COMPILER  : Tasking/GNU/Diab                                              **
**                                                                            **
**  AUTHOR    : DL-AUTOSAR-Engineering                                        **
**                                                                            **
**  VENDOR    : Infineon Technologies                                         **
**                                                                            **
**  DESCRIPTION  : This file contains                                         **
**                                                                            **
**  SPECIFICATION(S) :                                                        **
**                                                                            **
**  MAY BE CHANGED BY USER [Yes/No]: Yes                                      **
*******************************************************************************/
/*******************************************************************************
**                      Author(s) Identity                                    **
********************************************************************************
**                                                                            **
** Initials     Name                                                          **
** ---------------------------------------------------------------------------**
**	SHW			Sung Hoon, Wayne     										  **
*******************************************************************************/

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include <stdio.h>
#include <Ifx_Types.h>
#include "Multican/Can/IfxMultican_Can.h"
#include "Multican/Std/IfxMultican.h"

#include "Test_Irq.h"
#include "Test_CAN.h"

/*******************************************************************************
**            			Private Macro Definitions                             **
*******************************************************************************/
#define CAN_NODE_0	STD_ON
#define CAN_NODE_1	STD_OFF		// CAN Node 1 is overlapped with ASCLIN0. It can be used only for testing CAN communication with Starter kit.
#define CAN_NODE_2	STD_OFF
#define CAN_NODE_3	STD_OFF

/*******************************************************************************
**                      Private Type Definitions                              **
*******************************************************************************/

/*******************************************************************************
**                      Private Function Declarations                         **
*******************************************************************************/

/*******************************************************************************
**                      Global Constant Definitions                           **
*******************************************************************************/

/*******************************************************************************
**                      Global Variable Definitions                           **
*******************************************************************************/
CAN_MESSAGE_OBJECT	CAN_MO_RX[NUMBER_OF_CAN_CHANNEL];
CAN_MESSAGE_OBJECT	CAN_MO_TX[NUMBER_OF_CAN_CHANNEL];

IfxMultican_Message msg[NUMBER_OF_CAN_CHANNEL];
IfxMultican_Message msg1[NUMBER_OF_CAN_CHANNEL];

Can_PduType Can_Pdu;

uint32 RxDataHigh;
uint32 RxDataLow;

uint32 dataLow = 0;
uint32 dataHigh = 0;

/* Variable for SwPduHandle in CanIf */
uint8  	Can_TxIndex = 0;
uint32 TxNotification[4];
uint32 RxNotification[4];

/* CanIf_RxIndication and CanIf_TxConfirmation counters used in CanIf_Cbk.c */
volatile uint8	Test_RxConfirmCount = 0;
volatile uint8	Test_TxConfirmCount = 0;

/* Transmit data for CAN controller*/
uint8 Can_TestData[][8] =
{
    {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80 },
    {0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71, 0x81 },
    {0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72, 0x82 },
    {0x13, 0x23, 0x33, 0x43, 0x53, 0x63, 0x73, 0x83 },
    {0x14, 0x24, 0x34, 0x44, 0x54, 0x64, 0x74, 0x84 },
    {0x15, 0x25, 0x35, 0x45, 0x55, 0x65, 0x75, 0x85 },
    {0x16, 0x26, 0x36, 0x46, 0x56, 0x66, 0x76, 0x86 },
    {0x17, 0x27, 0x37, 0x47, 0x57, 0x67, 0x77, 0x87 },
    {0x18, 0x28, 0x38, 0x48, 0x58, 0x68, 0x78, 0x88 },
    {0x19, 0x29, 0x39, 0x49, 0x59, 0x69, 0x79, 0x89 },
    {0x1A, 0x2A, 0x3A, 0x4A, 0x5A, 0x6A, 0x7A, 0x8A },
    {0x1B, 0x2B, 0x3B, 0x4B, 0x5B, 0x6B, 0x7B, 0x8B },
    {0x1C, 0x2C, 0x3C, 0x4C, 0x5C, 0x6C, 0x7C, 0x8C },
    {0x1D, 0x2D, 0x3D, 0x4D, 0x5D, 0x6D, 0x7D, 0x8D },
    {0x1E, 0x2E, 0x3E, 0x4E, 0x5E, 0x6E, 0x7E, 0x8E },
    {0x1F, 0x2F, 0x3F, 0x4F, 0x5F, 0x6F, 0x7F, 0x8F }
};

/*******************************************************************************
**                      Private  Constant Definitions                         **
*******************************************************************************/

/*******************************************************************************
**                     Private  Variable Definitions                          **
*******************************************************************************/
App_MulticanBasic Test_Multican; /**< \brief Demo information */

/*******************************************************************************
**                      Global Function Definitions                           **
*******************************************************************************/
void Can_Initialization(void);
void Can_Test(void);

IFX_INTERRUPT (CAN0_TxNotification, 0, ISR_PRIORITY_CAN0_TX);
void CAN0_TxNotification(void)
{

	TxNotification[0]++;

}

IFX_INTERRUPT (CAN0_RxNotification, 0, ISR_PRIORITY_CAN0_RX);
void CAN0_RxNotification(void)
{
    Can_Recieve( 0, 8,&Test_Multican.can0RxMsgObj_A);
	RxNotification[0]++;

}

IFX_INTERRUPT (CAN1_TxNotification, 0, ISR_PRIORITY_CAN1_TX);
void CAN1_TxNotification(void)
{

	TxNotification[1]++;

}

IFX_INTERRUPT (CAN1_RxNotification, 0, ISR_PRIORITY_CAN1_RX);
void CAN1_RxNotification(void)
{
    Can_Recieve(1, 8,&Test_Multican.can1RxMsgObj);
	RxNotification[1]++;

}
IFX_INTERRUPT (CAN2_TxNotification, 0, ISR_PRIORITY_CAN2_TX);
void CAN2_TxNotification(void)
{

	TxNotification[2]++;

}

IFX_INTERRUPT (CAN2_RxNotification, 0, ISR_PRIORITY_CAN2_RX);
void CAN2_RxNotification(void)
{
    Can_Recieve( 2, 8,&Test_Multican.can2RxMsgObj);
	RxNotification[2]++;

}
IFX_INTERRUPT (CAN3_TxNotification, 0, ISR_PRIORITY_CAN3_TX);
void CAN3_TxNotification(void)
{

	TxNotification[3]++;

}

IFX_INTERRUPT (CAN3_RxNotification, 0, ISR_PRIORITY_CAN3_RX);
void CAN3_RxNotification(void)
{
    Can_Recieve( 3, 8,&Test_Multican.can3RxMsgObj);
	RxNotification[3]++;

}



//******************************************************************************
// @Function	 	void CAN_Initialization(void)
// @Description   	Initialize CAN nodes
// @Returnvalue		None
// @Parameters    	None
//******************************************************************************
void Can_Initialization(void)
{
    /* create module config */
    IfxMultican_Can_Config canConfig;
    IfxMultican_Can_initModuleConfig(&canConfig, &MODULE_CAN);

    /* initialize module */
    IfxMultican_Can_initModule(&Test_Multican.can, &canConfig);

    /* create CAN node config */
    IfxMultican_Can_NodeConfig canNodeConfig;
    IfxMultican_Can_Node_initConfig(&canNodeConfig, &Test_Multican.can);

    canNodeConfig.baudrate = 500000;     // 500K

    /* CAN Node 0*/
    {
        canNodeConfig.nodeId    = (IfxMultican_NodeId)((int)IfxMultican_NodeId_0);
        canNodeConfig.rxPin     = &IfxMultican_RXD0B_P20_7_IN;
        canNodeConfig.rxPinMode = IfxPort_InputMode_pullUp;
        canNodeConfig.txPin     = &IfxMultican_TXD0_P20_8_OUT;
        canNodeConfig.txPinMode = IfxPort_OutputMode_pushPull;

        IfxMultican_Can_Node_init(&Test_Multican.can0Node, &canNodeConfig);
    }
#if(CAN_NODE_0 == STD_ON && CAN_NODE_1 == STD_ON)

    /* CAN Node 1*/
    {
        canNodeConfig.nodeId    = (IfxMultican_NodeId)((int)IfxMultican_NodeId_1);
        canNodeConfig.rxPin     = &IfxMultican_RXD1B_P14_1_IN;
        canNodeConfig.rxPinMode = IfxPort_InputMode_pullUp;
        canNodeConfig.txPin     = &IfxMultican_TXD1_P14_0_OUT;
        canNodeConfig.txPinMode = IfxPort_OutputMode_pushPull;

        IfxMultican_Can_Node_init(&Test_Multican.can1Node, &canNodeConfig);
    }
#endif
#if(CAN_NODE_2 == STD_ON && CAN_NODE_3 == STD_ON)
    /* CAN Node 2*/
	{
	canNodeConfig.nodeId    = (IfxMultican_NodeId)((int)IfxMultican_NodeId_2);
	canNodeConfig.rxPin     = &IfxMultican_RXD2A_P15_1_IN;
	canNodeConfig.rxPinMode = IfxPort_InputMode_pullUp;
	canNodeConfig.txPin     = &IfxMultican_TXD2_P15_0_OUT;
	canNodeConfig.txPinMode = IfxPort_OutputMode_pushPull;

	IfxMultican_Can_Node_init(&Test_Multican.can2Node, &canNodeConfig);
	}

	/* CAN Node 3*/
	{
	canNodeConfig.nodeId    = (IfxMultican_NodeId)((int)IfxMultican_NodeId_3);
	canNodeConfig.rxPin     = &IfxMultican_RXD3E_P20_9_IN;
	canNodeConfig.rxPinMode = IfxPort_InputMode_pullUp;
	canNodeConfig.txPin     = &IfxMultican_TXD3_P20_10_OUT;
	canNodeConfig.txPinMode = IfxPort_OutputMode_pushPull;

	IfxMultican_Can_Node_init(&Test_Multican.can3Node, &canNodeConfig);
	}

#endif

    canMsgObj_init(&Test_Multican.can0Node,0x100,1,IfxMultican_Frame_transmit,&Test_Multican.can0TxMsgObj_A,ISR_PRIORITY_CAN0_TX);
    canMsgObj_init(&Test_Multican.can0Node,0x110,9,IfxMultican_Frame_transmit,&Test_Multican.can0TxMsgObj_B,ISR_PRIORITY_CAN0_TX);
    canMsgObj_init(&Test_Multican.can0Node,0x120,10,IfxMultican_Frame_transmit,&Test_Multican.can0TxMsgObj_C,ISR_PRIORITY_CAN0_TX);
#if(CAN_NODE_0 == STD_ON && CAN_NODE_1 == STD_ON)
    canMsgObj_init(&Test_Multican.can1Node,0x200,2,IfxMultican_Frame_transmit,&Test_Multican.can1TxMsgObj,ISR_PRIORITY_CAN1_TX);
#if(CAN_NODE_2 == STD_ON && CAN_NODE_3 == STD_ON)
    canMsgObj_init(&Test_Multican.can2Node,0x300,3,IfxMultican_Frame_transmit,&Test_Multican.can2TxMsgObj,ISR_PRIORITY_CAN2_TX);
    canMsgObj_init(&Test_Multican.can3Node,0x400,4,IfxMultican_Frame_transmit,&Test_Multican.can3TxMsgObj,ISR_PRIORITY_CAN3_TX);
#endif
#endif
    canMsgObj_init(&Test_Multican.can0Node,0x200,11,IfxMultican_Frame_receive,&Test_Multican.can0RxMsgObj_A,ISR_PRIORITY_CAN0_RX);
    canMsgObj_init(&Test_Multican.can0Node,0x210,12,IfxMultican_Frame_receive,&Test_Multican.can0RxMsgObj_B,ISR_PRIORITY_CAN0_RX);
    canMsgObj_init(&Test_Multican.can0Node,0x220,13,IfxMultican_Frame_receive,&Test_Multican.can0RxMsgObj_C,ISR_PRIORITY_CAN0_RX);
#if(CAN_NODE_0 == STD_ON && CAN_NODE_1 == STD_ON)
    canMsgObj_init(&Test_Multican.can1Node,0x100,6,IfxMultican_Frame_receive,&Test_Multican.can1RxMsgObj,ISR_PRIORITY_CAN1_RX);
#if(CAN_NODE_2 == STD_ON && CAN_NODE_3 == STD_ON)
    canMsgObj_init(&Test_Multican.can2Node,0x400,7,IfxMultican_Frame_receive,&Test_Multican.can2RxMsgObj,ISR_PRIORITY_CAN2_RX);
    canMsgObj_init(&Test_Multican.can3Node,0x300,8,IfxMultican_Frame_receive,&Test_Multican.can3RxMsgObj,ISR_PRIORITY_CAN3_RX);
#endif
#endif

} /* CAN_Initialization */

void canMsgObj_init(IfxMultican_Can_Node* msgobj,IfxMultican_MsgObjId id,uint32 num ,IfxMultican_Frame frame_type,IfxMultican_Can_MsgObj* obj,uint16 priority)
{

    /* Source Message object  */
    {
        /* create message object config */
        IfxMultican_Can_MsgObjConfig canMsgObjConfig;
        IfxMultican_Can_MsgObj_initConfig(&canMsgObjConfig, msgobj);

        canMsgObjConfig.msgObjId              = num;
        canMsgObjConfig.messageId             = id;
        canMsgObjConfig.acceptanceMask        = 0x7FFFFFFFUL;
        canMsgObjConfig.frame                 = frame_type;
        canMsgObjConfig.control.messageLen    = IfxMultican_DataLengthCode_8;
        canMsgObjConfig.control.extendedFrame = FALSE;
        canMsgObjConfig.control.matchingId    = TRUE;


        /* create message object Interrupt */
        if(frame_type == IfxMultican_Frame_transmit){
        canMsgObjConfig.txInterrupt.enabled = 1;
        canMsgObjConfig.txInterrupt.srcId = canMsgObjConfig.node->nodeId;

        volatile Ifx_SRC_SRCR *srcPointer = IfxMultican_getSrcPointer(msgobj->mcan, canMsgObjConfig.txInterrupt.srcId);
        IfxSrc_init(srcPointer, IfxSrc_Tos_cpu0, priority);
        IfxSrc_enable(srcPointer);

        }
        if(frame_type == IfxMultican_Frame_receive){
        canMsgObjConfig.rxInterrupt.enabled = 1;
        canMsgObjConfig.rxInterrupt.srcId = canMsgObjConfig.node->nodeId + 4;

        volatile Ifx_SRC_SRCR *srcPointer = IfxMultican_getSrcPointer(msgobj->mcan, canMsgObjConfig.rxInterrupt.srcId);
        IfxSrc_init(srcPointer, IfxSrc_Tos_cpu0, priority);
        IfxSrc_enable(srcPointer);

        }
        /* initialize message object */
        IfxMultican_Can_MsgObj_init(obj, &canMsgObjConfig);
    }

};

//******************************************************************************
// @Function	 	void Can_Transmit(void)
// @Description   	Transmit CAN data frame
// @Returnvalue		None
// @Parameters    	SW Index number, Tx MO Number, Rx MO ID, number of bytes to be sent
//******************************************************************************
void Can_Transmit(uint8 swPduHandle, uint8 MoNum, uint32 PduId, uint8 DataLength,IfxMultican_Can_MsgObj* obj)
{
	uint8 i = 0;

	if(DataLength > 8)
	{
		__nop();
	}
	else
	{
		CAN_MO_TX[MoNum].MessageObject = obj->msgObjId;
		CAN_MO_TX[MoNum].Identifier = PduId;
		CAN_MO_TX[MoNum].DataLength = DataLength ;
		for(i=0;i<4;i++){

			dataLow = (dataLow<< (8)) |(Can_TestData[swPduHandle][i]);
			dataHigh = (dataHigh<< (8)) |(Can_TestData[swPduHandle][i+4]);
		}
		CAN_MO_TX[MoNum].DataHigh  = dataHigh;
		CAN_MO_TX[MoNum].DataLow  = dataLow;

        IfxMultican_Message_init(&msg[MoNum], PduId, dataLow, dataHigh, IfxMultican_DataLengthCode_8);

    	// Transmit Data
    	if( IfxMultican_Can_MsgObj_sendMessage(obj, &msg[MoNum]) == IfxMultican_Status_notSentBusy )
    	{
    		//return  USRCAN_TX_FAILED;
    	}
    	else
    	{
    		//return USRCAN_TX_SUCCESS;
    	}

	}

} /* Can_Transmit */

//******************************************************************************
// @Function	 	void Can_Recieve(void)
// @Description   	Recieve CAN data frame
// @Returnvalue		None
// @Parameters    	SW Index number, Tx MO Number, Rx MO ID, number of bytes to be sent
//******************************************************************************
void Can_Recieve(uint8 MoNum, uint8 DataLength,IfxMultican_Can_MsgObj* obj)
{
    /* Receiving Data */
    {
        /* wait until MCAN received the frame */
        while (!IfxMultican_Can_MsgObj_isRxPending(obj))
        {}

		CAN_MO_RX[MoNum].MessageObject = obj->msgObjId;
		CAN_MO_RX[MoNum].Identifier = (*(uint32 *)(0xF0019018+(0x20*(obj->msgObjId))))&0x1FFC0000;
		CAN_MO_RX[MoNum].Identifier = CAN_MO_RX[MoNum].Identifier >> 18;
		CAN_MO_RX[MoNum].DataLength = DataLength ;

       // IfxMultican_Message msg1;
        IfxMultican_Message_init(&msg1[MoNum], 0xdead, 0xdeadbeef, 0xdeadbeef, IfxMultican_DataLengthCode_8); /* start with invalid values */

        IfxMultican_Status  readStatus = IfxMultican_Can_MsgObj_readMessage(obj, &msg1[MoNum]);

		CAN_MO_RX[obj->node->nodeId].DataHigh  = msg1[MoNum].data[0];
		CAN_MO_RX[obj->node->nodeId].DataLow  = msg1[MoNum].data[1];

    }



} /* Can_Transmit */

//******************************************************************************
// @Function	 	void Can_Test(void)
// @Description   	Transmit CAN data frame
// @Returnvalue		None
// @Parameters    	None
//******************************************************************************

void Can_Test(void)
{
	static	uint8	i = 0;

         /* Transmit Data */
	     Can_Transmit(i, 0, 0x100, 8,&Test_Multican.can0TxMsgObj_A);
#if(CAN_NODE_0 == STD_ON && CAN_NODE_1 == STD_ON)
	     Can_Transmit(i, 1, 0x200, 8,&Test_Multican.can1TxMsgObj);
#if(CAN_NODE_2 == STD_ON && CAN_NODE_3 == STD_ON)
	     Can_Transmit(i, 2, 0x300, 8,&Test_Multican.can2TxMsgObj);
	     Can_Transmit(i, 3, 0x400, 8,&Test_Multican.can3TxMsgObj);
#endif
#endif


#if(CAN_NODE_0 == STD_ON && CAN_NODE_1 == STD_ON)
//		 /* Receiving Data */
//	     Can_Recieve(0, 8,&Test_Multican.can0RxMsgObj);		//For Interrupt Test.
//	     Can_Recieve(1, 8,&Test_Multican.can1RxMsgObj);
#if(CAN_NODE_2 == STD_ON && CAN_NODE_3 == STD_ON)
//	     Can_Recieve(2, 8,&Test_Multican.can2RxMsgObj);
//	     Can_Recieve(3, 8,&Test_Multican.can3RxMsgObj);
#endif
#endif


		if(i<0x10)		// Depends on the size of test data array, Can_TestData[16][8]
		{
			i++;
		}
		else
		{
			i=0;
		}

} /* Can_Test */
