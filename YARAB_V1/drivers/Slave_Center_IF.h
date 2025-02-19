/*
Graduation Project, Battery Management System
Eng.: Abdullah Mohamed
Component: Slave_Centeer_IF
File:student.c
*/

#define yarab_debug
#ifdef yarab_debug
#define yarab_debug PRINTF("\nyarab_debug: Func: %s, File: %s, Line: %d \n \r\r", __func__, __FILE__, __LINE__);
#endif


#include <stdio.h>       // Required for printf
#include <stdint.h>      // Required for fixed-width integer types
#include "fsl_spi.h"     // Required for SPI driver functions
#include "MKL25Z4.h"
#include "fsl_debug_console.h"

#define BUFFERSIZE 5

//#define yarab_print  // Uncomment this line to enable printing
//#define yarab_SPI
#define SPI_used SPI0
#ifndef SLAVE_CENTER_IF_H
#define SLAVE_CENTER_IF_H



/*=====================================================================================*/
/*============================== Initialization Register ==============================*/
/*=====================================================================================*/
/*
    The initialization register is used to configure the initial settings of the device.
    This includes enabling or disabling specific features during startup.
*/
#define INIT_REGISTER                       0x01          // Device initialization register

/*============================================================================================*/
/*============================== System Configuration Registers ==============================*/
/*============================================================================================*/
/*
    The system configuration registers control various global settings of the device,
    including power management and communication settings.
*/
#define SYS_CFG_GLOBAL                      0x02          // Global system configuration register
#define SYS_CFG1                            0x03          // System configuration register 1
#define SYS_CFG2                            0x04          // System configuration register 2
#define SYS_DIAG                            0x05          // System diagnostic register

/*=========================================================================================*/
/*============================== ADC Configuration Registers ==============================*/
/*=========================================================================================*/
/*
    The ADC configuration registers control the settings for the analog-to-digital converter (ADC),
    which is responsible for measuring voltages and currents within the system.
*/
#define ADC_CFG                             0x06          // ADC configuration register
#define ADC2_OFFSET_COMP                    0x07          // ADC2 offset compensation register

/*====================================================================================================*/
/*============================== Voltage and Fault Monitoring Registers ==============================*/
/*====================================================================================================*/
/*
    These registers manage fault detection related to voltage levels,
    such as overvoltage (OV) and undervoltage (UV) conditions.
*/
#define OV_UV_EN                            0x08          // Overvoltage/Undervoltage enable register
#define CELL_OV_FLT                         0x09          // Overvoltage fault register
#define CELL_UV_FLT                         0x0A          // Undervoltage fault register

/*====================================================================================================*/
/*============================== Cell Balancing Configuration Registers ==============================*/
/*====================================================================================================*/
/*
    These registers configure the cell balancing feature, allowing control over individual cell balancing drivers.
*/
#define CB1_CFG                             0x0C          // Cell 1 balance configuration register
#define CB2_CFG                             0x0D          // Cell 2 balance configuration register
#define CB3_CFG                             0x0E          // Cell 3 balance configuration register
#define CB4_CFG                             0x0F          // Cell 4 balance configuration register
#define CB5_CFG                             0x10          // Cell 5 balance configuration register
#define CB6_CFG                             0x11          // Cell 6 balance configuration register
#define CB7_CFG                             0x12          // Cell 7 balance configuration register
#define CB8_CFG                             0x13          // Cell 8 balance configuration register
#define CB9_CFG                             0x14          // Cell 9 balance configuration register
#define CB10_CFG                            0x15          // Cell 10 balance configuration register
#define CB11_CFG                            0x16          // Cell 11 balance configuration register
#define CB12_CFG                            0x17          // Cell 12 balance configuration register
#define CB13_CFG                            0x18          // Cell 13 balance configuration register
#define CB14_CFG                            0x19          // Cell 14 balance configuration register

// Bit Definitions for CBx_CFG
#define CB_CFG_ENABLE                       1             // Enable cell balancing
#define CB_CFG_DISABLE                      0             // Disable cell balancing
#define CB_CFG_DURATION_MASK                0x1FF         // Mask for balancing duration (9 bits)
#define CB_CFG_DURATION_SHIFT               0             // No shift needed, will be handled in function

