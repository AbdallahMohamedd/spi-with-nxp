/*
 * Copyright 2016-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    YARAB_V1.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_clock.h"
#include "fsl_debug_console.h"
#include "Slave_Center_IF.h"
//#include "fsl_i2c.h"
//#include "Driver_I2C.h"
//#include "fsl_i2c_dma.h"
//#include "IF_LCD.h"
//#include "LCD.h"
//#include "../CMSIS_driver/Driver_I2C.h"


#define BOARD_LED_GPIO BOARD_LED_BLUE_GPIO
#define BOARD_LED_GPIO_PIN BOARD_LED_BLUE_GPIO_PIN


////////////////////////////////////////////////////
/*
uint32_t I2C0_GetFreq(void)
{
    return CLOCK_GetFreq(I2C0_CLK_SRC);
}

uint32_t I2C1_GetFreq(void)
{
    return CLOCK_GetFreq(I2C1_CLK_SRC);
}
void I2C_MasterSignalEvent_t(uint32_t event)
{
      Transfer done
    if (event == ARM_I2C_EVENT_TRANSFER_DONE)
    {
        g_MasterCompletionFlag = true;
    }
}*/
///////////////////////////////////////////////////



void delay_seconds(float seconds)
{
    volatile uint32_t i = 0;
    uint32_t iterations = (uint32_t)(seconds * 800000); // Calculate the number of iterations

    for (i = 0; i < iterations; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

//spi_master_callback_t SPICallback
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */


int main(void)
{
    /* Define the init structure for the output LED pin*/
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput, 0,
    };

    /* Initialize Board */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    #ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    BOARD_InitDebugConsole();
    #endif
    GPIO_PinInit(BOARD_LED_GPIO, BOARD_LED_GPIO_PIN, &led_config);
    yarab_debug;

    /* SPI Initialization */
    SPI_Init(SPI_used);
    //yarab_debug;

    ////////////////////////////////////////////////
  /*
    /* DMAMux init and DMA init
       DMAMUX_Init(I2C_DMAMUX_BASEADDR);
       DMA_Init(I2C_DMA_BASEADDR);

       /*enable DMA0 IRQ and set the priority
       NVIC_EnableIRQ(DMA0_IRQn);
       NVIC_SetPriority(DMA0_IRQn, 1);

       /*Init I2C
       I2C_MASTER.Initialize(I2C_MasterSignalEvent_t);

       I2C_MASTER.PowerControl(ARM_POWER_FULL);

       /*config slave addr
       /*config transmit speed
       I2C_MASTER.Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_STANDARD);
       lcd(50,50,5,25,0);
    ////////////////////////////////////////////////
*/
    PRINTF("SPI Transmission Complete!\n");
    while (1)
    {
    	delay_seconds(5);
    	SPI_WriteRegister(SPI_used, 0x59, 0x284a);
    	//lcd(80,95,1,24,0);
    	yarab_debug;
    	GPIO_TogglePinsOutput(BOARD_LED_GPIO, 1u << BOARD_LED_GPIO_PIN);
    	delay_seconds(5);
    	//lcd(75,95,0.9,25,0);
    	yarab_debug;
    	SPI_WriteRegister(SPI_used, 0xff, 0x59ac);
    	GPIO_TogglePinsOutput(BOARD_LED_GPIO, 1u << BOARD_LED_GPIO_PIN);
    	//lcd(70,95,3,27,1);
    	delay_seconds(5);
    	SPI_WriteRegister(SPI_used, 0x1c, 0x005a);
    	GPIO_TogglePinsOutput(BOARD_LED_GPIO, 1u << BOARD_LED_GPIO_PIN);

    }

    return 0;
}


