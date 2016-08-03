/*
 * Test_Pwm.c
 *
 *  Created on: 2015. 3. 24.
 *      Author: kimwayne
 */

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include <stdio.h>
#include "Test_Pwm.h"
#include "Test_Irq.h"

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/

App_GtmTomPwmHl GtmTomPwmHl; /**< \brief Demo information */
App_GtmTomPwmHl GtmTomPwmHlDCmotor; /**< \brief Demo information */
App_GtmTomTimer GtmTomTimer; /**< \brief Demo information */
App_GtmTomTimer GtmTomMsTimer; /**< \brief Demo information */
App_GtmTomPwmHl GtmTomValve; /**< \brief Demo information */

uint16	TestDuty;
unsigned int	Gpt_TimerOverflow;
unsigned int	Gpt_MsTimerOverflow;
/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*------------------------Private Variables/Constants-------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/

/** \name Interrupts for Timer
 * \{ */

IFX_INTERRUPT(Gpt_Notification_SystemTick_1us, 0, ISR_PRIORITY_TIMER_1us);
void Gpt_Notification_SystemTick_1us(void)
{
    IfxCpu_enableInterrupts();

    IfxGtm_Tom_Timer_acknowledgeTimerIrq(&GtmTomValve.drivers.timer);
    Gpt_TimerOverflow++;
   // IfxPort_togglePin(&MODULE_P33, 13);

}


void Delay_ms(uint32 DelayTime)
{

	Gpt_MsTimerOverflow = 0;
    IfxGtm_Tom_Timer_run(&GtmTomMsTimer.drivers.timerOneMs);

    while(Gpt_MsTimerOverflow < (DelayTime*10)){

    	__nop();

    }

    IfxGtm_Tom_Timer_stop(&GtmTomMsTimer.drivers.timerOneMs);

}

/** \name Interrupts for Timer
 * \{ */

IFX_INTERRUPT(Gpt_Notification_SystemTick_1ms, 0, ISR_PRIORITY_TIMER_1ms);
void Gpt_Notification_SystemTick_1ms(void)
{
    IfxCpu_enableInterrupts();

    IfxGtm_Tom_Timer_acknowledgeTimerIrq(&GtmTomMsTimer.drivers.timerOneMs);
    Gpt_MsTimerOverflow++;
   // IfxPort_togglePin(&MODULE_P33, 13);

}




void Pwm_TestDuty(uint16 maxDuty){

	uint16 increment = 1;
    uint16 DutyLimit = 0;

    static uint8	direction = 0;	// 0: increase 1: decrease


    DutyLimit =  (FREQ_20KHZ * maxDuty)/100;

    if(direction == 0 && TestDuty < DutyLimit)
    {
    	TestDuty += increment;
    }
    else if(direction == 0 && TestDuty == DutyLimit)
    {
    	direction = 1;
    }
    else if(direction == 1 && TestDuty > 0)
    {
    	TestDuty -= increment;
    }
    else if(direction == 1 && TestDuty == 0)
    {
    	direction = 0;
    }

}

void Pwm_MotorDutyAndDirectionControl(uint16 DutyCycle, uint8 Direction)
{
	/* set Inhibit pin high active */
	P00_OUT.B.P0 = 1;
	P00_OUT.B.P1 = 1;

//	P33_OUT.B.P3 = 1;
//	P33_OUT.B.P4 = 1;

	/* motor rev direction set up */
	switch(Direction)
	{
	case 0:          // CW
		Pwm_DutyUpdate(IfxGtm_TOM0_9_TOUT11_P00_2_OUT   ,DutyCycle);
		Pwm_DutyUpdate(IfxGtm_TOM0_10_TOUT12_P00_3_OUT  ,0);


//		Pwm_DutyUpdate(IfxGtm_TOM0_2_TOUT28_P33_6_OUT   ,DutyCycle);
//		Pwm_DutyUpdate(IfxGtm_TOM0_1_TOUT27_P33_5_OUT  ,0);

		break;
	case 1:         // CCW
		Pwm_DutyUpdate(IfxGtm_TOM0_9_TOUT11_P00_2_OUT   ,0);
		Pwm_DutyUpdate(IfxGtm_TOM0_10_TOUT12_P00_3_OUT  ,DutyCycle);

//		Pwm_DutyUpdate(IfxGtm_TOM0_2_TOUT28_P33_6_OUT   ,0);
//		Pwm_DutyUpdate(IfxGtm_TOM0_1_TOUT27_P33_5_OUT  ,DutyCycle);

		break;
	default:
		break;
	}
}