/*==========================================================================================*/
/*============================== Cell Balance Fault Registers ==============================*/
/*==========================================================================================*/
/*
    These registers store fault conditions related to cell balancing,
    including open and short circuit faults.
*/
#define CB_OPEN_FLT                         0x1A          // Open circuit fault detection for cell balancing
#define CB_SHORT_FLT                        0x1B          // Short circuit fault detection for cell balancing
#define CB_DRV_STS                          0x1C          // Cell balancing driver status register

// Bit Definitions for CB_OPEN_FLT
#define CB_OPEN_FLT_DETECTED              (1 << 0)        // Open fault detected

// Bit Definitions for CB_SHORT_FLT
#define CB_SHORT_FLT_DETECTED             (1 << 0)        // Short fault detected

// Bit Definitions for CB_DRV_STS
#define CB_DRV_ACTIVE                     (1 << 0)        // Cell balancing driver active

/*==========================================================================================*/
/*============================== GPIO Configuration Registers ==============================*/
/*==========================================================================================*/
/*
    These registers configure the GPIO (General Purpose Input/Output) ports,
    allowing control over the functionality and behavior of the GPIO pins.
*/
#define GPIO_CFG1                           0x1D          // GPIO configuration register 1
#define GPIO_CFG2                           0x1E          // GPIO configuration register 2
#define GPIO_STS                            0x1F          // GPIO status register

// Bit Definitions for GPIO_CFG1
#define GPIO_CFG1_GPIO6_CFG             (0x03 << 12)      // Configuration bits for GPIO6
#define GPIO_CFG1_GPIO5_CFG             (0x03 << 10)      // Configuration bits for GPIO5
#define GPIO_CFG1_GPIO4_CFG             (0x03 << 8 )      // Configuration bits for GPIO4
#define GPIO_CFG1_GPIO3_CFG             (0x03 << 6 )      // Configuration bits for GPIO3
#define GPIO_CFG1_GPIO2_CFG             (0x03 << 4 )      // Configuration bits for GPIO2
#define GPIO_CFG1_GPIO1_CFG             (0x03 << 2 )      // Configuration bits for GPIO1
#define GPIO_CFG1_GPIO0_CFG             (0x03 << 0 )      // Configuration bits for GPIO0

// Configuration options for GPIOx_CFG
#define GPIO_CFG_ANALOG_RATIO               0x00          // GPIO configured as analog input for ratiometric measurement
#define GPIO_CFG_ANALOG_ABS                 0x01          // GPIO configured as analog input for absolute measurement
#define GPIO_CFG_DIGITAL_IN                 0x02          // GPIO configured as digital input
#define GPIO_CFG_DIGITAL_OUT                0x03          // GPIO configured as digital output

// Bit Definitions for GPIO_CFG2
#define GPIO_CFG2_GPIO0_WU                (1 << 0)        // GPIO0 Wake-up Enable
#define GPIO_CFG2_GPIO2_SOC               (1 << 2)        // GPIO2 Start-of-Conversion Enable
#define GPIO_CFG2_GPIO0_FLT_ACT           (1 << 4)        // GPIO0 Fault Activate Enable

/*=======================================================================================*/
/*============================== Short/Open Diag Registers ==============================*/
/*=======================================================================================*/
/*
 These registers check for short and open diagnostics
*/
#define GPIO_SHORT_ANX_OPEN_STS             0x21          // GPIO short/ open diagnostic status
#define I_STATUS                            0x22          // PGA DAC value

// Bit Definitions for GPIO_SHORT_ANX_OPEN_STS register
#define GPIO_SHORT_ANX_OPEN_STS_ANX_OPEN  (1 << 0)        // ANx OPEN

/*============================================================================================*/
/*============================== Communication status Registers ==============================*/
/*============================================================================================*/
/*
 These register has communication number of crc error count
*/
#define COM_STATUS                          0x23          // Number of CRC error counted

/*====================================================================================*/
/*============================== Fault status Registers ==============================*/
/*====================================================================================*/
/*
  These Registers shows the fault statues for different components.
*/
#define FAULT1_STATUS                       0x24          // Fault status 1
#define FAULT2_STATUS                       0x25          // Fault status 2
#define FAULT3_STATUS                       0x26          // Fault status 3

