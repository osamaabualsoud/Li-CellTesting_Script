/***************************************************************************//**
* \file     ADC_1.h
* \version  3.10
*
* \brief
* Provides the source code to the API for the ADC_1 Component.
*
********************************************************************************
* \copyright
* (c) 2015-2018, Cypress Semiconductor Corporation. All rights reserved.
* This software, including source code, documentation and related
* materials ("Software"), is owned by Cypress Semiconductor
* Corporation ("Cypress") and is protected by and subject to worldwide
* patent protection (United States and foreign), United States copyright
* laws and international treaty provisions. Therefore, you may use this
* Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, nonexclusive,
* non-transferable license to copy, modify, and compile the
* Software source code solely for use in connection with Cypress's
* integrated circuit products. Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO
* WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING,
* BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE. Cypress reserves the right to make
* changes to the Software without notice. Cypress does not assume any
* liability arising out of the application or use of the Software or any
* product or circuit described in the Software. Cypress does not
* authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

#if !defined(CY_ADC_1_H)
    #define CY_ADC_1_H

/*******************************************************************************
                                  Project Files
*******************************************************************************/
#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"
#include "ADC_1_IRQ.h"

#define ADC_1_VREF_ROUTED                  ((0) == 1u)
#if (ADC_1_VREF_ROUTED)
#include "ADC_1_vrefAMux.h"
#endif

#define ADC_1_CLOCK_INTERNAL               ((1) == 1u)
#if (ADC_1_CLOCK_INTERNAL)
#include "ADC_1_intSarClock.h"
#endif /* ADC_1_CLOCK_INTERNAL */

#define ADC_1_CFG0_FILTER_PRESENT          (0uL)
#define ADC_1_CFG1_FILTER_PRESENT          (0uL)
#define ADC_1_CFG2_FILTER_PRESENT          (0uL)
#define ADC_1_CFG3_FILTER_PRESENT          (0uL)

#define ADC_1_ANY_CONFIG_USES_FILTER       ( ADC_1_CFG0_FILTER_PRESENT \
                                                      | ADC_1_CFG1_FILTER_PRESENT \
                                                      | ADC_1_CFG2_FILTER_PRESENT \
                                                      | ADC_1_CFG3_FILTER_PRESENT \
                                                      )


#if(ADC_1_ANY_CONFIG_USES_FILTER != 0u)
    #include "ADC_1_FILTER.h"
    #include "ADC_1_UABH_A.h"
    #include "ADC_1_UABH_B.h"
    #include "ADC_1_intUabClock.h"
    #include "ADC_1_filterVinMux.h"
    #include "ADC_1_FILTERAGND2SAR_BUFFER.h"
#endif

/**
* \addtogroup group_structures
*
*/
/** Low power Mode API Support */
typedef struct
{
    /** Preserves state of block prior to sleep. */
    uint8   enableState;
}   ADC_1_BACKUP_STRUCT;

typedef struct
{
    uint32 channelBase;    /* Start of channels for the configuration */
    uint32 numChannels;    /* Number of channels in the configuration */
    uint32 ctrl;           /* Contains the initial settings for the CRTL register except for the BOOSTPUMP_EN and
                              ENABLED bits which are set in the ADC_1_Init() ADC_1_Enable()
                              respectively */
    uint32 sampleCtrl;     /* Initial SAMPLE_CTRL register value */
    uint32 sampleTime01;   /* Initial SAMPLE_TIME01 register value */
    uint32 sampleTime23;   /* Initial SAMPLE_TIME23 register value */
    uint32 rangeThres;     /* Initial RANGE_THRES register value */
    uint32 rangeCond;      /* Initial RANGE_COND register value */
    uint32 chanEn;         /* Initial CHAN_EN register value */
    uint32 rangeIntMask;   /* Initial RANGE_INTR_MASK register value */
    uint32 satIntMask;     /* Stores the initial SAT_INTR_MASK register value */
    int32 vrefMvValue;      /* Calculated value of VREF in millivolts. Used in
                               ADC_1_CountsTo*Volts() functions */
    uint32 miscConfig;      /* Miscellaneous configuration bits broken down as follows:
                                [0] - Freerunning: Set if the sample mode is freerunning
                                [1] - Filter Present: Set if the configuration uses a UAB filter
                                [2] - Mux Switch 0: Set when VSSA is used for the neg input to any single-ended channel
                            */

#if (ADC_1_CLOCK_INTERNAL)
    uint16 clkDivider;      /* Clock divider */
#endif /* ADC_1_CLOCK_INTERNAL */

#if(ADC_1_ANY_CONFIG_USES_FILTER != 0u)
    uint16 filterClkDivider;
    uint16 filterTrimDelay;
#endif /* ADC_1_ANY_CONFIG_USES_FILTER   != 0u */
} ADC_1_CONFIG_STRUCT;
/** @} structures */

/*******************************************************************************
*           API Constants
*******************************************************************************/
/**
* \addtogroup group_constants
* {
*/
/** \addtogroup endConversion EOS status
* \brief Parameter constants for the ADC_1_IsEndConversion()
*  function.
*  @{
*/

/** Immediately returns conversion result */
#define ADC_1_RETURN_STATUS              (1u)
/** Doesn't return until ADC conversion is done */
#define ADC_1_WAIT_FOR_RESULT            (2u)

#define ADC_1_CONTINUOUS                 (1u)
#define ADC_1_SINGLE_SHOT                (2u)
typedef enum
{
    VNEGSOURCE_VSSA_K = 0, /**< Vssa */
    VNEGSOURCE_EXTERNAL = 2, /**< External */
    VNEGSOURCE_VREFSHORT = 7 /**< Vref */
} ADC_1_vneg_source_enum;


/* Constants for Sleep mode states */
#define ADC_1_ENABLED                    (0x01u)
#define ADC_1_STARTED                    (0x02u)
#define ADC_1_BOOSTPUMP_ENABLED          (0x04u)
#define ADC_1_SLEEP_DFT_DCEN             (0x08u)
#define ADC_1_SLEEP_DFT_DLY_INC          (0x10u)
/** @} group_constants */

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
void ADC_1_SelectConfig(uint32 config, uint32 restart);
void ADC_1_InitConfig(const ADC_1_CONFIG_STRUCT *config);
void ADC_1_Init(void);
void ADC_1_Enable(void);

CY_ISR_PROTO(ADC_1_Interrupt);

/**
* \addtogroup group_general
* @{
*/
void ADC_1_Start(void);
void ADC_1_StartEx(cyisraddress address);
void ADC_1_Stop(void);
void ADC_1_StartConvert(void);
void ADC_1_StopConvert(void);
void ADC_1_SetConvertMode(uint32 mode);
void ADC_1_SetEosMask(uint32 mask);
uint32 ADC_1_IsEndConversion(uint32 retMode);
int16 ADC_1_GetResult16(uint32 chan);
int32 ADC_1_GetResult32(uint32 chan);
void ADC_1_SetLowLimit(uint32 lowLimit);
void ADC_1_SetHighLimit(uint32 highLimit);
void ADC_1_SetLimitMask(uint32 limitMask);
void ADC_1_SetSatMask(uint32 satMask);
void ADC_1_SetOffset(uint32 chan, int16 offset);
void ADC_1_SetGain(uint32 chan, int32 adcGain);
int16 ADC_1_RawCounts2Counts(uint32 chan, int16 adcCounts);
float32 ADC_1_CountsTo_Volts(uint32 chan, int16 adcCounts);
int16 ADC_1_CountsTo_mVolts(uint32 chan, int16 adcCounts);
int32 ADC_1_CountsTo_uVolts(uint32 chan, int16 adcCounts);

/** @} general */


/**
* \addtogroup group_interrupts
* @{
*/

#define ADC_1_ClearInterrupt() do{ADC_1_SAR_INTR_REG = ADC_1_EOS_MASK;}while(0)

/** @} interrupts */


void ADC_1_SetChanMask(uint32 enableMask);
void ADC_1_SaveConfig(void);
void ADC_1_RestoreConfig(void);

/**
* \addtogroup group_power
* @{
*/
void ADC_1_Sleep(void);
void ADC_1_Wakeup(void);
/** @} power */
CY_ISR_PROTO( ADC_1_ISR );

/*******************************************************************************
*       Type defines
*******************************************************************************/


/*******************************************************************************
                                  Parameters
The parameters that are set in the customizer are redefined as constants here.
*******************************************************************************/

/*  Sample Mode setting constants */
#define ADC_1_TOTAL_CONFIGS                (1ul)

/* ************************************************************************** */
/* Begin configuration 0 customizer defines                                   */
/* ************************************************************************** */

#define ADC_1_CFG0_SAMPLE_RATE             (100000)
#define ADC_1_CFG0_CHANNEL_BASE            (0u)
#define ADC_1_CFG0_CHANNEL_COUNT           (1u)
#define ADC_1_CFG0_VREF_SEL_MASK           (0xc0u)
#define ADC_1_CFG0_NOMINAL_CLOCK_FREQ      (12000000u)
#define ADC_1_CFG0_CHANNEL_EN              (0x1u)
/* Sample Control Register */
#define ADC_1_CFG0_SUB_RESOLUTION          (0u)
#define ADC_1_CFG0_SINGLE_ENDED_FORMAT     (0uL)
#define ADC_1_CFG0_DIFFERENTIAL_FORMAT     (1uL)
#define ADC_1_CFG0_SAMPLES_AVERAGED        (7uL)
#define ADC_1_CFG0_RANGE_INTR_MASK         (0x0u)
#define ADC_1_CFG0_SATURATE_INTR_MASK      (0x0u)
#define ADC_1_CFG0_VNEG_INPUT_SEL          (7)
#define ADC_1_CFG0_SINGLE_PRESENT          (1)
#define ADC_1_CFG0_VREF_MV_VALUE           (1200)
#define ADC_1_CFG0_APERTURE_TIME0          (3uL)
#define ADC_1_CFG0_APERTURE_TIME1          (2uL)
#define ADC_1_CFG0_APERTURE_TIME2          (2uL)
#define ADC_1_CFG0_APERTURE_TIME3          (2uL)
#define ADC_1_CFG0_FREERUNNING             (1u)
#define ADC_1_CFG0_AVGERAGING_MODE         (1uL)
#define ADC_1_CFG0_LOW_LIMIT               (512u)
#define ADC_1_CFG0_HIGH_LIMIT              (3584u)
#define ADC_1_CFG0_RANGE_COND              (0uL)
#define ADC_1_CFG0_USE_SOC                 (0uL)
/* Clock parameters*/
#if (ADC_1_CLOCK_INTERNAL)
    #define ADC_1_CFG0_CLOCKDIVIDER        (((CYDEV_BCLK__HFCLK__HZ) / (ADC_1_CFG0_NOMINAL_CLOCK_FREQ)) - 1u)
#endif /* (ADC_1_CLOCK_INTERNAL) */
/* Filter Parameters */
#if(ADC_1_CFG0_FILTER_PRESENT == 1uL)
    /* Timing settings */
    /* Clock divider register uses 0 for a divider of 1, 1 for a divider of 2, and so on. */
    #define ADC_1_CFG0_FILTERCLOCKDIVIDER  (0u - 1u)
    #define ADC_1_CFG0_FILTERSTARTDELAY    (0u)
    #define ADC_1_CFG0_FILTERTRIMDELAY     (143u)
#endif /* (ADC_1_CFG0_FILTER_PRESENT == 1uL) */

/* ************************************************************************** */
/* End configuration 0 customizer defines                                     */
/* ************************************************************************** */

/* ************************************************************************** */
/* Begin configuration 1 customizer defines                                   */
/* ************************************************************************** */