void Pwm_DutyUpdate(IfxGtm_Tom_ToutMap ChannelInfo, uint16 DutyCycle)
{
    IfxGtm_Tom_Timer *timer = &GtmTomPwmHlDCmotor.drivers.timer;
	uint16 CenterAlign, ComparePeriod, CompareDuty;
	Ifx_GTM_TOM_CH*   pTomCh;

	CenterAlign = DutyCycle;

	ComparePeriod = (FREQ_20KHZ+ CenterAlign)/2 ;
	CompareDuty   = (FREQ_20KHZ - CenterAlign)/2;

    /* Special handling due to GTM issue */
                      /* 100% duty cycle */
    if (CompareDuty == 0&& (CenterAlign != 4999))
    {
    	CompareDuty = 1;
    	ComparePeriod = ComparePeriod + 2;
    }

	IfxGtm_Tom_Timer_disableUpdate(timer);

	pTomCh = (Ifx_GTM_TOM_CH *)(0xF0108000+(0x800*(ChannelInfo.tom)) + 0x40*(ChannelInfo.channel));
	pTomCh->SR0.U = ComparePeriod;
	pTomCh->SR1.U = CompareDuty;

	IfxGtm_Tom_Timer_applyUpdate(timer);

} /* Pwm_MotorDrive */

void Pwm_ValveDutyUpdate(IfxGtm_Tom_ToutMap ChannelInfo,uint16 DutyCycle)
{
    IfxGtm_Tom_Timer *timer = &GtmTomValve.drivers.timer;
	Ifx_GTM_TOM_CH*   pTomCh;

	IfxGtm_Tom_Timer_disableUpdate(timer);

	pTomCh = (Ifx_GTM_TOM_CH *)(0xF0108000+(0x800*(ChannelInfo.tom)) + 0x40*(ChannelInfo.channel));
	pTomCh->SR0.U = FREQ_20KHZ;
	pTomCh->SR1.U = DutyCycle;

	IfxGtm_Tom_Timer_applyUpdate(timer);

} /* Pwm_MotorDrive */

void Pwm_StepDutyUpdate(IfxGtm_Tom_ToutMap ChannelInfo,uint16 DutyCycle)
{
    IfxGtm_Tom_Timer *timer = &GtmTomValve.drivers.timer;
	Ifx_GTM_TOM_CH*   pTomCh;

	IfxGtm_Tom_Timer_disableUpdate(timer);

	pTomCh = (Ifx_GTM_TOM_CH *)(0xF0108000+(0x800*(ChannelInfo.tom)) + 0x40*(ChannelInfo.channel));
	pTomCh->SR0.U = 0x2048;
	pTomCh->SR1.U = DutyCycle;

	IfxGtm_Tom_Timer_applyUpdate(timer);

} /* Pwm_MotorDrive */