// Bit Definitions for FAULT1_STATUS register
#define FAULT1_STATUS_COM_ERR_OVR_FLT     (1 << 0)        // COM ERR OVR FLT fault bit
#define FAULT1_STATUS_VPWR_OV_FLT         (1 << 1)        // VPWR overvoltage fault bit
#define FAULT1_STATUS_VPWR_LV_FLT         (1 << 2)        // VPWR low voltage fault bit
#define FAULT1_STATUS_OT_FLT              (1 << 3)        // Overtemperature fault bit (from analog inputs)

// Bit Definitions for FAULT2_STATUS register
#define FAULT2_STATUS_GND_LOSS_FLT        (1 << 0)        // Loss of ground detected
#define FAULT2_STATUS_CB_SHORT_FLT        (1 << 1)        // Cell balance short circuit fault
#define FAULT2_STATUS_IC_TSD_FLT          (1 << 2)        // IC thermal shutdown fault
#define FAULT2_STATUS_VCOM_UV_FLT         (1 << 3)        // VCOM undervoltage fault

// Bit Definitions for FAULT3_STATUS register
#define FAULT3_STATUS_SAMP_OVF            (1 << 0)        // Sample overflow fault
#define FAULT3_STATUS_CC_OVR_FLT          (1 << 1)        // Coulomb counter overflow fault

/*==================================================================================*/
/*============================== FAULT MASK Registers ==============================*/
/*==================================================================================*/
/*
  These Registers used to mask the fault registers.
*/
#define FAULT_MASK1                         0x27          // Fault mask 1
#define FAULT_MASK2                         0x28          // Fault mask 2
#define FAULT_MASK3                         0x29          // Fault mask 3

// Bit Definitions for FAULT_MASK1 register
#define FAULT_MASK1_COM_ERR_OVR_FLT       (1 << 0)        // COM ERR OVR FLT fault bit mask
#define FAULT_MASK1_VPWR_OV_FLT           (1 << 1)        // VPWR overvoltage fault bit mask
#define FAULT_MASK1_VPWR_LV_FLT           (1 << 2)        // VPWR low voltage fault bit mask
#define FAULT_MASK1_OT_FLT                (1 << 3)        // Overtemperature fault bit mask (from analog inputs)

// Bit Definitions for FAULT_MASK2 register
#define FAULT_MASK2_GND_LOSS_FLT          (1 << 0)        // Loss of ground detected fault bit mask
#define FAULT_MASK2_CB_SHORT_FLT          (1 << 1)        // cell balance short circuit fault bit mask
#define FAULT_MASK2_IC_TSD_FLT            (1 << 2)        // thermal shutdown fault bit mask
#define FAULT_MASK2_VCOM_UV_FLT           (1 << 3)        // VCOM undervoltage fault bit mask
#define FAULT_MASK2_10F                   (1 << 4)        // Bit-4 Unused
#define FAULT_MASK2_9F                    (1 << 5)        // Bit-5 Unused
#define FAULT_MASK2_8F                    (1 << 6)        // Bit-6 Unused
#define FAULT_MASK2_7F                    (1 << 7)        // Bit-7 Unused

// Bit Definitions for FAULT_MASK3 register
#define FAULT_MASK3_15F                   (1 << 0)        // Bit-0 Unused
#define FAULT_MASK3_14F                   (1 << 1)        // Bit-1 Unused
#define FAULT_MASK3_13F                   (1 << 2)        // Bit-2 Unused
#define FAULT_MASK3_12F                   (1 << 3)        // Bit-3 Unused
#define FAULT_MASK3_11F                   (1 << 4)        // Bit-4 Unused
#define FAULT_MASK3_10F                   (1 << 5)        // Bit-5 Unused
#define FAULT_MASK3_9F                    (1 << 6)        // Bit-6 Unused
#define FAULT_MASK3_8F                    (1 << 7)        // Bit-7 Unused