#if(ADC_1_TOTAL_CONFIGS > 1)
    #define ADC_1_CFG1_SAMPLE_RATE             ()
    #define ADC_1_CFG1_CHANNEL_BASE            (u)
    #define ADC_1_CFG1_CHANNEL_COUNT           (u)
    #define ADC_1_CFG1_VREF_SEL_MASK           (u)
    #define ADC_1_CFG1_NOMINAL_CLOCK_FREQ      (u)
    #define ADC_1_CFG1_CHANNEL_EN              (u)
    /* Sample Control Register */
    #define ADC_1_CFG1_SUB_RESOLUTION          (u)
    #define ADC_1_CFG1_SINGLE_ENDED_FORMAT     (uL)
    #define ADC_1_CFG1_DIFFERENTIAL_FORMAT     (uL)
    #define ADC_1_CFG1_SAMPLES_AVERAGED        (uL)
    #define ADC_1_CFG1_RANGE_INTR_MASK         (u)
    #define ADC_1_CFG1_SATURATE_INTR_MASK      (u)
    #define ADC_1_CFG1_VNEG_INPUT_SEL          ()
    #define ADC_1_CFG1_SINGLE_PRESENT          ()
    #define ADC_1_CFG1_VREF_MV_VALUE           ()
    #define ADC_1_CFG1_APERTURE_TIME0          (uL)
    #define ADC_1_CFG1_APERTURE_TIME1          (uL)
    #define ADC_1_CFG1_APERTURE_TIME2          (uL)
    #define ADC_1_CFG1_APERTURE_TIME3          (uL)
    #define ADC_1_CFG1_FREERUNNING             (u)
    #define ADC_1_CFG1_AVGERAGING_MODE         (uL)
    #define ADC_1_CFG1_LOW_LIMIT               (u)
    #define ADC_1_CFG1_HIGH_LIMIT              (u)
    #define ADC_1_CFG1_RANGE_COND              (uL)
    #define ADC_1_CFG1_USE_SOC                 (0uL)
    /* Clock parameters*/
    #if (ADC_1_CLOCK_INTERNAL)
        #define ADC_1_CFG1_CLOCKDIVIDER        (((CYDEV_BCLK__HFCLK__HZ) / (ADC_1_CFG1_NOMINAL_CLOCK_FREQ)) - 1u)
    #endif /* (ADC_1_CLOCK_INTERNAL) */
    /* Filter Parameters */
    #if(ADC_1_CFG1_FILTER_PRESENT == 1uL)
    /* Timing settings */
        /* Clock divider register uses 0 for a divider of 1, 1 for a divider of 2, and so on. */
        #define ADC_1_CFG1_FILTERCLOCKDIVIDER  (u - 1u)
        #define ADC_1_CFG1_FILTERSTARTDELAY    (u)
        #define ADC_1_CFG1_FILTERTRIMDELAY     (u)
    #endif /* (ADC_1_CFG1_FILTER_PRESENT == 1uL) */
#endif /* #if(ADC_1_TOTAL_CONFIGS > 1) */

/* ************************************************************************** */
/* End configuration 1 customizer defines                                     */
/* ************************************************************************** */

/* ************************************************************************** */
/* Begin configuration 2 customizer defines                                         */
/* ************************************************************************** */

#if(ADC_1_TOTAL_CONFIGS > 2)
    #define ADC_1_CFG2_SAMPLE_RATE             ()
    #define ADC_1_CFG2_CHANNEL_BASE            (u)
    #define ADC_1_CFG2_CHANNEL_COUNT           (u)
    #define ADC_1_CFG2_VREF_SEL_MASK           (u)
    #define ADC_1_CFG2_NOMINAL_CLOCK_FREQ      (u)
    #define ADC_1_CFG2_CHANNEL_EN              (u)
    /* Sample Control Register */
    #define ADC_1_CFG2_SUB_RESOLUTION          (u)
    #define ADC_1_CFG2_SINGLE_ENDED_FORMAT     (uL)
    #define ADC_1_CFG2_DIFFERENTIAL_FORMAT     (uL)
    #define ADC_1_CFG2_SAMPLES_AVERAGED        (uL)
    #define ADC_1_CFG2_RANGE_INTR_MASK         (u)
    #define ADC_1_CFG2_SATURATE_INTR_MASK      (u)
    #define ADC_1_CFG2_VNEG_INPUT_SEL          ()
    #define ADC_1_CFG2_SINGLE_PRESENT          ()
    #define ADC_1_CFG2_VREF_MV_VALUE           ()
    #define ADC_1_CFG2_APERTURE_TIME0          (uL)
    #define ADC_1_CFG2_APERTURE_TIME1          (uL)
    #define ADC_1_CFG2_APERTURE_TIME2          (uL)
    #define ADC_1_CFG2_APERTURE_TIME3          (uL)
    #define ADC_1_CFG2_FREERUNNING             (u)
    #define ADC_1_CFG2_AVGERAGING_MODE         (uL)
    #define ADC_1_CFG2_LOW_LIMIT               (u)
    #define ADC_1_CFG2_HIGH_LIMIT              (u)
    #define ADC_1_CFG2_RANGE_COND              (uL)
    #define ADC_1_CFG2_USE_SOC                 (0uL)
    /* Clock parameters*/
    #if (ADC_1_CLOCK_INTERNAL)
        #define ADC_1_CFG2_CLOCKDIVIDER        (((CYDEV_BCLK__HFCLK__HZ) / (ADC_1_CFG2_NOMINAL_CLOCK_FREQ)) - 1u)
    #endif /* (ADC_1_CLOCK_INTERNAL) */
    /* Filter Parameters */
    #if(ADC_1_CFG2_FILTER_PRESENT == 1uL)
    /* Timing settings */
        /* Clock divider register uses 0 for a divider of 1, 1 for a divider of 2, and so on. */
        #define ADC_1_CFG2_FILTERCLOCKDIVIDER  (u - 1u)
        #define ADC_1_CFG2_FILTERSTARTDELAY    (u)
        #define ADC_1_CFG2_FILTERTRIMDELAY     (u)
    #endif /* (ADC_1_CFG2_FILTER_PRESENT == 1uL) */
#endif /* (ADC_1_TOTAL_CONFIGS > 2) */

/* ************************************************************************** */
/* End configuration 2 customizer defines                                     */
/* ************************************************************************** */

/* ************************************************************************** */
/* Begin configuration 3 customizer defines                                         */
/* ************************************************************************** */

#if(ADC_1_TOTAL_CONFIGS > 3)
    #define ADC_1_CFG3_SAMPLE_RATE             ()
    #define ADC_1_CFG3_CHANNEL_BASE            (u)
    #define ADC_1_CFG3_CHANNEL_COUNT           (u)
    #define ADC_1_CFG3_VREF_SEL_MASK           (u)
    #define ADC_1_CFG3_NOMINAL_CLOCK_FREQ      (u)
    #define ADC_1_CFG3_CHANNEL_EN              (u)
    /* Sample Control Register */
    #define ADC_1_CFG3_SUB_RESOLUTION          (u)
    #define ADC_1_CFG3_SINGLE_ENDED_FORMAT     (uL)
    #define ADC_1_CFG3_DIFFERENTIAL_FORMAT     (uL)
    #define ADC_1_CFG3_SAMPLES_AVERAGED        (uL)
    #define ADC_1_CFG3_RANGE_INTR_MASK         (u)
    #define ADC_1_CFG3_SATURATE_INTR_MASK      (u)
    #define ADC_1_CFG3_VNEG_INPUT_SEL          ()
    #define ADC_1_CFG3_SINGLE_PRESENT          ()
    #define ADC_1_CFG3_VREF_MV_VALUE           ()
    #define ADC_1_CFG3_APERTURE_TIME0          (uL)
    #define ADC_1_CFG3_APERTURE_TIME1          (uL)
    #define ADC_1_CFG3_APERTURE_TIME2          (uL)
    #define ADC_1_CFG3_APERTURE_TIME3          (uL)
    #define ADC_1_CFG3_FREERUNNING             (u)
    #define ADC_1_CFG3_AVGERAGING_MODE         (uL)
    #define ADC_1_CFG3_LOW_LIMIT               (u)
    #define ADC_1_CFG3_HIGH_LIMIT              (u)
    #define ADC_1_CFG3_RANGE_COND              (uL)
    #define ADC_1_CFG3_USE_SOC                 (0uL)
    /* Clock parameters*/
    #if (ADC_1_CLOCK_INTERNAL)
        #define ADC_1_CFG3_CLOCKDIVIDER        (((CYDEV_BCLK__HFCLK__HZ) / (ADC_1_CFG3_NOMINAL_CLOCK_FREQ)) - 1u)
    #endif /* (ADC_1_CLOCK_INTERNAL) */
    /* Filter Parameters */
    #if(ADC_1_CFG3_FILTER_PRESENT == 1uL)
    /* Timing settings */
        /* Clock divider register uses 0 for a divider of 1, 1 for a divider of 2, and so on. */
        #define ADC_1_CFG3_FILTERCLOCKDIVIDER  (u - 1u)
        #define ADC_1_CFG3_FILTERSTARTDELAY    (u)
        #define ADC_1_CFG3_FILTERTRIMDELAY     (u)
    #endif /* (ADC_1_CFG3_FILTER_PRESENT == 1uL) */
#endif /* (ADC_1_TOTAL_CONFIGS > 3) */

/* ************************************************************************** */
/* End configuration 3 customizer defines                                     */
/* ************************************************************************** */

#define ADC_1_DEFAULT_SAMPLE_MODE_SEL      (0u)
#define ADC_1_FREERUNNING                  (1u)
#define ADC_1_HARDWARESOC                  (1u)

/* Constants for setting the SAR to trigger the UAB Filter */
#define ADC_1_SRAM_CTRL_TRIGGER            (0x3F000000u)

/** ADC_1_TOTAL_CHANNELS_NUM
* This constant represents the amount of input channels available for scanning.
*/
#define ADC_1_TOTAL_CHANNELS_NUM           (1u)

/*******************************************************************************
*    Variables with External Linkage
*******************************************************************************/
/**
* \addtogroup group_globals
* Globals are noted in the descriptions of the functions that use globals.
* They are marked with (R), (W), or (RW) noting whether the variable is read,
* write, or read/write.
* @{
*/

/** ADC_1_initVar
* The ADC_1_initVar variable is used to indicate
* initial configuration of this component. The variable is initialized to zero and
* set to 1 the first time ADC_1_Start() is called. This allows for
* component initialization without reinitialization in all subsequent calls to the
* ADC_1_Start() routine.
*
* If reinitialization of the component is required, then the ADC_1_Init() function
* can be called before the ADC_1_Start() or ADC_1_Enable()
* functions.
*/
extern uint8 ADC_1_initVar;
extern uint8 ADC_1_selected;

#define ADC_1_INIT_VAR_INIT_FLAG     (0x01u)

/** ADC_1_offset
* This array calibrates the offset for each channel. The first time Start() is
* called, the offset array's entries are initialized to 0, except for channels
* which are Single-Ended, Signed, and have Vneg=Vref, for which it is set to
* -(2^Resolution)/2. It can be modified using ADC_SetOffset(). The array
* is used by the ADC_CountsTo_Volts(), ADC_CountsTo_mVolts(), and
* ADC_CountsTo_uVolts() functions.
*/
extern volatile int16 ADC_1_offset[ADC_1_TOTAL_CHANNELS_NUM];

/** ADC_1_countsPer10Volt
* This array is used to calibrate the gain for each channel. It is calculated
* the first time ADC_1_Start() is called. The value depends on
* channel resolution and voltage reference. It can be changed using
* ADC_1_SetGain().
*
* This array affects the ADC_1_CountsTo_Volts(),
* ADC_1_CountsTo_mVolts(), and ADC_1_CountsTo_uVolts()
* functions by supplying the correct conversion between ADC counts and the
* applied input voltage.
*/
extern volatile int32 ADC_1_countsPer10Volt[ADC_1_TOTAL_CHANNELS_NUM];   /* Gain compensation */
extern const ADC_1_CONFIG_STRUCT ADC_1_allConfigs[ADC_1_TOTAL_CONFIGS];
extern const uint32 CYCODE ADC_1_InputsPlacement[ADC_1_TOTAL_CHANNELS_NUM];

/** @} globals */

#define ADC_1_HALF_A_CC_GND              (0) /* Half A CC_GND is always zero */
#define ADC_1_SW_EXTAGND  (CyUAB_sw_id_enum) CyUAB_SW_ID(ADC_1_UABH_A_SW_STATIC_PTR,((uint32)ADC_1_UABH_A_halfuab__VAGND_SRC), CyUAB_SW_NOX)  /* static: OA+ <-> External routed agnd */
#define ADC_1_SW_EXTREFA  (CyUAB_sw_id_enum) CyUAB_SW_ID(ADC_1_UABH_A_SW_CA_IN0_PTR,((uint32)((uint32)ADC_1_UABH_A_halfuab__VREF_SRC)*ADC_1_UABH_A_DYNAMIC_FIELD_SIZE), CyUAB_SW_NOX) /* A in <-> External routed vref */
#define ADC_1_SW_EXTREFB  (CyUAB_sw_id_enum) CyUAB_SW_ID(ADC_1_UABH_A_SW_CB_IN0_PTR,((uint32)((uint32)ADC_1_UABH_A_halfuab__VREF_SRC)*ADC_1_UABH_A_DYNAMIC_FIELD_SIZE ), CyUAB_SW_NOX) /* B in <-> External routed vref */