void GtmTomPwmHl_initTimer_DCmotor(void)
{
    {   /* GTM TOM configuration */
        IfxGtm_Tom_Timer_Config timerConfig;
        IfxGtm_Tom_PwmHl_Config pwmHlConfig;

        IfxGtm_Tom_Timer_initConfig(&timerConfig, &MODULE_GTM);
        timerConfig.base.frequency                  = 20000;  //20Khz
       // timerConfig.base.isrPriority                = ISR_PRIORITY_TIMER;	//For Interrupt Enable
        timerConfig.base.isrPriority                = 0;
        timerConfig.base.isrProvider                = 0;
        timerConfig.base.minResolution              = (1.0 / timerConfig.base.frequency) / 1000;
        timerConfig.base.trigger.enabled            = FALSE;
        timerConfig.tom                             = IfxGtm_Tom_0;
        timerConfig.timerChannel                    = IfxGtm_Tom_Ch_0;
        timerConfig.clock                           = IfxGtm_Cmu_Clk_0;

        timerConfig.triggerOut                      = &IfxGtm_TOM0_0_TOUT18_P00_9_OUT;
        timerConfig.base.trigger.outputEnabled      = TRUE;
        timerConfig.base.trigger.enabled            = FALSE;	//For Trigger Signal Check via Oscilloscope
        timerConfig.base.trigger.triggerPoint       = 2500;
        timerConfig.base.trigger.risingEdgeAtPeriod = TRUE;

        IfxGtm_Tom_Timer_init(&GtmTomPwmHlDCmotor.drivers.timer, &timerConfig);

        IfxGtm_Tom_ToutMapP ccx[2]   = {&IfxGtm_TOM0_9_TOUT11_P00_2_OUT,
        								&IfxGtm_TOM0_10_TOUT12_P00_3_OUT,
//        								&IfxGtm_TOM1_8_TOUT0_P02_0_OUT,
//        								&IfxGtm_TOM1_10_TOUT2_P02_2_OUT
        								};

        IfxGtm_Tom_ToutMapP coutx[2] = {&IfxGtm_TOM0_1_TOUT27_P33_5_OUT,
        		                        &IfxGtm_TOM0_2_TOUT28_P33_6_OUT,
//        		                        &IfxGtm_TOM1_9_TOUT1_P02_1_OUT,
//        		                        &IfxGtm_TOM1_11_TOUT3_P02_3_OUT
        		                        };

        pwmHlConfig.timer                 = &GtmTomPwmHlDCmotor.drivers.timer;
        pwmHlConfig.tom                   = timerConfig.tom;
//      pwmHlConfig.base.deadtime         = 1e-8;
//      pwmHlConfig.base.minPulse         = 0;
        pwmHlConfig.base.channelCount     = 2;
        pwmHlConfig.base.emergencyEnabled = FALSE;
        pwmHlConfig.base.outputMode       = IfxPort_OutputMode_pushPull;
        pwmHlConfig.base.outputDriver     = IfxPort_PadDriver_cmosAutomotiveSpeed1;
        pwmHlConfig.base.ccxActiveState   = Ifx_ActiveState_high;
        pwmHlConfig.base.coutxActiveState = Ifx_ActiveState_low;
        pwmHlConfig.ccx                   = ccx;
        pwmHlConfig.coutx                 = coutx;

        IfxGtm_Tom_PwmHl_init(&GtmTomPwmHlDCmotor.drivers.pwm, &pwmHlConfig);

        GTM_TOM0_TGC0_OUTEN_CTRL.U = 0x0000AAAA;
        GTM_TOM0_TGC0_OUTEN_STAT.U = 0x0000AAAA;
        GTM_TOM0_TGC0_ENDIS_CTRL.U = 0x0000AAAA;
        GTM_TOM0_TGC0_ENDIS_STAT.U = 0x0000AAAA;

        GTM_TOM0_TGC1_OUTEN_STAT.U = 0x0000AAAA;
        GTM_TOM0_TGC1_OUTEN_CTRL.U = 0x0000AAAA;
        GTM_TOM0_TGC1_ENDIS_CTRL.U = 0x0000AAAA;
        GTM_TOM0_TGC1_ENDIS_STAT.U = 0x0000AAAA;

        GTM_TOM0_TGC0_GLB_CTRL.U = 0xAAAA0000;
        GTM_TOM0_TGC1_GLB_CTRL.U = 0xAAAA0000;

     //   IfxGtm_Tom_Timer_run(&GtmTomPwmHlDCmotor.drivers.timer);

    }
}
void GtmTomTimer_initTimer(void)
{
    {   /* GTM TOM configuration */
        IfxGtm_Tom_Timer_Config timerConfig;
        IfxGtm_Tom_Timer_initConfig(&timerConfig, &MODULE_GTM);
        timerConfig.base.frequency       = 10000;
        timerConfig.base.isrPriority     = 0;
        timerConfig.base.isrProvider     = 0;
        timerConfig.base.minResolution   = (1.0 / timerConfig.base.frequency) / 1000;
        timerConfig.base.trigger.enabled = FALSE;

        //IfxGtm_TOM1_0_TOUT32_P33_10_OUT
        timerConfig.tom                  = IfxGtm_Tom_1;
        timerConfig.timerChannel         = IfxGtm_Tom_Ch_0;
        timerConfig.clock                = IfxGtm_Tom_Ch_ClkSrc_cmuFxclk2;
        timerConfig.triggerOut           = &IfxGtm_TOM1_0_TOUT32_P33_10_OUT;

        timerConfig.base.trigger.outputEnabled      = TRUE;
        timerConfig.base.trigger.enabled            = TRUE;
        timerConfig.base.trigger.triggerPoint       = 0;
        timerConfig.base.trigger.risingEdgeAtPeriod = TRUE;

        IfxGtm_Tom_Timer_init(&GtmTomValve.drivers.timer, &timerConfig);
        GTM_TOM1_CH0_SR0.B.SR0 = 	24600; //1500;			// 1500 -> 1.5ms

    }
}

