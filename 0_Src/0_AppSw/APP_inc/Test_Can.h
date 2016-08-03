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
**  FILENAME  : Test_CAN.h                                                    **
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

#ifndef TEST_CAN_H
#define TEST_CAN_H

#include "Platform_Types.h"
#include "IfxMultican_Can.h"

#define NUMBER_OF_CAN_CHANNEL	4

typedef struct
{
	IfxMultican_MsgObjId	MessageObject;
	uint32					Identifier;
	uint8					DataLength;
	uint32					DataHigh;
	uint32					DataLow;

} CAN_MESSAGE_OBJECT;

/** \brief Asc information */
typedef struct
{

        IfxMultican_Can        can;          /**< \brief CAN driver handle */
        IfxMultican_Can_Node   can0Node;   /**< \brief CAN Source Node */
        IfxMultican_Can_Node   can1Node;   /**< \brief CAN Source Node */
        IfxMultican_Can_Node   can2Node;   /**< \brief CAN Source Node */
        IfxMultican_Can_Node   can3Node;   /**< \brief CAN Source Node */

        IfxMultican_Can_MsgObj can0TxMsgObj_A; /**< \brief CAN Destination Message object */
        IfxMultican_Can_MsgObj can1TxMsgObj; /**< \brief CAN Source Message object */
        IfxMultican_Can_MsgObj can2TxMsgObj; /**< \brief CAN Destination Message object */
        IfxMultican_Can_MsgObj can3TxMsgObj; /**< \brief CAN Destination Message object */

        IfxMultican_Can_MsgObj can0RxMsgObj_A; /**< \brief CAN Source Message object */
        IfxMultican_Can_MsgObj can1RxMsgObj; /**< \brief CAN Destination Message object */
        IfxMultican_Can_MsgObj can2RxMsgObj; /**< \brief CAN Destination Message object */
        IfxMultican_Can_MsgObj can3RxMsgObj; /**< \brief CAN Destination Message object */

        IfxMultican_Can_MsgObj can0TxMsgObj_B; /**< \brief CAN Source Message object */
        IfxMultican_Can_MsgObj can0TxMsgObj_C; /**< \brief CAN Destination Message object */

        IfxMultican_Can_MsgObj can0RxMsgObj_B; /**< \brief CAN Destination Message object */
        IfxMultican_Can_MsgObj can0RxMsgObj_C; /**< \brief CAN Destination Message object */


} App_MulticanBasic;

IFX_EXTERN App_MulticanBasic g_MulticanBasic;

typedef struct Can_PduType
{
  uint8 swPduHandle;    /* SW Handle that will identify the request in
                               the confirmation callback function. */
  uint8 length;             /* DLC */
  uint32 id;            /* Identifier of L-PDU */
  uint8 *sdu;               /* Pointer to L-SDU */
} Can_PduType;


extern uint8  Can_TxIndex;

extern CAN_MESSAGE_OBJECT	CAN_MO_RX[NUMBER_OF_CAN_CHANNEL];
extern CAN_MESSAGE_OBJECT	CAN_MO_TX[NUMBER_OF_CAN_CHANNEL];
extern void canMsgObj_init(IfxMultican_Can_Node* msgobj,IfxMultican_MsgObjId id,uint32 num ,IfxMultican_Frame frame_type,IfxMultican_Can_MsgObj* obj,uint16 priority);
extern void Can_Recieve(uint8 MoNum, uint8 DataLength,IfxMultican_Can_MsgObj* obj);


#endif /* TEST_CAN_H */