#define ADC_1_MAX_FREQUENCY              (18000000u)       /*18Mhz*/

#define ADC_1_10US_DELAY                 (10u)
#define ADC_1_10V_COUNTS                 (10.0F)
#define ADC_1_10MV_COUNTS                (10000)
#define ADC_1_10UV_COUNTS                (10000000L)

/*******************************************************************************
*              Registers
*******************************************************************************/

#define ADC_1_SAR_CTRL_REG                   (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CTRL )
#define ADC_1_SAR_CTRL_PTR                   ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CTRL )

#define ADC_1_SAR_SAMPLE_CTRL_REG            (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_SAMPLE_CTRL )
#define ADC_1_SAR_SAMPLE_CTRL_PTR            ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_SAMPLE_CTRL )

#define ADC_1_SAR_SAMPLE_TIME01_REG          (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_SAMPLE_TIME01 )
#define ADC_1_SAR_SAMPLE_TIME01_PTR          ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_SAMPLE_TIME01 )

#define ADC_1_SAR_SAMPLE_TIME23_REG          (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_SAMPLE_TIME23 )
#define ADC_1_SAR_SAMPLE_TIME23_PTR          ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_SAMPLE_TIME23 )

#define ADC_1_SAR_RANGE_THRES_REG            (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_RANGE_THRES )
#define ADC_1_SAR_RANGE_THRES_PTR            ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_RANGE_THRES )

#define ADC_1_SAR_RANGE_COND_REG             (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_RANGE_COND )
#define ADC_1_SAR_RANGE_COND_PTR             ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_RANGE_COND )

#define ADC_1_SAR_CHAN_EN_REG                (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_EN )
#define ADC_1_SAR_CHAN_EN_PTR                ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_EN )

#define ADC_1_SAR_START_CTRL_REG             (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_START_CTRL )
#define ADC_1_SAR_START_CTRL_PTR             ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_START_CTRL )
/* CDT-230388: These constants are getting added to cyfitter.h */
#define ADC_1_SAR_DFT_CTRL_REG               (*(reg32 *) CYREG_SAR_DFT_CTRL )
#define ADC_1_SAR_DFT_CTRL_PTR               ( (reg32 *) CYREG_SAR_DFT_CTRL )

#define ADC_1_SAR_CHAN_CONFIG_REG            (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_CONFIG00 )
#define ADC_1_SAR_CHAN_CONFIG_PTR            ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_CONFIG00 )
#define ADC_1_SAR_CHAN_CONFIG_IND            ( ADC_1_cy_psoc4_sar_1__SAR_CHAN_CONFIG00 )

#define ADC_1_SAR_CHAN_WORK_REG              (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_WORK00 )
#define ADC_1_SAR_CHAN_WORK_PTR              ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_WORK00 )

#define ADC_1_SAR_CHAN_RESULT_REG            (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT00 )
#define ADC_1_SAR_CHAN_RESULT_PTR            ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT00 )

#define ADC_1_SAR_CHAN_RESULT_IND            ( ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT00 )

#define ADC_1_SAR_CHAN0_RESULT_REG           (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT00 )
#define ADC_1_SAR_CHAN0_RESULT_PTR           ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT00 )

#define ADC_1_SAR_CHAN1_RESULT_REG           (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT01 )
#define ADC_1_SAR_CHAN1_RESULT_PTR           ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT01 )

#define ADC_1_SAR_CHAN2_RESULT_REG           (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT02 )
#define ADC_1_SAR_CHAN2_RESULT_PTR           ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT02 )

#define ADC_1_SAR_CHAN3_RESULT_REG           (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT03 )
#define ADC_1_SAR_CHAN3_RESULT_PTR           ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT03 )

#define ADC_1_SAR_CHAN4_RESULT_REG           (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT04 )
#define ADC_1_SAR_CHAN4_RESULT_PTR           ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT04 )

#define ADC_1_SAR_CHAN5_RESULT_REG           (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT05 )
#define ADC_1_SAR_CHAN5_RESULT_PTR           ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT05 )

#define ADC_1_SAR_CHAN6_RESULT_REG           (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT06 )
#define ADC_1_SAR_CHAN6_RESULT_PTR           ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT06 )

#define ADC_1_SAR_CHAN7_RESULT_REG           (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT07 )
#define ADC_1_SAR_CHAN7_RESULT_PTR           ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT07 )

#define ADC_1_SAR_CHAN8_RESULT_REG           (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT08 )
#define ADC_1_SAR_CHAN8_RESULT_PTR           ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT08 )

#define ADC_1_SAR_CHAN9_RESULT_REG           (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT09 )
#define ADC_1_SAR_CHAN9_RESULT_PTR           ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT09 )

#define ADC_1_SAR_CHAN10_RESULT_REG          (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT10 )
#define ADC_1_SAR_CHAN10_RESULT_PTR          ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT10 )

#define ADC_1_SAR_CHAN11_RESULT_REG          (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT11 )
#define ADC_1_SAR_CHAN11_RESULT_PTR          ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT11 )

#define ADC_1_SAR_CHAN12_RESULT_REG          (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT12 )
#define ADC_1_SAR_CHAN12_RESULT_PTR          ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT12 )

#define ADC_1_SAR_CHAN13_RESULT_REG          (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT13 )
#define ADC_1_SAR_CHAN13_RESULT_PTR          ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT13 )

#define ADC_1_SAR_CHAN14_RESULT_REG          (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT14 )
#define ADC_1_SAR_CHAN14_RESULT_PTR          ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT14 )

#define ADC_1_SAR_CHAN15_RESULT_REG          (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT15 )
#define ADC_1_SAR_CHAN15_RESULT_PTR          ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_CHAN_RESULT15 )
/* CDT-230483: These will be in cyfitter. */
#define ADC_1_SAR_CHAN_WORK_UPDATED_REG      (*(reg32 *) CYREG_SAR_CHAN_WORK_UPDATED)
#define ADC_1_SAR_CHAN_WORK_UPDATED_PTR      ( (reg32 *) CYREG_SAR_CHAN_WORK_UPDATED)

#define ADC_1_SAR_CHAN_RESULT_UPDATED_REG    ( *(reg32 *) CYREG_SAR_CHAN_RESULT_UPDATED )
#define ADC_1_SAR_CHAN_RESULT_UPDATED_PTR    ( (reg32 *) CYREG_SAR_CHAN_RESULT_UPDATED )

#define ADC_1_SAR_CHAN_WORK_NEWVALUE_REG     (*(reg32 *) CYREG_SAR_CHAN_WORK_NEWVALUE)
#define ADC_1_SAR_CHAN_WORK_NEWVALUE_PTR     ( (reg32 *) CYREG_SAR_CHAN_WORK_NEWVALUE)

#define ADC_1_SAR_CHAN_RESULT_NEWVALUE_REG   ( *(reg32 *) CYREG_SAR_CHAN_RESULT_NEWVALUE )
#define ADC_1_SAR_CHAN_RESULT_NEWVALUE_PTR   ( (reg32 *) CYREG_SAR_CHAN_RESULT_NEWVALUE )

#define ADC_1_SAR_STATUS_REG                 (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_STATUS )
#define ADC_1_SAR_STATUS_PTR                 ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_STATUS )

#define ADC_1_SAR_AVG_START_REG              (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_AVG_STAT )
#define ADC_1_SAR_AVG_START_PTR              ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_AVG_STAT )

#define ADC_1_SAR_INTR_REG                   (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_INTR )
#define ADC_1_SAR_INTR_PTR                   ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_INTR )

#define ADC_1_SAR_INTR_SET_REG               (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_INTR_SET )
#define ADC_1_SAR_INTR_SET_PTR               ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_INTR_SET )

#define ADC_1_SAR_INTR_MASK_REG              (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_INTR_MASK )
#define ADC_1_SAR_INTR_MASK_PTR              ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_INTR_MASK )

#define ADC_1_SAR_INTR_MASKED_REG            (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_INTR_MASKED )
#define ADC_1_SAR_INTR_MASKED_PTR            ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_INTR_MASKED )

#define ADC_1_SAR_SATURATE_INTR_REG          (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_SATURATE_INTR )
#define ADC_1_SAR_SATURATE_INTR_PTR          ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_SATURATE_INTR )

#define ADC_1_SAR_SATURATE_INTR_SET_REG      (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_SATURATE_INTR_SET )
#define ADC_1_SAR_SATURATE_INTR_SET_PTR      ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_SATURATE_INTR_SET )

#define ADC_1_SAR_SATURATE_INTR_MASK_REG     (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_SATURATE_INTR_MASK )
#define ADC_1_SAR_SATURATE_INTR_MASK_PTR     ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_SATURATE_INTR_MASK )

#define ADC_1_SAR_SATURATE_INTR_MASKED_REG \
                                                 (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_SATURATE_INTR_MASKED )
#define ADC_1_SAR_SATURATE_INTR_MASKED_PTR \
                                                 ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_SATURATE_INTR_MASKED )

#define ADC_1_SAR_RANGE_INTR_REG             (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_RANGE_INTR )
#define ADC_1_SAR_RANGE_INTR_PTR             ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_RANGE_INTR )

#define ADC_1_SAR_RANGE_INTR_SET_REG         (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_RANGE_INTR_SET )
#define ADC_1_SAR_RANGE_INTR_SET_PTR         ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_RANGE_INTR_SET )

#define ADC_1_SAR_RANGE_INTR_MASK_REG        (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_RANGE_INTR_MASK )
#define ADC_1_SAR_RANGE_INTR_MASK_PTR        ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_RANGE_INTR_MASK )

#define ADC_1_SAR_RANGE_INTR_MASKED_REG      (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_RANGE_INTR_MASKED )
#define ADC_1_SAR_RANGE_INTR_MASKED_PTR      ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_RANGE_INTR_MASKED )

#define ADC_1_SAR_INTR_CAUSE_REG             (*(reg32 *) ADC_1_cy_psoc4_sar_1__SAR_INTR_CAUSE )
#define ADC_1_SAR_INTR_CAUSE_PTR             ( (reg32 *) ADC_1_cy_psoc4_sar_1__SAR_INTR_CAUSE )

#define ADC_1_SAR_INJ_CHAN_CONFIG_REG        (*(reg32 *) ADC_1_cy_psoc4_sarmux_1__SAR_INJ_CHAN_CONFIG)
#define ADC_1_SAR_INJ_CHAN_CONFIG_PTR        ( (reg32 *) ADC_1_cy_psoc4_sarmux_1__SAR_INJ_CHAN_CONFIG)

#define ADC_1_SAR_INJ_RESULT_REG             (*(reg32 *) ADC_1_cy_psoc4_sarmux_1__SAR_INJ_RESULT )
#define ADC_1_SAR_INJ_RESULT_PTR             ( (reg32 *) ADC_1_cy_psoc4_sarmux_1__SAR_INJ_RESULT )
/* CDT-230388: These constants are getting added to cyfitter.h */
#define ADC_1_MUX_SWITCH0_REG                (*(reg32 *)  CYREG_SAR_MUX_SWITCH0 )
#define ADC_1_MUX_SWITCH0_PTR                ( (reg32 *)  CYREG_SAR_MUX_SWITCH0 )

#define ADC_1_MUX_SWITCH_HW_CTRL_REG         (*(reg32 *)  CYREG_SAR_MUX_SWITCH_HW_CTRL )
#define ADC_1_MUX_SWITCH_HW_CTRL_PTR         ( (reg32 *)  CYREG_SAR_MUX_SWITCH_HW_CTRL )

#define ADC_1_PUMP_CTRL_REG                  (*(reg32 *)  CYREG_SAR_PUMP_CTRL )
#define ADC_1_PUMP_CTRL_PTR                  ( (reg32 *)  CYREG_SAR_PUMP_CTRL )

#define ADC_1_ANA_TRIM_REG                   (*(reg32 *)  CYREG_SAR_ANA_TRIM )
#define ADC_1_ANA_TRIM_PTR                   ( (reg32 *)  CYREG_SAR_ANA_TRIM )