void GtmTomTimer_initMsTimer(void)
{
    {   /* GTM TOM configuration */
        IfxGtm_Tom_Timer_Config MstimerConfig;
        IfxGtm_Tom_Timer_initConfig(&MstimerConfig, &MODULE_GTM);
        MstimerConfig.base.frequency       = 100000;
        MstimerConfig.base.isrPriority     = ISR_PRIORITY_TIMER_1ms;
        MstimerConfig.base.isrProvider     = 0;
        MstimerConfig.base.minResolution   = (1.0 / MstimerConfig.base.frequency) / 1000;
        MstimerConfig.base.trigger.enabled = FALSE;

        MstimerConfig.tom                  = IfxGtm_Tom_0;
        MstimerConfig.timerChannel         = IfxGtm_Tom_Ch_14;
        MstimerConfig.clock                = IfxGtm_Cmu_Fxclk_0;

        IfxGtm_Tom_Timer_init(&GtmTomMsTimer.drivers.timerOneMs, &MstimerConfig);
        GTM_TOM0_CH14_SR0.B.SR0 = 	10000; //1500;			// 1500 -> 1.5ms
        IfxGtm_Tom_Timer_run(&GtmTomMsTimer.drivers.timerOneMs);

    }

}


void GtmTomValve_init(void){


    IfxGtm_Tom_Timer_Config timerConfig;
   // IfxGtm_Tom_PwmHl_Config pwmHlConfig;

    IfxGtm_Tom_Timer_initConfig(&timerConfig, &MODULE_GTM);
    timerConfig.base.frequency                  = 1600000;  //20Khz
    timerConfig.base.isrPriority                = ISR_PRIORITY_TIMER_1us;	//For Interrupt Enable
   // timerConfig.base.isrPriority                = 0;
    timerConfig.base.isrProvider                = 0;
    timerConfig.base.minResolution              = (1.0 / timerConfig.base.frequency) / 1000;
    timerConfig.tom                             = IfxGtm_Tom_1;
    timerConfig.timerChannel                    = IfxGtm_Tom_Ch_0;
    timerConfig.clock                           = IfxGtm_Tom_Ch_ClkSrc_cmuFxclk2;
    timerConfig.triggerOut                      = &IfxGtm_TOM1_0_TOUT32_P33_10_OUT;
    timerConfig.base.trigger.outputEnabled      = TRUE;
    timerConfig.base.trigger.enabled            = TRUE;
    timerConfig.base.trigger.triggerPoint       = 0;
    timerConfig.base.trigger.risingEdgeAtPeriod = TRUE;

    IfxGtm_Tom_Timer_init(&GtmTomValve.drivers.timer, &timerConfig);

    GTM_TOM1_CH0_SR0.B.SR0 = 	0x2048; //8264;
    GTM_TOM1_CH0_SR1.B.SR1 = 	0x270; //624;
    timerConfig.base.trigger.triggerPoint       = 0;

    timerConfig.base.trigger.enabled            = TRUE;	//For Trigger Signal Check via Oscilloscope

//    timerConfig.tom                             = IfxGtm_Tom_2;
//    timerConfig.timerChannel                    = IfxGtm_Tom_Ch_1;
//    timerConfig.triggerOut                      = &IfxGtm_TOM2_1_TOUT54_P21_3_OUT;                                 //&IfxGtm_TOM0_9_TOUT11_P00_2_OUT,
//    IfxGtm_Tom_Timer_init(&GtmTomValve.drivers.timer, &timerConfig);                                               //&IfxGtm_TOM0_10_TOUT12_P00_3_OUT,
                                                                                                                     //&IfxGtm_TOM1_8_TOUT0_P02_0_OUT,
//    timerConfig.tom                             = IfxGtm_Tom_0;                                                      //&IfxGtm_TOM1_10_TOUT2_P02_2_OUT
//    timerConfig.timerChannel                    = IfxGtm_Tom_Ch_9;                                                   //};
//    timerConfig.triggerOut                      = &IfxGtm_TOM0_9_TOUT11_P00_2_OUT;                                   //
//    IfxGtm_Tom_Timer_init(&GtmTomValve.drivers.timer, &timerConfig);                                                 //&IfxGtm_TOM0_1_TOUT27_P33_5_OUT,
//                                                                                                                     //&IfxGtm_TOM0_2_TOUT28_P33_6_OUT,
//                                                                                                                     //&IfxGtm_TOM1_9_TOUT1_P02_1_OUT,
//    timerConfig.tom                             = IfxGtm_Tom_0;                                                      //&IfxGtm_TOM1_11_TOUT3_P02_3_OUT
//    timerConfig.timerChannel                    = IfxGtm_Tom_Ch_10;
//    timerConfig.triggerOut                      = &IfxGtm_TOM0_10_TOUT12_P00_3_OUT;
//    IfxGtm_Tom_Timer_init(&GtmTomValve.drivers.timer, &timerConfig);
//
//
//
//    timerConfig.tom                             = IfxGtm_Tom_0;                                                      //&IfxGtm_TOM1_10_TOUT2_P02_2_OUT
//    timerConfig.timerChannel                    = IfxGtm_Tom_Ch_1;                                                   //};
//    timerConfig.triggerOut                      = &IfxGtm_TOM0_1_TOUT27_P33_5_OUT;                                   //
//    IfxGtm_Tom_Timer_init(&GtmTomValve.drivers.timer, &timerConfig);                                                 //&IfxGtm_TOM0_1_TOUT27_P33_5_OUT,
//                                                                                                                     //&IfxGtm_TOM0_2_TOUT28_P33_6_OUT,
//                                                                                                                     //&IfxGtm_TOM1_9_TOUT1_P02_1_OUT,
//    timerConfig.tom                             = IfxGtm_Tom_0;                                                      //&IfxGtm_TOM1_11_TOUT3_P02_3_OUT
//    timerConfig.timerChannel                    = IfxGtm_Tom_Ch_2;
//    timerConfig.triggerOut                      = &IfxGtm_TOM0_2_TOUT28_P33_6_OUT;
//    IfxGtm_Tom_Timer_init(&GtmTomValve.drivers.timer, &timerConfig);


    IfxGtm_Tom_Timer_run(&GtmTomValve.drivers.timer);

}
/** \brief Demo init API
 *
 * This function is called from main during initialization phase
 */