/*===================================================================================*/
/*============================== WAKEUP MASK Registers ==============================*/
/*===================================================================================*/
/*
  These Registers used to mask wakeup events.
*/
#define WAKEUP_MASK1                        0x2A          // WAKEUP mask 1
#define WAKEUP_MASK2                        0x2B          // WAKEUP mask 2
#define WAKEUP_MASK3                        0x2C          // WAKEUP mask 3

// Bit Definitions for WAKEUP_MASK1 register
#define WAKEUP_MASK1_OT_OT_FLT            (1 << 0)        // Overtemperature fault bit mask
#define WAKEUP_MASK1_VPWR_OV_FLT          (1 << 1)        // VPWR overvoltage fault bit mask
#define WAKEUP_MASK1_VPWR_LV_FLT          (1 << 2)        // VPWR low voltage fault bit mask
#define WAKEUP_MASK1_COM_ERR_OVR_FLT      (1 << 3)        // Communication error Overflow fault bit mask

// Bit Definitions for WAKEUP_MASK2 register
#define WAKEUP_MASK2_CB_SHORT_FLT         (1 << 0)        // Cell Balance short circuit fault bit mask
#define WAKEUP_MASK2_IC_TSD_FLT           (1 << 1)        // IC thermal shutdown fault bit mask
#define WAKEUP_MASK2_VCOM_UV_FLT          (1 << 2)        // VCOM undervoltage fault bit mask

// Bit Definitions for WAKEUP_MASK3 register
#define WAKEUP_MASK3_CC_OVF               (1 << 0)        //  Coulomb count overflow bit mask

/*=======================================================================================================*/
/*============================== Coulomb count number of samples Registers ==============================*/
/*=======================================================================================================*/
/*
 This register used to count coulomb count number of samples
*/
#define CC_NB_SAMPLES                       0x2D          // Number of samples in coulomb counter

/*====================================================================================================*/
/*============================== Coulomb Counting Accumulator Registers ==============================*/
/*====================================================================================================*/
/*
 These register used to Accumulated Samples
*/
#define COULOMB_CNT1                        0x2E          // Coulomb counting accumulator MSB
#define COULOMB_CNT2                        0x2F          // Coulomb counting accumulator LSB

/*===========================================================================================*/
/*============================== Current Measurement Registers ==============================*/
/*===========================================================================================*/
/*
 These register used to to measure the the current .
*/
#define MEAS_ISENSE1                        0x30          // ISENSE measurement
#define MEAS_ISENSE2                        0x31          // ISENSE measurement

/*=================================================================================================*/
/*============================== Stack Voltage Measurement Registers ==============================*/
/*=================================================================================================*/
/*
 This register Stack voltage measurement
*/
#define MEAS_STACK                          0x32          // Stack voltage measurement

/*================================================================================================*/
/*============================== Cell Voltage Measurement Registers ==============================*/
/*================================================================================================*/
/*
 These register used to measure cell voltage.
*/
#define MEAS_CELL14                         0x33          // Cell 14 voltage measurement
#define MEAS_CELL13                         0x34          // Cell 13 voltage measurement
#define MEAS_CELL12                         0x35          // Cell 12 voltage measurement
#define MEAS_CELL11                         0x36          // Cell 11 voltage measurement
#define MEAS_CELL10                         0x37          // Cell 10 voltage measurement
#define MEAS_CELL9                          0x38          // Cell 9 voltage measurement
#define MEAS_CELL8                          0x39          // Cell 8 voltage measurement
#define MEAS_CELL7                          0x3A          // Cell 7 voltage measurement
#define MEAS_CELL6                          0x3B          // Cell 6 voltage measurement
#define MEAS_CELL5                          0x3C          // Cell 5 voltage measurement
#define MEAS_CELL4                          0x3D          // Cell 4 voltage measurement
#define MEAS_CELL3                          0x3E          // Cell 3 voltage measurement
#define MEAS_CELL2                          0x3F          // Cell 2 voltage measurement
#define MEAS_CELL1                          0x40          // Cell 1 voltage measurement

/*===============================================================================================*/
/*============================== AN6 voltage measurement Registers ==============================*/
/*===============================================================================================*/
/*
   ANX is analog input
*/
#define MEAS_AN6                            0x41          // AN6 voltage measurement