#define ADC_1_WOUNDING_REG                   (*(reg32 *)  CYREG_SAR_WOUNDING )
#define ADC_1_WOUNDING_PTR                   ( (reg32 *)  CYREG_SAR_WOUNDING )

/*******************************************************************************
*       Register Constants
*******************************************************************************/
#define ADC_1_VIN_SWITCH_CLOSED                        (0xFu)
#define ADC_1_VIN_ALL_SWITCHES_MASK                    (0xFFFFu)     /* Mask for SW_C**_IN0 VIN settings */

#define ADC_1_INTC_NUMBER                              (ADC_1_IRQ__INTC_NUMBER)
#define ADC_1_INTC_PRIOR_NUMBER                        (ADC_1_IRQ__INTC_PRIOR_NUM)

/* ***************************SAR_CTRL_REG fields *************************** */
/* VREF_SEL bitfield -- 3 bits wide -- RW access */
#define ADC_1_SAR_CTRL_VREF_SEL_MSK                    (0x00000070u)
#define ADC_1_SAR_CTRL_VREF_SEL_SHFT                   (4ul)
    /* VREF_SEL bitfield enumerated values */
    #define ADC_1_VREF_SEL_VREF0                       (0x0ul)
    #define ADC_1_VREF_SEL_VREF1                       (0x1ul)
    #define ADC_1_VREF_SEL_VREF2                       (0x2ul)
    #define ADC_1_VREF_SEL_VREF_AROUTE                 (0x3ul)
    #define ADC_1_VREF_SEL_VBGR                        (0x4ul)
    #define ADC_1_VREF_SEL_VREF_EXT                    (0x5ul)
    #define ADC_1_VREF_SEL_VDDA_DIV_2                  (0x6ul)
    #define ADC_1_VREF_SEL_VDDA                        (0x7ul)

#define ADC_1__SAR_CTRL_VREF_BYP_AND_SEL_MSK           (0x000000F0u)
#define ADC_1__INTERNAL1024                            (ADC_1_VREF_SEL_VBGR \
                                                                    << ADC_1_SAR_CTRL_VREF_SEL_SHFT)
/* VREF is zero for the component if routed by Creator to an internal reference */
#define ADC_1__INTERNALVREF                            (0x0ul)
#define ADC_1__VDDA_VREF                               (ADC_1_VREF_SEL_VDDA \
                                                                    << ADC_1_SAR_CTRL_VREF_SEL_SHFT)
#define ADC_1__EXT_VREF                               (ADC_1_VREF_SEL_VREF_EXT \
                                                                    << ADC_1_SAR_CTRL_VREF_SEL_SHFT)
/* VREF_BYP_CAP_EN bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_CTRL_VREF_BYP_CAP_EN_MSK             (0x00000080u)
#define ADC_1_SAR_CTRL_VREF_BYP_CAP_EN_SHFT            (7U)

/* NEG_SEL bitfield -- 3 bits wide -- RW access */
#define ADC_1_SAR_CTRL_NEG_SEL_MSK                     (0x00000e00ul)
#define ADC_1_SAR_CTRL_NEG_SEL_SHFT                    (9ul)
    /* NEG_SEL bitfield enumerated values */
    #define ADC_1_NEG_SEL_VSSA_KELVIN                  (0x0uL)
    #define ADC_1_NEG_SEL_ART_VSSA                     (0x1uL)
    #define ADC_1_NEG_SEL_P1                           (0x2uL)
    #define ADC_1_NEG_SEL_P3                           (0x3uL)
    #define ADC_1_NEG_SEL_P5                           (0x4uL)
    #define ADC_1_NEG_SEL_P7                           (0x5uL)
    #define ADC_1_NEG_SEL_ACORE                        (0x6uL)
    #define ADC_1_NEG_SEL_VREF                         (0x7uL)
    #define ADC_1_NEG_VREF_SHIFTED                     (ADC_1_NEG_SEL_VREF \
                                                                    << ADC_1_SAR_CTRL_NEG_SEL_SHFT)
/* SAR_HW_CTRL_NEGVREF bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_CTRL_SAR_HW_CTRL_NEGVREF_MSK         (0x00002000u)
#define ADC_1_SAR_CTRL_SAR_HW_CTRL_NEGVREF_SHFT        (13U)

/* PWR_CTRL_VREF bitfield -- 2 bits wide -- RW access */
#define ADC_1_SAR_CTRL_PWR_CTRL_VREF_MSK               (0x0000c000u)
#define ADC_1_SAR_CTRL_PWR_CTRL_VREF_SHFT              (14u)
    /* PWR_CTRL_VREF bitfield enumerated values */
    #define ADC_1_PWR_CTRL_VREF_NORMAL_PWR             (0x0uL)
    #define ADC_1_PWR_CTRL_VREF_THIRD_PWR              (0x2uL)

/* SPARE bitfield -- 4 bits wide -- RW access */
#define ADC_1_SAR_CTRL_SPARE_MSK                       (0x000f0000u)
#define ADC_1_SAR_CTRL_SPARE_SHFT                      (16U)

/* BOOSTPUMP_EN bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_CTRL_BOOSTPUMP_EN_MSK                (0x00100000u)
#define ADC_1_SAR_CTRL_BOOSTPUMP_EN_SHFT               (20U)

/* REFBUF_EN bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_CTRL_REFBUF_EN_MSK                   (0x00200000u)
#define ADC_1_SAR_CTRL_REFBUF_EN_SHFT                  (21U)

/* ICONT_LV bitfield -- 2 bits wide -- RW access */
#define ADC_1_SAR_CTRL_ICONT_LV_MSK                    (0x03000000u)
#define ADC_1_SAR_CTRL_ICONT_LV_SHFT                   (24U)
    /* ICONT_LV bitfield enumerated values */
    #define ADC_1_ICONT_LV_NORMAL_PWR                  (0x0)
    #define ADC_1_ICONT_LV_HALF_PWR                    (0x1)
    #define ADC_1_ICONT_LV_MORE_PWR                    (0x2)
    #define ADC_1_ICONT_LV_QUARTER_PWR                 (0x3)

/* DEEPSLEEP_ON bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_CTRL_DEEPSLEEP_ON_MSK                (0x08000000u)
#define ADC_1_SAR_CTRL_DEEPSLEEP_ON_SHFT               (27U)

/* DSI_SYNC_CONFIG bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_CTRL_DSI_SYNC_CONFIG_MSK             (0x10000000u)
#define ADC_1_SAR_CTRL_DSI_SYNC_CONFIG_SHFT            (28U)

/* DSI_MODE bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_CTRL_DSI_MODE_MSK                    (0x20000000u)
#define ADC_1_SAR_CTRL_DSI_MODE_SHFT                   (29U)

/* SWITCH_DISABLE bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_CTRL_SWITCH_DISABLE_MSK              (0x40000000u)
#define ADC_1_SAR_CTRL_SWITCH_DISABLE_SHFT             (30U)

/* ENABLED bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_CTRL_ENABLED_MSK                     (0x80000000u)
#define ADC_1_SAR_CTRL_ENABLED_SHFT                    (31U)


/* ***********************SAR_SAMPLE_CTRL_REG fields ************************ */
/* SUB_RESOLUTION bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_CTRL_SUB_RESOLUTION_MSK       (0x00000001u)
#define ADC_1_SAR_SAMPLE_CTRL_SUB_RESOLUTION_SHFT      (0U)
    /* SUB_RESOLUTION bitfield enumerated values */
    #define ADC_1_SUB_RESOLUTION_8B                    (0x0U)
    #define ADC_1_SUB_RESOLUTION_10B                   (0x1U)

/* LEFT_ALIGN bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_CTRL_LEFT_ALIGN_MSK           (0x00000002u)
#define ADC_1_SAR_SAMPLE_CTRL_LEFT_ALIGN_SHFT          (1u)

/* SINGLE_ENDED_SIGNED bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_CTRL_SINGLE_ENDED_SIGNED_MSK  (0x00000004u)
#define ADC_1_SAR_SAMPLE_CTRL_SINGLE_ENDED_SIGNED_SHFT (2u)
    /* SINGLE_ENDED_SIGNED bitfield enumerated values */
    #define ADC_1_SINGLE_ENDED_SIGNED_UNSIGNED         (0x0L)
    #define ADC_1_SINGLE_ENDED_SIGNED_SIGNED           (0x1L)

/* DIFFERENTIAL_SIGNED bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_CTRL_DIFFERENTIAL_SIGNED_MSK  (0x00000008u)
#define ADC_1_SAR_SAMPLE_CTRL_DIFFERENTIAL_SIGNED_SHFT (3u)
    /* DIFFERENTIAL_SIGNED bitfield enumerated values */
    #define ADC_1_DIFFERENTIAL_SIGNED_UNSIGNED         (0x0u)
    #define ADC_1_DIFFERENTIAL_SIGNED_SIGNED           (0x1u)

/* AVG_CNT bitfield -- 3 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_CTRL_AVG_CNT_MSK              (0x00000070u)
#define ADC_1_SAR_SAMPLE_CTRL_AVG_CNT_SHFT             (4u)

/* AVG_SHIFT bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_CTRL_AVG_SHIFT_MSK            (0x00000080u)
#define ADC_1_SAR_SAMPLE_CTRL_AVG_SHIFT_SHFT           (7u)

/* AVG_MODE bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_CTRL_AVG_MODE_MSK             (0x00000100u)
#define ADC_1_SAR_SAMPLE_CTRL_AVG_MODE_SHFT            (8u)
    /* AVG_MODE bitfield enumerated values */
    #define ADC_1_AVG_MODE_ACCUNDUMP                   (0x0u)
    #define ADC_1_AVG_MODE_INTERLEAVED                 (0x1u)

/* CONTINUOUS bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_CTRL_CONTINUOUS_MSK           (0x00010000u)
#define ADC_1_SAR_SAMPLE_CTRL_CONTINUOUS_SHFT          (16u)

/* DSI_TRIGGER_EN bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_CTRL_DSI_TRIGGER_EN_MSK       (0x00020000u)
#define ADC_1_SAR_SAMPLE_CTRL_DSI_TRIGGER_EN_SHFT      (17u)

/* DSI_TRIGGER_LEVEL bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_CTRL_DSI_TRIGGER_LEVEL_MSK    (0x00040000u)
#define ADC_1_SAR_SAMPLE_CTRL_DSI_TRIGGER_LEVEL_SHFT   (18u)

/* DSI_SYNC_TRIGGER bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_CTRL_DSI_SYNC_TRIGGER_MSK     (0x00080000u)
#define ADC_1_SAR_SAMPLE_CTRL_DSI_SYNC_TRIGGER_SHFT    (19u)

/* UAB_SCAN_MODE bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_CTRL_UAB_SCAN_MODE_MSK        (0x00400000u)
#define ADC_1_SAR_SAMPLE_CTRL_UAB_SCAN_MODE_SHFT       (22u)
    /* UAB_SCAN_MODE bitfield enumerated values */
    #define ADC_1_UAB_SCAN_MODE_UNSCHEDULED            (0x0u)
    #define ADC_1_UAB_SCAN_MODE_SCHEDULED              (0x1u)