void Pwm_Initialization(void)
{
    /* disable interrupts */
    boolean  interruptState = IfxCpu_disableInterrupts();

    /** - GTM clocks */
    Ifx_GTM *gtm = &MODULE_GTM;
    GtmTomPwmHl.info.gtmFreq = IfxGtm_Cmu_getModuleFrequency(gtm);
    IfxGtm_enable(gtm);

    /* Set the global clock frequencies */
    IfxGtm_Cmu_setGclkFrequency(gtm, GtmTomPwmHl.info.gtmFreq);
    GtmTomPwmHl.info.gtmGclkFreq = IfxGtm_Cmu_getGclkFrequency(gtm);

    //GTM_CMU_CLK_2_CTRL.B.CLK_CNT = 0x100;
   IfxGtm_Cmu_setClkFrequency(gtm, IfxGtm_Cmu_Clk_0,1000000);

    GtmTomPwmHl.info.gtmCmuClk0Freq = IfxGtm_Cmu_getClkFrequency(gtm, IfxGtm_Cmu_Clk_0, TRUE);
    GtmTomPwmHl.info.stateDeadline  = now();

    /** - Initialise the GTM part */
    GtmTomPwmHl_initTimer_DCmotor();

    /** - Initialise the GTM part */
    GtmTomTimer_initMsTimer();
    /** - Initialise the GTM part */
 //   GtmTomTimer_initTimer();


    /** - Initialise the GTM part */
    GtmTomValve_init();

    /* enable interrupts again */
    IfxCpu_restoreInterrupts(interruptState);

    IfxGtm_Cmu_enableClocks(gtm, IFXGTM_CMU_CLKEN_FXCLK |IFXGTM_CMU_CLKEN_CLK0);

}