/*===============================================================================================*/
/*============================== AN5 voltage measurement Registers ==============================*/
/*===============================================================================================*/
/*
   ANX is analog input
*/
#define MEAS_AN5                            0x42          // AN5 voltage measurement

/*===============================================================================================*/
/*============================== AN4 voltage measurement Registers ==============================*/
/*===============================================================================================*/
/*
   ANX is analog input
*/
#define MEAS_AN4                            0x43          // AN4 voltage measurement

/*===============================================================================================*/
/*============================== AN3 voltage measurement Registers ==============================*/
/*===============================================================================================*/
/*
   ANX is analog input
*/
#define MEAS_AN3                            0x44          // AN3 voltage measurement

/*===============================================================================================*/
/*============================== AN2 voltage measurement Registers ==============================*/
/*===============================================================================================*/
/*
   ANX is analog input
*/
#define MEAS_AN2                            0x45          // AN2 voltage measurement

/*===============================================================================================*/
/*============================== AN1 voltage measurement Registers ==============================*/
/*===============================================================================================*/
/*
   ANX is analog input
*/
#define MEAS_AN1                            0x46          // AN1 voltage measurement

/*===============================================================================================*/
/*============================== ANO voltage measurement Registers ==============================*/
/*===============================================================================================*/
/*
   ANX is analog input
*/
#define MEAS_ANO                            0x47          // ANO voltage measurement

/*==================================================================================================*/
/*============================== IC temperature measurement Registers ==============================*/
/*==================================================================================================*/
/*
    These register IC temperature measurement
*/
#define MEAS_IC_TEMP                        0x48          // IC temperature measurement

/*============================================================================================================*/
/*============================== ADCIA band gap reference measurement Registers ==============================*/
/*============================================================================================================*/
/*
    These register isADCIA band gap reference measurement
*/
#define MEAS_VBG_DIAG_ADC1A                 0x49          // ADCIA band gap reference measurement

/*============================================================================================================*/
/*============================== ADCIB band gap reference measurement Registers ==============================*/
/*============================================================================================================*/
/*
    These register isADCIB band gap reference measurement
*/
#define MEAS_VBG_DIAG_ADC1B                 0x4A          // ADCIB band gap reference measurement

/*===========================================================================================================*/
/*============================== CTx over and undervoltage threshold Registers ==============================*/
/*===========================================================================================================*/
/*
    These register is CTx over and undervoltage threshold Registers
*/
#define TH_ALL_CT                           0x4B          // CTx over and undervoltage threshold
#define TH_CT14                             0x4C          // CT14 over and undervoltage threshold
#define TH_CT13                             0x4D          // CT13 over and undervoltage threshold
#define TH_CT12                             0x4E          // CT12 over and undervoltage threshold
#define TH_CT11                             0x4F          // CT11 over and undervoltage threshold
#define TH_CT10                             0x50          // CT10 over and undervoltage threshold
#define TH_CT9                              0x51          // CT9 over and undervoltage threshold
#define TH_CT8                              0x52          // CT8 over and undervoltage threshold
#define TH_CT7                              0x53          // CT7 over and undervoltage threshold
#define TH_CT6                              0x54          // CT6 over and undervoltage threshold
#define TH_CT5                              0x55          // CT5 over and undervoltage threshold
#define TH_CT4                              0x56          // CT4 over and undervoltage threshold
#define TH_CT3                              0x57          // CT3 over and undervoltage threshold
#define TH_CT2                              0x58          // CT2 over and undervoltage threshold
#define TH_CT1                              0x59          // CT1 over and undervoltage threshold

/*=================================================================================================*/
/*============================== overtemperature threshold Registers ==============================*/
/*=================================================================================================*/
/*
    These register is TH_ANx_OT overtemperature threshold Registers
*/
#define TH_AN6_OT                           0x5A          // AN6 overtemperature threshold
#define TH_AN5_OT                           0x5B          // AN5 overtemperature threshold
#define TH_AN4_OT                           0x5C          // AN4 overtemperature threshold
#define TH_AN3_OT                           0x5D          // AN3 overtemperature threshold
#define TH_AN2_OT                           0x5E          // AN2 overtemperature threshold
#define TH_AN1_OT                           0x5F          // AN1 overtemperature threshold
#define TH_ANO_OT                           0x60          // ANO overtemperature threshold