/* REPEAT_INVALID bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_CTRL_REPEAT_INVALID_MSK       (0x00800000u)
#define ADC_1_SAR_SAMPLE_CTRL_REPEAT_INVALID_SHFT      (23u)

/* VALID_SEL bitfield -- 3 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_CTRL_VALID_SEL_MSK            (0x07000000u)
#define ADC_1_SAR_SAMPLE_CTRL_VALID_SEL_SHFT           (24u)

/* VALID_SEL_EN bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_CTRL_VALID_SEL_EN_MSK         (0x08000000u)
#define ADC_1_SAR_SAMPLE_CTRL_VALID_SEL_EN_SHFT        (27u)

/* VALID_IGNORE bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_CTRL_VALID_IGNORE_MSK         (0x10000000u)
#define ADC_1_SAR_SAMPLE_CTRL_VALID_IGNORE_SHFT        (28u)

/* TRIGGER_OUT_EN bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_CTRL_TRIGGER_OUT_EN_MSK       (0x40000000u)
#define ADC_1_SAR_SAMPLE_CTRL_TRIGGER_OUT_EN_SHFT      (30u)

/* EOS_DSI_OUT_EN bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_CTRL_EOS_DSI_OUT_EN_MSK       (0x80000000u)
#define ADC_1_SAR_SAMPLE_CTRL_EOS_DSI_OUT_EN_SHFT      (31u)


/* **********************SAR_SAMPLE_TIME01_REG fields *********************** */
/* SAMPLE_TIME0 bitfield -- 10 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_TIME01_SAMPLE_TIME0_MSK       (0x000003ffu)
#define ADC_1_SAR_SAMPLE_TIME01_SAMPLE_TIME0_SHFT      (0u)

/* SAMPLE_TIME1 bitfield -- 10 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_TIME01_SAMPLE_TIME1_MSK       (0x03ff0000u)
#define ADC_1_SAR_SAMPLE_TIME01_SAMPLE_TIME1_SHFT      (16u)


/* **********************SAR_SAMPLE_TIME23_REG fields *********************** */
/* SAMPLE_TIME2 bitfield -- 10 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_TIME23_SAMPLE_TIME2_MSK       (0x000003ffu)
#define ADC_1_SAR_SAMPLE_TIME23_SAMPLE_TIME2_SHFT      (0u)

/* SAMPLE_TIME3 bitfield -- 10 bits wide -- RW access */
#define ADC_1_SAR_SAMPLE_TIME23_SAMPLE_TIME3_MSK       (0x03ff0000u)
#define ADC_1_SAR_SAMPLE_TIME23_SAMPLE_TIME3_SHFT      (16u)


/* ***********************SAR_RANGE_THRES_REG fields ************************ */
/* RANGE_LOW bitfield -- 16 bits wide -- RW access */
#define ADC_1_SAR_RANGE_THRES_RANGE_LOW_MSK            (0x0000ffffu)
#define ADC_1_SAR_RANGE_THRES_RANGE_LOW_SHFT           (0U)

/* RANGE_HIGH bitfield -- 16 bits wide -- RW access */
#define ADC_1_SAR_RANGE_THRES_RANGE_HIGH_MSK           (0xffff0000u)
#define ADC_1_SAR_RANGE_THRES_RANGE_HIGH_SHFT          (16U)


/* ************************SAR_RANGE_COND_REG fields ************************ */
/* RANGE_COND bitfield -- 2 bits wide -- RW access */
#define ADC_1_SAR_RANGE_COND_RANGE_COND_MSK            (0xc0000000u)
#define ADC_1_SAR_RANGE_COND_RANGE_COND_SHFT           (30u)
    /* RANGE_COND bitfield enumerated values */
    #define ADC_1_RANGE_COND_BELOW                     (0x0)
    #define ADC_1_RANGE_COND_INSIDE                    (0x1)
    #define ADC_1_RANGE_COND_ABOVE                     (0x2)
    #define ADC_1_RANGE_COND_OUTSIDE                   (0x3)


/* *************************SAR_CHAN_EN_REG fields ************************** */
/* CHAN_EN bitfield -- 16 bits wide -- RW access */
#define ADC_1_SAR_CHAN_EN_CHAN_EN_MSK                  (0x0000ffffu)
#define ADC_1_SAR_CHAN_EN_CHAN_EN_SHFT                 (0u)


/* ************************SAR_START_CTRL_REG fields ************************ */
/* FW_TRIGGER bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_START_CTRL_FW_TRIGGER_MSK            (0x00000001uL)
#define ADC_1_SAR_START_CTRL_FW_TRIGGER_SHFT           (0u)


/* *************************SAR_DFT_CTRL_REG fields ************************* */
/* DLY_INC bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_DFT_CTRL_DLY_INC_MSK                 (0x00000001uL)
#define ADC_1_SAR_DFT_CTRL_DLY_INC_SHFT                (0u)

/* HIZ bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_DFT_CTRL_HIZ_MSK                     (0x00000002uL)
#define ADC_1_SAR_DFT_CTRL_HIZ_SHFT                    (1u)

/* DFT_INC bitfield -- 4 bits wide -- RW access */
#define ADC_1_SAR_DFT_CTRL_DFT_INC_MSK                 (0x000f0000uL)
#define ADC_1_SAR_DFT_CTRL_DFT_INC_SHFT                (16u)

/* DFT_OUTC bitfield -- 3 bits wide -- RW access */
#define ADC_1_SAR_DFT_CTRL_DFT_OUTC_MSK                (0x00700000uL)
#define ADC_1_SAR_DFT_CTRL_DFT_OUTC_SHFT               (20u)

/* SEL_CSEL_DFT bitfield -- 4 bits wide -- RW access */
#define ADC_1_SAR_DFT_CTRL_SEL_CSEL_DFT_MSK            (0x0f000000uL)
#define ADC_1_SAR_DFT_CTRL_SEL_CSEL_DFT_SHFT           (24u)

/* EN_CSEL_DFT bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_DFT_CTRL_EN_CSEL_DFT_MSK             (0x10000000uL)
#define ADC_1_SAR_DFT_CTRL_EN_CSEL_DFT_SHFT            (28u)

/* DCEN bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_DFT_CTRL_DCEN_MSK                    (0x20000000uL)
#define ADC_1_SAR_DFT_CTRL_DCEN_SHFT                   (29u)

/* ADFT_OVERRIDE bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_DFT_CTRL_ADFT_OVERRIDE_MSK           (0x80000000uL)
#define ADC_1_SAR_DFT_CTRL_ADFT_OVERRIDE_SHFT          (31u)

/* ***********************SAR_CHAN_CONFIG_REG fields *********************** */
/* POS_PIN_ADDR bitfield -- 3 bits wide -- RW access */
#define ADC_1_SAR_CHAN_CONFIG_POS_PIN_ADDR_MSK         (0x00000007u)
#define ADC_1_SAR_CHAN_CONFIG_POS_PIN_ADDR_SHFT        (0u)

/* POS_PORT_ADDR bitfield -- 3 bits wide -- RW access */
#define ADC_1_SAR_CHAN_CONFIG_POS_PORT_ADDR_MSK        (0x00000070u)
#define ADC_1_SAR_CHAN_CONFIG_POS_PORT_ADDR_SHFT       (4u)
    /* POS_PORT_ADDR bitfield enumerated values */
    #define ADC_1_POS_PORT_ADDR_SARMUX                 (0x0)
    #define ADC_1_POS_PORT_ADDR_CTB0                   (0x1)
    #define ADC_1_POS_PORT_ADDR_CTB1                   (0x2)
    #define ADC_1_POS_PORT_ADDR_CTB2                   (0x3)
    #define ADC_1_POS_PORT_ADDR_CTB3                   (0x4)
    #define ADC_1_POS_PORT_ADDR_AROUTE_VIRT2           (0x5)
    #define ADC_1_POS_PORT_ADDR_AROUTE_VIRT1           (0x6)
    #define ADC_1_POS_PORT_ADDR_SARMUX_VIRT            (0x7)

/* DIFFERENTIAL_EN bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_CHAN_CONFIG_DIFFERENTIAL_EN_MSK      (0x00000100uL)
#define ADC_1_SAR_CHAN_CONFIG_DIFFERENTIAL_EN_SHFT     (8u)

/* RESOLUTION bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_CHAN_CONFIG_RESOLUTION_MSK           (0x00000200u)
#define ADC_1_SAR_CHAN_CONFIG_RESOLUTION_SHFT          (9u)
    /* RESOLUTION bitfield enumerated values */
    #define ADC_1_RESOLUTION_MAXRES                    (0x0)
    #define ADC_1_RESOLUTION_SUBRES                    (0x1)

/* AVG_EN bitfield -- 1 bits wide -- RW access */
#define ADC_1_SAR_CHAN_CONFIG_AVG_EN_MSK               (0x00000400u)
#define ADC_1_SAR_CHAN_CONFIG_AVG_EN_SHFT              (10u)

/* SAMPLE_TIME_SEL bitfield -- 2 bits wide -- RW access */
#define ADC_1_SAR_CHAN_CONFIG_SAMPLE_TIME_SEL_MSK      (0x00003000u)
#define ADC_1_SAR_CHAN_CONFIG_SAMPLE_TIME_SEL_SHFT     (12u)

/* NEG_PIN_ADDR bitfield -- 3 bits wide -- RW access */
#define ADC_1_SAR_CHAN_CONFIG_NEG_PIN_ADDR_MSK         (0x00070000u)
#define ADC_1_SAR_CHAN_CONFIG_NEG_PIN_ADDR_SHFT        (16u)

/* NEG_PORT_ADDR bitfield -- 3 bits wide -- RW access */
#define ADC_1_SAR_CHAN_CONFIG_NEG_PORT_ADDR_MSK        (0x00700000u)
#define ADC_1_SAR_CHAN_CONFIG_NEG_PORT_ADDR_SHFT       (20u)
/* NEG_PORT_ADDR bitfield enumerated values */
#define ADC_1_NEG_PORT_ADDR_SARMUX                     (0x0)
#define ADC_1_NEG_PORT_ADDR_CTB3                       (0x4)
/* NEG_ADDR_EN bitfield */
#define ADC_1_CHANNEL_CONFIG_NEG_ADDR_EN_MSK           (0x01000000u)
#define ADC_1_CHANNEL_CONFIG_NEG_ADDR_EN_SHFT          (24u)
/*Mask for everything that is set by ADC_1_channelsConfig[] */
#define ADC_1_CHANNEL_CONFIG_MASK                      (0x80003700u)
/* Channels are differential if DIFFERENTIAL _EN or NEG_ADDR_EN */
#define ADC_1_CHANNEL_CONFIG_DIFF_ALL_MSK              (ADC_1_SAR_CHAN_CONFIG_DIFFERENTIAL_EN_MSK \
                                                                  | ADC_1_CHANNEL_CONFIG_NEG_ADDR_EN_MSK)
/* ***********************SAR_CHAN_WORK_REG fields ************************* */
#define ADC_1_SAR_WRK_MAX_12BIT                        (0x00001000u)
#define ADC_1_SAR_WRK_MAX_10BIT                        (0x00000400u)
#define ADC_1_SAR_WRK_MAX_8BIT                         (0x00000100u)


/* ***********************SAR_CHAN_RESULT_REG fields ************************ */
#define ADC_1_RESULT_MASK                              (0x0000FFFFLu)
#define ADC_1_SATURATE_INTR_MIR                        (0x20000000Lu)
#define ADC_1_RANGE_INTR_MIR                           (0x40000000Lu)
#define ADC_1_CHAN_RESULT_VALID_MIR                    (0x80000000Lu)


/* ***********************SAR_INTR_MASK_REG fields ************************* */
#define ADC_1_EOS_MASK                                 (0x00000001u)
#define ADC_1_OVERFLOW_MASK                            (0x00000002u)
#define ADC_1_FW_COLLISION_MASK                        (0x00000004u)
#define ADC_1_DSI_COLLISION_MASK                       (0x00000008u)
#define ADC_1_INJ_EOC_MASK                             (0x00000010u)
#define ADC_1_INJ_SATURATE_MASK                        (0x00000020u)
#define ADC_1_INJ_RANGE_MASK                           (0x00000040u)
#define ADC_1_INJ_COLLISION_MASK                       (0x00000080u)

/* ************************  SAR_STATUS_REG fields ************************* */
#define ADC_1_CUR_CHAN_MSK                             (0x0000001Fu)
#define ADC_1_SW_VREF_NEG_MSK                          (0x40000000u)
#define ADC_1_SW_VREF_NEG_SHFT                         (30u)
#define ADC_1_BUSY_MSK                                 (0x80000000u)
#define ADC_1_BUSY_SHFT                                (31u)

/* ***********************SAR_MUX_SWITCH0_REG fields *********************** */
#define ADC_1_MUX_FW_VSSA_VMINUS                       (0x00010000Lu)


/* ***********************SAR_PUMP_CTRL_REG fields *********************** */
#define ADC_1_PUMP_CTRL_ENABLED                        (0x80000000Lu)

/* ************************************************************************** */
/* *******************Global MUX_SWITCH0 Definitions ************************ */
/* ************************************************************************** */
#if(ADC_1_CFG0_VNEG_INPUT_SEL == ADC_1_NEG_SEL_VSSA_KELVIN)
    /* Do not connect VSSA to VMINUS when one channel in differential mode used */
    #if((ADC_1_TOTAL_CHANNELS_NUM == 1u) && (ADC_1_CFG0_SINGLE_PRESENT == 0u))
        #define ADC_1_MUX_SWITCH0_INIT      0u
    #else    /* multiple channels or one single ended channel */
        #define ADC_1_MUX_SWITCH0_INIT      1u
    #endif /* ((ADC_1_TOTAL_CHANNELS_NUM == 1u) && (ADC_1_CFG0_SINGLE_PRESENT == 0u)) */
