/**
 * \file Cpu0_Main.c
 * \brief Main function definition for Cpu core 0 .
 *
 * \copyright Copyright (c) 2012 Infineon Technologies AG. All rights reserved.
 *
 *
 *
 *                                 IMPORTANT NOTICE
 *
 *
 * Infineon Technologies AG (Infineon) is supplying this file for use
 * exclusively with Infineon's microcontroller products. This file can be freely
 * distributed within development tools that are supporting such microcontroller
 * products.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 */

#include "Tricore/Cpu/Std/Ifx_Types.h"
#include "Tricore/Cpu/Std/IfxCpu_Intrinsics.h"
#include "Tricore/Scu/Std/IfxScuWdt.h"
#include <Stm/Std/IfxStm.h>
#include <Port/Std/IfxPort.h>
#include "Test_Irq.h"
#include "Test_ModuleInit.h"


#include "Test_Pwm.h"
#include "Test_Adc.h"
#include "Test_Can.h"
#include "TLF35584Demo.h"

#define CENTER          ((uint16)0x270U)
#define LEFT            ((uint16)0x210U)
#define RIGHT           ((uint16)0x2D0U)

extern void Can_Test(void);
extern void Uart_Test(void);
extern void Uart_Initialization(void);

volatile uint16 inittest = 0;
volatile uint16 initStartCount = 0;

void FrontControl(uint16 Angle){


	Pwm_StepDutyUpdate(IfxGtm_TOM1_0_TOUT32_P33_10_OUT,Angle);

}


void CarRuning(uint16 Speed,uint16 Direction){

	Pwm_MotorDutyAndDirectionControl(Speed, Direction);

}

void RcCarInit(void){
	 uint16 i = 0;

	 FrontControl(CENTER);

	 Delay_ms(2000);

	 FrontControl(LEFT);

	 Delay_ms(2000);

	 FrontControl(RIGHT);

	 Delay_ms(2000);

	 FrontControl(CENTER);

	 Delay_ms(2000);


    for(i=0;i<800;i++){

    CarRuning(i, 1);
    Delay_ms(4);
    }

    for(i=800;i>1;i--){

    CarRuning(i, 1);
    Delay_ms(4);
    }

    i =0;

    for(i=0;i<850;i++){

    CarRuning(i, 0);
    Delay_ms(4);
    }

    for(i=850;i>1;i--){

    CarRuning(i, 0);
    Delay_ms(4);
    }



}
void core0_main (void)
{
    __enable ();
    Uart_Initialization();
    /*
     * !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
     * Enable the watchdog in the demo if it is required and also service the watchdog periodically
     * */
    IfxScuWdt_disableCpuWatchdog (IfxScuWdt_getCpuWatchdogPassword ());
    IfxScuWdt_disableSafetyWatchdog (IfxScuWdt_getSafetyWatchdogPassword ());

    Test_ModuleInit();
    //----------------------
    int i;
    for(i=0;i<800;i++){

    CarRuning(i, 1);
    Delay_ms(4);
    }
    //----------------------

    while (1)
    {

    	//CarRuning(800, 1);
    	  //  Delay_ms(4);

    	    //Uart_Test();


    	if(inittest==1){
    		RcCarInit();

    	inittest=0;
    	initStartCount = 0;
    	}

    }

}

void SecondTimer_Initialization(void)
{
    volatile float       stm_freq;
    Ifx_STM             *stm = &MODULE_STM0;
    IfxStm_CompareConfig stmCompareConfig;

    /* suspend by debugger enabled */
    IfxStm_enableOcdsSuspend(stm);
    /* get current STM frequency : debug purpose only*/
    stm_freq = IfxScuCcu_getStmFrequency();
    /* constructor of configuration */
    IfxStm_initCompareConfig(&stmCompareConfig);
	stmCompareConfig.triggerPriority = ISR_PRIORITY_STM;
	stmCompareConfig.ticks = 100000000;
	stmCompareConfig.typeOfService = IfxSrc_Tos_cpu0;
    /* Now Compare functionality is initialized */
    IfxStm_initCompare(stm, &stmCompareConfig);

} // End of TaskScheduler_Initialization()

//*********************************************************************************************
// @Function	 	void UsrIsr_Stm_0(void)
// @Description   	STM0 Interrupt for system tick generation
// @Returnvalue		None
// @Parameters    	None
//*********************************************************************************************
IFX_INTERRUPT (SecondTimer_Isr, 0, ISR_PRIORITY_STM);
void SecondTimer_Isr(void)
{
    Ifx_STM *stm = &MODULE_STM0;

    /* Set next 1ms scheduler tick alarm */
    IfxStm_updateCompare(stm, IfxStm_Comparator_0, IfxStm_getLower(stm) + 100000000);


    P13_OUT.B.P3 = !P13_OUT.B.P3;
    P13_OUT.B.P1 = !P13_OUT.B.P1;


    initStartCount++;

	Test_VadcAutoScan(IfxVadc_GroupId_0);
	Test_VadcAutoScan(IfxVadc_GroupId_1);

    if(initStartCount==3){
	    Can_Test();

    }
    if(initStartCount==4){
        Uart_Test();

    }


    if(initStartCount>5){

    	inittest = 1;
    	initStartCount = 0;

    }


    __enable();

}