/*==================================================================================================*/
/*============================== undertemperature threshold Registers ==============================*/
/*==================================================================================================*/
/*
   These register is ANX undertemperature threshold  Registers
*/
#define TH_AN6_UT                           0x61          // AN6 undertemperature threshold
#define TH_AN5_UT                           0x62          // AN5 undertemperature threshold
#define TH_AN4_UT                           0x63          // AN4 undertemperature threshold
#define TH_AN3_UT                           0x64          // AN3 undertemperature threshold
#define TH_AN2_UT                           0x65          // AN2 undertemperature threshold
#define TH_AN1_UT                           0x66          // AN1 undertemperature threshold
#define TH_ANO_UT                           0x67          // ANO undertemperature threshold

/*====================================================================================================*/
/*============================== ISENSE overcurrent threshold Registers ==============================*/
/*====================================================================================================*/
/*
 This register ISENSE overcurrent threshold Register
*/
#define TH_ISENSE_OC                        0x68          // ISENSE overcurrent threshold
// Bit Definitions for TH_ISENSE_OC
#define TH_ISENSE_OC_THRESHOLD_MASK         0xFFFF        // Threshold value

/*=======================================================================================================*/
/*============================== Coulomb counter threshold (MSB) Registers ==============================*/
/*=======================================================================================================*/
/*
  This register over Coulomb counter threshold
*/
#define TH_COULOMB_CNT_MSB                  0x69          // Coulomb counter threshold (MSB)
// Bit Definitions for TH_COULOMB_CNT_MSB
#define TH_COULOMB_CNT_MSB_VAL_MASK         0xFFFF        // MSB value
#define TH_COULOMB_CNT_MSB_VAL_SHIFT        0             // no shift

/*=======================================================================================================*/
/*============================== Coulomb counter threshold (LSB) Registers ==============================*/
/*=======================================================================================================*/
/*
   This register over Coulomb counter threshold
*/
#define TH_COULOMB_CNT_LSB                  0x6A          // Coulomb counter threshold (LSB)
// Bit Definitions for TH_COULOMB_CNT_LSB
#define TH_COULOMB_CNT_LSB_VAL_MASK         0xFFFF        // LSB Value
#define TH_COULOMB_CNT_LSB_VAL_SHIFT        0             // No Shift

/*========================================================================================*/
/*============================== Silicon revision Registers ==============================*/
/*========================================================================================*/
/*
   This register Silicon revision
*/
#define SILICON_REV                         0x6B          // Silicon revision
// Bit Definitions for SILICON_REV
#define SILICON_REV_MASK                    0xFFFF        // Silicon revision mask
#define SILICON_REV_SHIFT                   0             // no shift




 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*========================================================================================*/
/****************************************** APIs ******************************************/
/*========================================================================================*/


/* ============================= SPI Communication Functions ============================= */
/**
 * @brief Writes a 16-bit value to a specific register on the MC33771B via SPI.
 *        The SPI transaction consists of a 40-bit frame as required by MC33771B.
 * @param base SPI peripheral instance (e.g., SPI0)
 * @param regAddress 7-bit register address to write to
 * @param data 16-bit data to be written
 */
void SPI_WriteRegister(SPI_Type *base, uint8_t regAddress, uint16_t data);

/**
 * @brief Reads a 16-bit value from a specific register on the MC33771B via SPI.
 *        The SPI transaction consists of a 40-bit frame, but only the 16-bit memory data field is returned.
 * @param base SPI peripheral instance (e.g., SPI0)
 * @param regAddress 7-bit register address to read from
 * @return 16-bit data read from the register
 */
uint16_t SPI_ReadRegister(SPI_Type *base, uint8_t regAddress);


/* ============================= System Configuration APIs ============================= */
/**
 * @brief Enables or disables initialization of the MC33771B.
 * @param enable Set to 0x10 to enable, 0x00 to disable
 */
void setInitialization(uint16_t data);

/**
 * @brief Configures global system settings.
 * @param config Configuration value to be set
 */
void setSystemGlobalConfig(uint16_t config);

/**
 * @brief Configures System Configuration Register 1.
 * @param config Configuration value to be set
 */