#else
    #define ADC_1_MUX_SWITCH0_INIT          0u
#endif /* ADC_1_CFG0_VNEG_INPUT_SEL == ADC_1_NEG_SEL_VSSA_KELVIN */

/* ************************************************************************** */
/* Begin configuration 0 calculated defines                                         */
/* ************************************************************************** */

#define ADC_1_CFG0_SAMPLE_TIME01_INIT \
        ((ADC_1_CFG0_APERTURE_TIME0 \
        << ADC_1_SAR_SAMPLE_TIME01_SAMPLE_TIME0_SHFT) \
        | (ADC_1_CFG0_APERTURE_TIME1 \
        << ADC_1_SAR_SAMPLE_TIME01_SAMPLE_TIME1_SHFT))

#define ADC_1_CFG0_SAMPLE_TIME23_INIT \
        ((ADC_1_CFG0_APERTURE_TIME2 \
        << ADC_1_SAR_SAMPLE_TIME23_SAMPLE_TIME2_SHFT) \
        | (ADC_1_CFG0_APERTURE_TIME3 \
        << ADC_1_SAR_SAMPLE_TIME23_SAMPLE_TIME3_SHFT))

/* Enable soc pin if used */
#define ADC_1_CFG0_DSI_TRIGGER_EN_INIT \
        (ADC_1_CFG0_USE_SOC \
        << ADC_1_SAR_SAMPLE_CTRL_DSI_TRIGGER_EN_SHFT)

/* Set soc operation to edge or level based on sample mode */
#if(ADC_1_CFG0_FREERUNNING == ADC_1_FREERUNNING)
    #define ADC_1_CFG0_DSI_TRIGGER_LEVEL_INIT    (ADC_1_SAR_SAMPLE_CTRL_DSI_TRIGGER_LEVEL_MSK)
#else /* Edge trigger */
    #define ADC_1_CFG0_DSI_TRIGGER_LEVEL_INIT    (0u)
#endif /* End ADC_1_CFG0_FREERUNNING == ADC_1_FREERUNNING */

/* Set SE_NEG_INPUT */
#if(ADC_1_CFG0_VNEG_INPUT_SEL == ADC_1_NEG_SEL_VSSA_KELVIN)
    #define ADC_1_CFG0_SE_NEG_INPUT_INIT \
            ((uint32)(ADC_1_NEG_SEL_VSSA_KELVIN \
            << ADC_1_SAR_CTRL_NEG_SEL_SHFT ))
#elif(ADC_1_CFG0_VNEG_INPUT_SEL == ADC_1_NEG_SEL_VREF)
    /* Do not connect VNEG to VREF when one channel in differential mode used */
    /* Only needed for config 0 since 2+ configs == 2+ channels */
    #if((ADC_1_CFG0_CHANNEL_COUNT == 1u) && (ADC_1_CFG0_SINGLE_PRESENT == 0u))
        #define ADC_1_CFG0_SE_NEG_INPUT_INIT     0u
    #else    /* multiple channels or one single channel */
        #define ADC_1_CFG0_SE_NEG_INPUT_INIT     (ADC_1_NEG_SEL_VREF \
                                                            << ADC_1_SAR_CTRL_NEG_SEL_SHFT )
    #endif /* (ADC_1_CFG0_CHANNEL_COUNT == 1u) && (ADC_1_CFG0_CHANNELS_MODE != 0u) */
#elif (ADC_1_CFG0_SINGLE_PRESENT != 0u)
    #define ADC_1_CFG0_SE_NEG_INPUT_INIT         ADC_1_CFG0_NEG_OTHER
#else
    #define ADC_1_CFG0_SE_NEG_INPUT_INIT         0u
#endif /* ADC_1_CFG0_VNEG_INPUT_SEL == ADC_1_NEG_SEL_VSSA_KELVIN */

#if(ADC_1_TOTAL_CHANNELS_NUM > 1u)
    #define ADC_1_CFG0_NEG_OTHER                 (uint16)((uint16)ADC_1_cy_psoc4_sarmux_1__VNEG0 << 9u)
    #define ADC_1_CFG0_SWITCH_CONF_INIT          0u
#else /* Disable SAR sequencer from enabling routing switches in single channel mode */
    #define ADC_1_CFG0_SWITCH_CONF_INIT          ADC_1_SAR_CTRL_SWITCH_DISABLE_MSK
    #define ADC_1_CFG0_NEG_OTHER                 0u
#endif /* ADC_1_CFG0_CHANNEL_COUNT > 1u */

/* If the SAR is configured for multiple channels, always set SAR_HW_CTRL_NEGVREF to 1 */
#if(ADC_1_TOTAL_CHANNELS_NUM == 1u)
    #define ADC_1_CFG0_HW_CTRL_NEGVREF_INIT      0u
#else
    #define ADC_1_CFG0_HW_CTRL_NEGVREF_INIT      ADC_1_SAR_CTRL_SAR_HW_CTRL_NEGVREF_MSK
#endif /* ADC_1_CFG0_CHANNEL_COUNT == 1u */

#define ADC_1_CFG0_POWER_INIT  (ADC_1_PWR_CTRL_VREF_NORMAL_PWR)

/* SAMPLE_CTRL initial values */
#define ADC_1_CFG0_SE_RESULT_FORMAT_INIT \
        (ADC_1_CFG0_SINGLE_ENDED_FORMAT \
        << ADC_1_SAR_SAMPLE_CTRL_SINGLE_ENDED_SIGNED_SHFT)

#define ADC_1_CFG0_DIFF_RESULT_FORMAT_INIT \
        (ADC_1_CFG0_DIFFERENTIAL_FORMAT \
        << ADC_1_SAR_SAMPLE_CTRL_DIFFERENTIAL_SIGNED_SHFT)

#define ADC_1_CFG0_AVG_SAMPLES_NUM_INIT \
        (ADC_1_CFG0_SAMPLES_AVERAGED \
        << ADC_1_SAR_SAMPLE_CTRL_AVG_CNT_SHFT)

/* Combination of Average Mode and Average Shift */
#define ADC_1_CFG0_AVG_MODE_INIT \
        (ADC_1_CFG0_AVGERAGING_MODE \
        << ADC_1_SAR_SAMPLE_CTRL_AVG_SHIFT_SHFT)

/* If using a filter, set TRIGGER_OUT_EN  and SCAN_MODE to scheduled*/
#if(ADC_1_CFG0_FILTER_PRESENT == 1uL)
    #define ADC_1_CFG0_TRIGGER_OUT_INIT \
            (ADC_1_SAR_SAMPLE_CTRL_TRIGGER_OUT_EN_MSK)
    #define ADC_1_CFG0_UAB_SCAN_MODE_INIT \
            (ADC_1_SAR_SAMPLE_CTRL_UAB_SCAN_MODE_MSK)
#else
    #define ADC_1_CFG0_TRIGGER_OUT_INIT      (0u)
    #define ADC_1_CFG0_UAB_SCAN_MODE_INIT    (0u)
#endif

#define ADC_1_CFG0_CTRL_INIT  \
        (ADC_1_CFG0_VREF_SEL_MASK \
        | ADC_1_CFG0_POWER_INIT \
        | ADC_1_SAR_CTRL_REFBUF_EN_MSK \
        | ADC_1_SAR_CTRL_DSI_SYNC_CONFIG_MSK  \
        | ADC_1_CFG0_SWITCH_CONF_INIT \
        | ADC_1_CFG0_SE_NEG_INPUT_INIT \
        | ADC_1_CFG0_HW_CTRL_NEGVREF_INIT )

#define ADC_1_CFG0_SAMPLE_CTRL_INIT \
        (ADC_1_CFG0_SUB_RESOLUTION \
        | ADC_1_CFG0_SE_RESULT_FORMAT_INIT \
        | ADC_1_CFG0_DIFF_RESULT_FORMAT_INIT \
        | ADC_1_CFG0_AVG_SAMPLES_NUM_INIT \
        | ADC_1_CFG0_AVG_MODE_INIT \
        | ADC_1_CFG0_DSI_TRIGGER_LEVEL_INIT \
        | ADC_1_CFG0_DSI_TRIGGER_EN_INIT \
        | ADC_1_CFG0_UAB_SCAN_MODE_INIT \
        | ADC_1_SAR_SAMPLE_CTRL_VALID_IGNORE_MSK \
        | ADC_1_CFG0_TRIGGER_OUT_INIT \
        | ADC_1_SAR_SAMPLE_CTRL_EOS_DSI_OUT_EN_MSK \
)

#define ADC_1_CFG0_RANGE_THRES_INIT \
        (ADC_1_CFG0_LOW_LIMIT \
        | (uint32)((uint32)ADC_1_CFG0_HIGH_LIMIT \
        << ADC_1_SAR_RANGE_THRES_RANGE_HIGH_SHFT))

#define ADC_1_CFG0_RANGE_COND_INIT \
        ((uint32)(ADC_1_CFG0_RANGE_COND \
        << ADC_1_SAR_RANGE_COND_RANGE_COND_SHFT))
/* Misc Config
    [0] - Freerunning: Set if the sample mode is freerunning
    [1] - Filter Present: Set if the configuration uses a UAB filter
*/
#define ADC_1_CFG0_MISC_CONFIG_INIT \
        (ADC_1_CFG0_FREERUNNING \
        | (ADC_1_CFG0_FILTER_PRESENT \
        << ADC_1_MISC_CONFIG_FILTER_PRESENT_SHFT))
/* ************************************************************************** */
/* End configuration 0 calculated defines                                     */
/* ************************************************************************** */

/* ************************************************************************** */
/* Begin configuration 1 calculated defines                                         */
/* ************************************************************************** */

#if(ADC_1_TOTAL_CONFIGS > 1)
    #define ADC_1_CFG1_SAMPLE_TIME01_INIT \
            ((ADC_1_CFG1_APERTURE_TIME0 \
            << ADC_1_SAR_SAMPLE_TIME01_SAMPLE_TIME0_SHFT) \
            | (ADC_1_CFG1_APERTURE_TIME1 \
            << ADC_1_SAR_SAMPLE_TIME01_SAMPLE_TIME1_SHFT))

    #define ADC_1_CFG1_SAMPLE_TIME23_INIT \
            ((ADC_1_CFG1_APERTURE_TIME2 \
            << ADC_1_SAR_SAMPLE_TIME23_SAMPLE_TIME2_SHFT) \
            | (ADC_1_CFG1_APERTURE_TIME3 \
            << ADC_1_SAR_SAMPLE_TIME23_SAMPLE_TIME3_SHFT))

    /* Enable soc pin if used */
    #define ADC_1_CFG1_DSI_TRIGGER_EN_INIT \
            (ADC_1_CFG1_USE_SOC \
            << ADC_1_SAR_SAMPLE_CTRL_DSI_TRIGGER_EN_SHFT)

    /* Set soc operation to edge or level based on sample mode */
    #if(ADC_1_CFG1_FREERUNNING == ADC_1_FREERUNNING)
        #define ADC_1_CFG1_DSI_TRIGGER_LEVEL_INIT    (ADC_1_SAR_SAMPLE_CTRL_DSI_TRIGGER_LEVEL_MSK)
    #else /* Edge trigger */
        #define ADC_1_CFG1_DSI_TRIGGER_LEVEL_INIT    (0u)
    #endif /* End ADC_1_CFG1_FREERUNNING == ADC_1_FREERUNNING */

    /* Set SE_NEG_INPUT */
    #if(ADC_1_CFG1_VNEG_INPUT_SEL == ADC_1_NEG_SEL_VSSA_KELVIN)
        #define ADC_1_CFG1_SE_NEG_INPUT_INIT \
            ((uint32)(ADC_1_NEG_SEL_VSSA_KELVIN \
            << ADC_1_SAR_CTRL_NEG_SEL_SHFT ))
    #elif(ADC_1_CFG1_VNEG_INPUT_SEL == ADC_1_NEG_SEL_VREF)
        #define ADC_1_CFG1_SE_NEG_INPUT_INIT     (ADC_1_NEG_SEL_VREF \
                                                            << ADC_1_SAR_CTRL_NEG_SEL_SHFT )
    #elif (ADC_1_CFG1_SINGLE_PRESENT != 0u)
        #define ADC_1_CFG1_SE_NEG_INPUT_INIT         ADC_1_CFG1_NEG_OTHER
    #else
        #define ADC_1_CFG1_SE_NEG_INPUT_INIT         0u
    #endif /* ADC_1_CFG1_VNEG_INPUT_SEL == ADC_1_NEG_SEL_VSSA_KELVIN */

    #define ADC_1_CFG1_NEG_OTHER                 (uint16)((uint16)ADC_1_cy_psoc4_sarmux_1__VNEG1 << 9u)
    #define ADC_1_CFG1_SWITCH_CONF_INIT          0u

    /* If the SAR is configured for multiple channels, always set SAR_HW_CTRL_NEGVREF to 1 */
    #define ADC_1_CFG1_HW_CTRL_NEGVREF_INIT      ADC_1_SAR_CTRL_SAR_HW_CTRL_NEGVREF_MSK

    #define ADC_1_CFG1_POWER_INIT  (ADC_1_PWR_CTRL_VREF_NORMAL_PWR)

    /* SAMPLE_CTRL initial values */
    #define ADC_1_CFG1_SE_RESULT_FORMAT_INIT \
            (ADC_1_CFG1_SINGLE_ENDED_FORMAT \
            << ADC_1_SAR_SAMPLE_CTRL_SINGLE_ENDED_SIGNED_SHFT)

    #define ADC_1_CFG1_DIFF_RESULT_FORMAT_INIT \
            (ADC_1_CFG1_DIFFERENTIAL_FORMAT \
            << ADC_1_SAR_SAMPLE_CTRL_DIFFERENTIAL_SIGNED_SHFT)

    #define ADC_1_CFG1_AVG_SAMPLES_NUM_INIT \
            (ADC_1_CFG1_SAMPLES_AVERAGED \
            << ADC_1_SAR_SAMPLE_CTRL_AVG_CNT_SHFT)

    /* Combination of Average Mode and Average Shift */
    #define ADC_1_CFG1_AVG_MODE_INIT \
            (ADC_1_CFG1_AVGERAGING_MODE \
            << ADC_1_SAR_SAMPLE_CTRL_AVG_SHIFT_SHFT)

    /* If using a filter, set TRIGGER_OUT_EN  and SCAN_MODE to scheduled*/
    #if(ADC_1_CFG1_FILTER_PRESENT == 1uL)
        #define ADC_1_CFG1_TRIGGER_OUT_INIT \
                (ADC_1_SAR_SAMPLE_CTRL_TRIGGER_OUT_EN_MSK)
        #define ADC_1_CFG1_UAB_SCAN_MODE_INIT \
                (ADC_1_SAR_SAMPLE_CTRL_UAB_SCAN_MODE_MSK)
    #else
        #define ADC_1_CFG1_TRIGGER_OUT_INIT      (0u)
        #define ADC_1_CFG1_UAB_SCAN_MODE_INIT    (0u)
    #endif

    #define ADC_1_CFG1_CTRL_INIT  \
            (ADC_1_CFG1_VREF_SEL_MASK \
            | ADC_1_CFG1_POWER_INIT \
            | ADC_1_SAR_CTRL_REFBUF_EN_MSK \
            | ADC_1_SAR_CTRL_DSI_SYNC_CONFIG_MSK  \
            | ADC_1_CFG1_SWITCH_CONF_INIT \
            | ADC_1_CFG1_SE_NEG_INPUT_INIT \
            | ADC_1_CFG1_HW_CTRL_NEGVREF_INIT )

    #define ADC_1_CFG1_SAMPLE_CTRL_INIT \
            (ADC_1_CFG1_SUB_RESOLUTION \
            | ADC_1_CFG1_SE_RESULT_FORMAT_INIT \
            | ADC_1_CFG1_DIFF_RESULT_FORMAT_INIT \
            | ADC_1_CFG1_AVG_SAMPLES_NUM_INIT \
            | ADC_1_CFG1_AVG_MODE_INIT \
            | ADC_1_CFG1_DSI_TRIGGER_LEVEL_INIT \
            | ADC_1_CFG1_DSI_TRIGGER_EN_INIT \
            | ADC_1_CFG1_UAB_SCAN_MODE_INIT \
            | ADC_1_SAR_SAMPLE_CTRL_VALID_IGNORE_MSK \
            | ADC_1_CFG1_TRIGGER_OUT_INIT \
            | ADC_1_SAR_SAMPLE_CTRL_EOS_DSI_OUT_EN_MSK \
    )

    #define ADC_1_CFG1_RANGE_THRES_INIT \
            (ADC_1_CFG1_LOW_LIMIT \
            | (uint32)((uint32)ADC_1_CFG1_HIGH_LIMIT \
            << ADC_1_SAR_RANGE_THRES_RANGE_HIGH_SHFT))

    #define ADC_1_CFG1_RANGE_COND_INIT \
            ((uint32)(ADC_1_CFG1_RANGE_COND \
            << ADC_1_SAR_RANGE_COND_RANGE_COND_SHFT))
    /* Misc Config
        [0] - Freerunning: Set if the sample mode is freerunning
        [1] - Filter Present: Set if the configuration uses a UAB filter
    */
    #define ADC_1_CFG1_MISC_CONFIG_INIT \
            (ADC_1_CFG1_FREERUNNING \
            | (ADC_1_CFG1_FILTER_PRESENT \
            << ADC_1_MISC_CONFIG_FILTER_PRESENT_SHFT))
#endif /* ADC_1_TOTAL_CONFIGS > 1 */

/* ************************************************************************** */
/* End configuration 1 calculated defines                                     */
/* ************************************************************************** */

/* ************************************************************************** */
/* Begin configuration 2 calculated defines                                         */
/* ************************************************************************** */

#if(ADC_1_TOTAL_CONFIGS > 2)
    #define ADC_1_CFG2_SAMPLE_TIME01_INIT \
            ((ADC_1_CFG2_APERTURE_TIME0 \
            << ADC_1_SAR_SAMPLE_TIME01_SAMPLE_TIME0_SHFT) \
            | (ADC_1_CFG2_APERTURE_TIME1 \
            << ADC_1_SAR_SAMPLE_TIME01_SAMPLE_TIME1_SHFT))

    #define ADC_1_CFG2_SAMPLE_TIME23_INIT \
            ((ADC_1_CFG2_APERTURE_TIME2 \
            << ADC_1_SAR_SAMPLE_TIME23_SAMPLE_TIME2_SHFT) \
            | (ADC_1_CFG2_APERTURE_TIME3 \
            << ADC_1_SAR_SAMPLE_TIME23_SAMPLE_TIME3_SHFT))

    /* Enable soc pin if used */
    #define ADC_1_CFG2_DSI_TRIGGER_EN_INIT \
            (ADC_1_CFG2_USE_SOC \
            << ADC_1_SAR_SAMPLE_CTRL_DSI_TRIGGER_EN_SHFT)

    /* Set soc operation to edge or level based on sample mode */
    #if(ADC_1_CFG2_FREERUNNING == ADC_1_FREERUNNING)
        #define ADC_1_CFG2_DSI_TRIGGER_LEVEL_INIT    (ADC_1_SAR_SAMPLE_CTRL_DSI_TRIGGER_LEVEL_MSK)
    #else /* Edge trigger */
        #define ADC_1_CFG2_DSI_TRIGGER_LEVEL_INIT    (0u)
    #endif /* End ADC_1_CFG2_FREERUNNING == ADC_1_FREERUNNING */

    /* Set SE_NEG_INPUT */
    #if(ADC_1_CFG2_VNEG_INPUT_SEL == ADC_1_NEG_SEL_VSSA_KELVIN)
        #define ADC_1_CFG2_SE_NEG_INPUT_INIT \
            ((uint32)(ADC_1_NEG_SEL_VSSA_KELVIN \
            << ADC_1_SAR_CTRL_NEG_SEL_SHFT ))
    #elif(ADC_1_CFG2_VNEG_INPUT_SEL == ADC_1_NEG_SEL_VREF)
        #define ADC_1_CFG2_SE_NEG_INPUT_INIT     (ADC_1_NEG_SEL_VREF \
                                                                << ADC_1_SAR_CTRL_NEG_SEL_SHFT )
    #elif (ADC_1_CFG2_SINGLE_PRESENT != 0u)
        #define ADC_1_CFG2_SE_NEG_INPUT_INIT         ADC_1_CFG2_NEG_OTHER
    #else
        #define ADC_1_CFG2_SE_NEG_INPUT_INIT         0u
    #endif /* ADC_1_CFG2_VNEG_INPUT_SEL == ADC_1_NEG_SEL_VSSA_KELVIN */

    #define ADC_1_CFG2_NEG_OTHER                 (uint16)((uint16)ADC_1_cy_psoc4_sarmux_1__VNEG2 << 9u)

    #define ADC_1_CFG2_SWITCH_CONF_INIT          0u

    /* If the SAR is configured for multiple channels, always set SAR_HW_CTRL_NEGVREF to 1 */
    #define ADC_1_CFG2_HW_CTRL_NEGVREF_INIT      ADC_1_SAR_CTRL_SAR_HW_CTRL_NEGVREF_MSK

    #define ADC_1_CFG2_POWER_INIT  (ADC_1_PWR_CTRL_VREF_NORMAL_PWR)

    /* SAMPLE_CTRL initial values */
    #define ADC_1_CFG2_SE_RESULT_FORMAT_INIT \
            (ADC_1_CFG2_SINGLE_ENDED_FORMAT \
            << ADC_1_SAR_SAMPLE_CTRL_SINGLE_ENDED_SIGNED_SHFT)

    #define ADC_1_CFG2_DIFF_RESULT_FORMAT_INIT \
            (ADC_1_CFG2_DIFFERENTIAL_FORMAT \
            << ADC_1_SAR_SAMPLE_CTRL_DIFFERENTIAL_SIGNED_SHFT)

    #define ADC_1_CFG2_AVG_SAMPLES_NUM_INIT \
            (ADC_1_CFG2_SAMPLES_AVERAGED \
            << ADC_1_SAR_SAMPLE_CTRL_AVG_CNT_SHFT)

    /* Combination of Average Mode and Average Shift */
    #define ADC_1_CFG2_AVG_MODE_INIT \
            (ADC_1_CFG2_AVGERAGING_MODE \
            << ADC_1_SAR_SAMPLE_CTRL_AVG_SHIFT_SHFT)

    /* If using a filter, set TRIGGER_OUT_EN  and SCAN_MODE to scheduled*/
    #if(ADC_1_CFG2_FILTER_PRESENT == 1uL)
        #define ADC_1_CFG2_TRIGGER_OUT_INIT \
                (ADC_1_SAR_SAMPLE_CTRL_TRIGGER_OUT_EN_MSK)
        #define ADC_1_CFG2_UAB_SCAN_MODE_INIT \
                (ADC_1_SAR_SAMPLE_CTRL_UAB_SCAN_MODE_MSK)
    #else
        #define ADC_1_CFG2_TRIGGER_OUT_INIT      (0u)
        #define ADC_1_CFG2_UAB_SCAN_MODE_INIT    (0u)
    #endif

    #define ADC_1_CFG2_CTRL_INIT  \
            (ADC_1_CFG2_VREF_SEL_MASK \
            | ADC_1_CFG2_POWER_INIT \
            | ADC_1_SAR_CTRL_REFBUF_EN_MSK \
            | ADC_1_SAR_CTRL_DSI_SYNC_CONFIG_MSK  \
            | ADC_1_CFG2_SWITCH_CONF_INIT \
            | ADC_1_CFG2_SE_NEG_INPUT_INIT \
            | ADC_1_CFG2_HW_CTRL_NEGVREF_INIT )


    #define ADC_1_CFG2_SAMPLE_CTRL_INIT \
            (ADC_1_CFG2_SUB_RESOLUTION \
            | ADC_1_CFG2_SE_RESULT_FORMAT_INIT \
            | ADC_1_CFG2_DIFF_RESULT_FORMAT_INIT \
            | ADC_1_CFG2_AVG_SAMPLES_NUM_INIT \
            | ADC_1_CFG2_AVG_MODE_INIT \
            | ADC_1_CFG2_DSI_TRIGGER_LEVEL_INIT \
            | ADC_1_CFG2_DSI_TRIGGER_EN_INIT \
            | ADC_1_CFG2_UAB_SCAN_MODE_INIT \
            | ADC_1_SAR_SAMPLE_CTRL_VALID_IGNORE_MSK \
            | ADC_1_CFG2_TRIGGER_OUT_INIT \
            | ADC_1_SAR_SAMPLE_CTRL_EOS_DSI_OUT_EN_MSK \
    )

    #define ADC_1_CFG2_RANGE_THRES_INIT \
            (ADC_1_CFG2_LOW_LIMIT \
            | (uint32)((uint32)ADC_1_CFG2_HIGH_LIMIT \
            << ADC_1_SAR_RANGE_THRES_RANGE_HIGH_SHFT))

    #define ADC_1_CFG2_RANGE_COND_INIT \
            ((uint32)(ADC_1_CFG2_RANGE_COND \
            << ADC_1_SAR_RANGE_COND_RANGE_COND_SHFT))
    /* Misc Config
        [0] - Freerunning: Set if the sample mode is freerunning
        [1] - Filter Present: Set if the configuration uses a UAB filter
    */
    #define ADC_1_CFG2_MISC_CONFIG_INIT \
            (ADC_1_CFG2_FREERUNNING \
            | (ADC_1_CFG2_FILTER_PRESENT \
            << ADC_1_MISC_CONFIG_FILTER_PRESENT_SHFT))