void setSystemConfig1(uint16_t config);

/**
 * @brief Configures System Configuration Register 2.
 * @param config Configuration value to be set
 */
void setSystemConfig2(uint16_t config);

/**
 * @brief Reads the system diagnostic register.
 * @return Diagnostic register value
 */
uint16_t getSystemDiagnostic();


/* ============================= ADC Configuration APIs ============================= */
/**
 * @brief Configures ADC settings.
 */
void setADCConfig();

/**
 * @brief Sets ADC2 offset compensation.
 */
void setADC2OffsetCompensation();


/* ============================= Voltage Fault Monitoring APIs ============================= */
/**
 * @brief Enables or disables overvoltage/undervoltage detection.
 */
void setOVUVEnable();

/**
 * @brief Reads overvoltage fault status.
 * @return Fault status value
 */
uint16_t getCellOVFault();

/**
 * @brief Reads undervoltage fault status.
 * @return Fault status value
 */
uint16_t getCellUVFault();

/* ============================= Cell Balancing APIs ============================= */
/**
 * @brief Configures cell balancing settings for a specific cell.
 * @param cellNumber The cell number (1-14)
 * @param duration Duration in milliseconds
 */
void setCellBalanceConfig(uint8_t CBx_CFG, float duration);

/**
 * @brief Reads open circuit fault status for cell balancing.
 * @return Fault status value
 */
uint16_t getCellBalanceOpenFault();

/**
 * @brief Reads short circuit fault status for cell balancing.
 * @return Fault status value
 */
uint16_t getCellBalanceShortFault();

/**
 * @brief Reads the status of the cell balancing driver.
 * @return Driver status value
 */
uint16_t getCellBalanceDriverStatus();

/* ============================= GPIO Configuration APIs ============================= */

/**
 * @brief Configures GPIO settings for GPIO configuration register 1.
 * @param config Configuration value
 */
void setGPIOConfig1(uint16_t config);

/**
 * @brief Configures GPIO settings for GPIO configuration register 2.
 * @param config Configuration value
 */
void setGPIOConfig2(uint8_t config);

/**
 * @brief Reads GPIO status register.
 * @return GPIO status value
 */
uint8_t getGPIOStatus();

/* ============================= Fault & Wakeup Masking APIs ============================= */

/**
 * @brief Sets fault mask register 1.
 * @param mask Fault mask value
 */
void setFaultMask1(uint8_t mask);

/**
 * @brief Sets fault mask register 2.
 * @param mask Fault mask value
 */
void setFaultMask2(uint8_t mask);

/**
 * @brief Sets fault mask register 3.
 * @param mask Fault mask value
 */
void setFaultMask3(uint8_t mask);

/**
 * @brief Sets wakeup mask register 1.
 * @param mask Wakeup mask value
 */
void setWakeupMask1(uint8_t mask);

/* ============================= Voltage, Current & Temperature Measurement APIs ============================= */
/**
 * @brief Reads the voltage measurement for a specific cell.
 * @param cellNumber The cell number (1-14)
 * @return Measured voltage in mV
 */
uint16_t getCellVoltage(uint8_t cellNumber);

/**
 * @brief Reads the stack voltage measurement.
 * @return Stack voltage in mV
 */
uint16_t getStackVoltage();

/**
 * @brief Reads the current measurement.
 * @return Current measurement in mA
 */

uint16_t getCurrentMeasurement();
/**
 * @brief Reads the internal IC temperature measurement.
 * @return Temperature in degrees Celsius
 */
uint16_t getICTemperature();

/* ============================= Coulomb Counter & Threshold Configuration ============================= */

/**
 * @brief Sets Coulomb counter threshold (MSB part).
 * @param threshold Threshold value
 */
void setCoulombCounterThresholdMSB(uint16_t threshold);

/**
 * @brief Sets Coulomb counter threshold (LSB part).
 * @param threshold Threshold value
 */
void setCoulombCounterThresholdLSB(uint16_t threshold);

/**
 * @brief Reads the silicon revision of the chip.
 * @return Silicon revision ID
 */
uint16_t getSiliconRevision();


#endif                                                    // SLAVE_CENTER_IF_H