#endif /* ADC_1_TOTAL_CONFIGS > 2 */

/* ************************************************************************** */
/* End configuration 2 calculated defines                                     */
/* ************************************************************************** */

/* ************************************************************************** */
/* Begin configuration 3 calculated defines                                         */
/* ************************************************************************** */

#if(ADC_1_TOTAL_CONFIGS > 3)
    #define ADC_1_CFG3_SAMPLE_TIME01_INIT \
            ((ADC_1_CFG3_APERTURE_TIME0 \
            << ADC_1_SAR_SAMPLE_TIME01_SAMPLE_TIME0_SHFT) \
            | (ADC_1_CFG3_APERTURE_TIME1 \
            << ADC_1_SAR_SAMPLE_TIME01_SAMPLE_TIME1_SHFT))

    #define ADC_1_CFG3_SAMPLE_TIME23_INIT \
            ((ADC_1_CFG3_APERTURE_TIME2 \
            << ADC_1_SAR_SAMPLE_TIME23_SAMPLE_TIME2_SHFT) \
            | (ADC_1_CFG3_APERTURE_TIME3 \
            << ADC_1_SAR_SAMPLE_TIME23_SAMPLE_TIME3_SHFT))

    /* Enable soc pin if used */
    #define ADC_1_CFG3_DSI_TRIGGER_EN_INIT \
            (ADC_1_CFG3_USE_SOC \
             << ADC_1_SAR_SAMPLE_CTRL_DSI_TRIGGER_EN_SHFT)

    /* Set soc operation to edge or level based on sample mode */
    #if(ADC_1_CFG3_FREERUNNING == ADC_1_FREERUNNING)
        #define ADC_1_CFG3_DSI_TRIGGER_LEVEL_INIT    (ADC_1_SAR_SAMPLE_CTRL_DSI_TRIGGER_LEVEL_MSK)
    #else /* Edge trigger */
        #define ADC_1_CFG3_DSI_TRIGGER_LEVEL_INIT    (0u)
    #endif /* End ADC_1_CFG3_FREERUNNING == ADC_1_FREERUNNING */

    /* Set SE_NEG_INPUT  */
    #if(ADC_1_CFG3_VNEG_INPUT_SEL == ADC_1_NEG_SEL_VSSA_KELVIN)
        #define ADC_1_CFG3_SE_NEG_INPUT_INIT \
            ((uint32)(ADC_1_NEG_SEL_VSSA_KELVIN \
            << ADC_1_SAR_CTRL_NEG_SEL_SHFT ))
    #elif(ADC_1_CFG3_VNEG_INPUT_SEL == ADC_1_NEG_SEL_VREF)
        #define ADC_1_CFG3_SE_NEG_INPUT_INIT     (ADC_1_NEG_SEL_VREF \
                                                                << ADC_1_SAR_CTRL_NEG_SEL_SHFT )
    #elif (ADC_1_CFG3_SINGLE_PRESENT != 0u)
        #define ADC_1_CFG3_SE_NEG_INPUT_INIT         ADC_1_CFG3_NEG_OTHER
    #else
        #define ADC_1_CFG3_SE_NEG_INPUT_INIT         0u
    #endif /* ADC_1_CFG3_VNEG_INPUT_SEL == ADC_1_NEG_SEL_VSSA_KELVIN */

    #define ADC_1_CFG3_NEG_OTHER                 (uint16)((uint16)ADC_1_cy_psoc4_sarmux_1__VNEG3 << 9u)

    #define ADC_1_CFG3_SWITCH_CONF_INIT          0u

    /* If the SAR is configured for multiple channels, always set SAR_HW_CTRL_NEGVREF to 1 */
    #define ADC_1_CFG3_HW_CTRL_NEGVREF_INIT      ADC_1_SAR_CTRL_SAR_HW_CTRL_NEGVREF_MSK

    #define ADC_1_CFG3_POWER_INIT  (ADC_1_PWR_CTRL_VREF_NORMAL_PWR)

    /* SAMPLE_CTRL initial values */
    #define ADC_1_CFG3_SE_RESULT_FORMAT_INIT \
            (ADC_1_CFG3_SINGLE_ENDED_FORMAT \
            << ADC_1_SAR_SAMPLE_CTRL_SINGLE_ENDED_SIGNED_SHFT)

    #define ADC_1_CFG3_DIFF_RESULT_FORMAT_INIT \
            (ADC_1_CFG3_DIFFERENTIAL_FORMAT \
            << ADC_1_SAR_SAMPLE_CTRL_DIFFERENTIAL_SIGNED_SHFT)

    #define ADC_1_CFG3_AVG_SAMPLES_NUM_INIT \
            (ADC_1_CFG3_SAMPLES_AVERAGED \
            << ADC_1_SAR_SAMPLE_CTRL_AVG_CNT_SHFT)

    /* Combination of Average Mode and Average Shift */
    #define ADC_1_CFG3_AVG_MODE_INIT \
            (ADC_1_CFG3_AVGERAGING_MODE \
            << ADC_1_SAR_SAMPLE_CTRL_AVG_SHIFT_SHFT)

    /* If using a filter, set TRIGGER_OUT_EN  and SCAN_MODE to scheduled*/
    #if(ADC_1_CFG3_FILTER_PRESENT == 1uL)
        #define ADC_1_CFG3_TRIGGER_OUT_INIT \
                (ADC_1_SAR_SAMPLE_CTRL_TRIGGER_OUT_EN_MSK)
        #define ADC_1_CFG3_UAB_SCAN_MODE_INIT \
                (ADC_1_SAR_SAMPLE_CTRL_UAB_SCAN_MODE_MSK)
    #else
        #define ADC_1_CFG3_TRIGGER_OUT_INIT      (0u)
        #define ADC_1_CFG3_UAB_SCAN_MODE_INIT    (0u)
    #endif

    #define ADC_1_CFG3_CTRL_INIT  \
            (ADC_1_CFG3_VREF_SEL_MASK \
            | ADC_1_CFG3_POWER_INIT \
            | ADC_1_SAR_CTRL_REFBUF_EN_MSK \
            | ADC_1_SAR_CTRL_DSI_SYNC_CONFIG_MSK  \
            | ADC_1_CFG3_SWITCH_CONF_INIT \
            | ADC_1_CFG3_SE_NEG_INPUT_INIT \
            | ADC_1_CFG3_HW_CTRL_NEGVREF_INIT )


    #define ADC_1_CFG3_SAMPLE_CTRL_INIT \
            (ADC_1_CFG3_SUB_RESOLUTION \
            | ADC_1_CFG3_SE_RESULT_FORMAT_INIT \
            | ADC_1_CFG3_DIFF_RESULT_FORMAT_INIT \
            | ADC_1_CFG3_AVG_SAMPLES_NUM_INIT \
            | ADC_1_CFG3_AVG_MODE_INIT \
            | ADC_1_CFG3_DSI_TRIGGER_LEVEL_INIT \
            | ADC_1_CFG3_DSI_TRIGGER_EN_INIT \
            | ADC_1_CFG3_UAB_SCAN_MODE_INIT \
            | ADC_1_SAR_SAMPLE_CTRL_VALID_IGNORE_MSK \
            | ADC_1_CFG3_TRIGGER_OUT_INIT \
            | ADC_1_SAR_SAMPLE_CTRL_EOS_DSI_OUT_EN_MSK \
    )

    #define ADC_1_CFG3_RANGE_THRES_INIT \
            (ADC_1_CFG3_LOW_LIMIT \
            | (uint32)((uint32)ADC_1_CFG3_HIGH_LIMIT \
            << ADC_1_SAR_RANGE_THRES_RANGE_HIGH_SHFT))

    #define ADC_1_CFG3_RANGE_COND_INIT \
            ((uint32)(ADC_1_CFG3_RANGE_COND \
            << ADC_1_SAR_RANGE_COND_RANGE_COND_SHFT))
    /* Misc Config
        [0] - Freerunning: Set if the sample mode is freerunning
        [1] - Filter Present: Set if the configuration uses a UAB filter
    */
    #define ADC_1_CFG3_MISC_CONFIG_INIT \
            (ADC_1_CFG3_FREERUNNING \
            | (ADC_1_CFG3_FILTER_PRESENT \
            << ADC_1_MISC_CONFIG_FILTER_PRESENT_SHFT))

#endif /* ADC_1_TOTAL_CONFIGS > 3 */

/* ************************************************************************** */
/* End configuration 3 calculated defines                                     */
/* ************************************************************************** */


/*******************************************************************************
*       Configuration Structure Constants
*******************************************************************************/

/* Misc Config
    [0] - Freerunning: Set if the sample mode is freerunning
    [1] - Filter Present: Set if the configuration uses a UAB filter
    [2] - Mux Switch 0: Set when VSSA is used for the neg input to any single-ended channel
*/
#define ADC_1_MISC_CONFIG_FREERUNNING_MSK        0x01u
#define ADC_1_MISC_CONFIG_FILTER_PRESENT_MSK     0x02u
#define ADC_1_MISC_CONFIG_FILTER_PRESENT_SHFT    0x01u
#define ADC_1_MISC_CONFIG_MUX_SWITCH0_MSK        0x04u
#define ADC_1_MISC_CONFIG_MUX_SWITCH0_SHFT       0x02u



#if (0)
#define ADC_1_CFG0_TARGET_SPS 100000
#define ADC_1_CFG0_ACHIEVED_SPS 2757
#define ADC_1_CFG0_ADC_CLOCK_FREQ_HZ 12000000
#define ADC_1_CFG0_VREF_SOURCE VREFSOURCE_VBGR
#define ADC_1_CFG0_VREF_BYPASS True
#define ADC_1_CFG0_VNEG_SOURCE VNEGSOURCE_VREFSHORT
#define ADC_1_CFG0_NUM_SAMPLES_AVERAGED TWOFIFTYSIX_SAMPLES
#define ADC_1_CFG0_AVERAGING_MODE SEQUENTIAL_AVG
#define ADC_1_CFG0_ALT_RES EIGHT_BIT
#define ADC_1_CFG0_FILTER_TYPE LOW_PASS
#define ADC_1_CFG0_FILTER_F1_KHZ 10
#define ADC_1_CFG0_FILTER_F2_KHZ -1
#define ADC_1_CFG0_FILTER_SAMPLE_FREQ_KHZ 1000
#define ADC_1_CFG0_NUM_CHANNELS 1
#define ADC_1_CFG0_CHAN0_ENABLED True
#define ADC_1_CFG0_CHAN0_USE_ALT_RES False
#define ADC_1_CFG0_CHAN0_INPUT_MODE SINGLE_ENDED_CHAN
#define ADC_1_CFG0_CHAN0_USE_AVERAGING True
#define ADC_1_CFG0_CHAN0_MIN_ACQ_TIME_NS 194
#define ADC_1_CFG0_CHAN0_ACHIEVED_ACQ_TIME_NS 208


#endif

#endif /* !defined(CY_ADC_1_H) */
/* [] END OF FILE */
