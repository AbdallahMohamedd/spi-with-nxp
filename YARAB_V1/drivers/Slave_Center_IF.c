#include "Slave_Center_IF.h"
/* ============================= System Configuration APIs ============================= */


/* ============================= SPI Communication APIs ============================= */
volatile bool spiTransferCompleted = false;
spi_master_handle_t spiHandle;

/* Callback Function */
// Callback Function (Called when the non-blocking SPI transfer is complete)
void SPI_Callback(SPI_Type *base, spi_master_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        // SPI transfer completed successfully
        PRINTF("SPI Transfer Callback - Success! Status: %d\n\r\r", status);
        spiTransferCompleted = true;
    }
    else
    {
        // SPI transfer failed
        PRINTF("SPI Transfer Failed! Status: %d\n\r\r", status);
        spiTransferCompleted = true; // Ensure the flag is set even on failure to prevent infinite loop
    }
}

spi_master_callback_t SPICallback = SPI_Callback;


/* SPI Initialization */
void SPI_Init(SPI_Type *base)
{
	yarab_debug;
	spi_master_config_t masterConfig;

	// Get default configuration
	SPI_MasterGetDefaultConfig(&masterConfig);

	// Customize SPI configuration
	masterConfig.enableMaster = true;
	masterConfig.polarity = kSPI_ClockPolarityActiveHigh;    // CPOL = 0
	masterConfig.phase = kSPI_ClockPhaseFirstEdge;           // CPHA = 0
	masterConfig.direction = kSPI_MsbFirst;                  // MSB first
	masterConfig.baudRate_Bps = 400000;                      // 400 kbps
	masterConfig.pinMode = kSPI_PinModeNormal;               // Normal pin mode
	masterConfig.outputMode = kSPI_SlaveSelectAutomaticOutput; // Auto SS control

	// Enable SPI clock based on the selected SPI module
	if (base == SPI0)
	{
		yarab_debug;
		CLOCK_EnableClock(kCLOCK_Spi0);
		NVIC_EnableIRQ(SPI0_IRQn);
		NVIC_SetPriority(SPI0_IRQn, 2);
	}
	else if (base == SPI1)
	{
		yarab_debug;
		CLOCK_EnableClock(kCLOCK_Spi1);
		NVIC_EnableIRQ(SPI1_IRQn);
		NVIC_SetPriority(SPI1_IRQn, 2);
	}

	// Initialize SPI with configuration
	SPI_MasterInit(base, &masterConfig, CLOCK_GetFreq(kCLOCK_BusClk));
	yarab_debug;
	// Create SPI handle and set callback (optional for non-blocking transmit only)
	SPI_MasterTransferCreateHandle(base, &spiHandle, SPI_Callback, NULL);
	yarab_debug;
	// Enable specific SPI interrupts as needed
	//SPI_EnableInterrupts(base, kSPI_TxEmptyInterruptEnable); // Enable only TxEmpty interrupt
}



void SPI_WriteRegister(SPI_Type *base, uint8_t regAddress, uint16_t data)
{
	uint8_t txFrame[BUFFERSIZE] = {0};
	uint8_t rxFrame[BUFFERSIZE] = {0}; // Rxframe is not used, but it needs to be passed to the transfer function

	// Construct the SPI frame
	txFrame[0] = 0x01;                      // CRC (Placeholder)
	txFrame[1] = 0x23;                      // Write command + Cluster ID
	txFrame[2] = (regAddress & 0x7F) | 0x80;         // Memory Address + Master/Slave Bit
	txFrame[3] = (data >> 8) & 0xFF;          // Data High Byte
	txFrame[4] = data & 0xFF;                   // Data Low Byte

	// Prepare SPI transfer structure
	spi_transfer_t spiXfer;
	spiXfer.txData = txFrame;
	spiXfer.rxData = rxFrame; // Rxframe is not used, but it needs to be passed to the transfer function
	spiXfer.dataSize = sizeof(txFrame);

	// Initiate non-blocking SPI transfer
	spiTransferCompleted = false; // Reset the transfer completion flag
	yarab_debug;
	status_t status = SPI_MasterTransferNonBlocking(base, &spiHandle, &spiXfer);
	//delay_us(100000);
	if (status != kStatus_Success)
	{
		yarab_debug;
		PRINTF("SPI Transfer Failed to Start! Status: %d\n\r\r", status);
		return;
	}

	// Wait for transfer completion with timeout (Optional)
	uint32_t timeout = 1000000; // Adjust timeout value as needed
	while (!spiTransferCompleted && timeout--)
	{
		yarab_debug;
		// Wait for the transfer to complete or timeout
	}

	if (!spiTransferCompleted)
	{
		yarab_debug;
		PRINTF("SPI Write Timeout!\n\r\r");
	}
}



uint16_t SPI_ReadRegister(SPI_Type *base, uint8_t regAddress)
{
    uint8_t txFrame[BUFFERSIZE] = {0};  // Transmit frame (request)
    uint8_t rxFrame[BUFFERSIZE] = {0};  // Receive buffer (response)
    uint16_t receivedData = 0;

    // Construct the 40-bit SPI frame for reading
    txFrame[0] = 0x00;                               // CRC (Placeholder)
    txFrame[1] = 0x01;                               // Read command (Bits[8:11]) + Cluster ID (Bits[12:15])
    txFrame[2] = (regAddress & 0x7F) | 0x80;            // Memory Address (Bits[16:22]) + Master/Slave Bit[23] (Set MSB for read)
    txFrame[3] = 0x00;                               // Placeholder for receiving data (Bits[24:31])
    txFrame[4] = 0x00;                               // Placeholder for receiving data (Bits[32:39])

    // Prepare SPI transfer structure
    spi_transfer_t spiXfer;
    spiXfer.txData = txFrame;
    spiXfer.rxData = rxFrame;
    spiXfer.dataSize = sizeof(txFrame);

    // Initiate non-blocking SPI transfer
    spiTransferCompleted = false;  // Reset flag

    status_t status = SPI_MasterTransferNonBlocking(base, &spiHandle, &spiXfer);

    if (status != kStatus_Success)
    {
        PRINTF("SPI Read Failed to Start! Status: %d\n", status);
        return 0; // Or some other error value
    }
        uint32_t timeout = 1000000;  // Adjust timeout value as needed
        while (!spiTransferCompleted && timeout--)
        {
            // Wait for the transfer to complete or timeout
        }

    if (!spiTransferCompleted)
    {
        PRINTF("SPI Read Timeout!\n");
        return 0;
    }
    // Extract the received 16-bit data from the response frame
    receivedData = (rxFrame[3] << 8) | rxFrame[4];

    // Print the received frame after reading
    //PRINTF("SPI Frame received (READ): ");
    for (int i = 0; i < 5; i++)
    {
        //PRINTF("%02X ", rxFrame[i]);  // Print each byte in HEX format
    }
    //PRINTF("\n");

    return receivedData;
}



/* ============================= System Configuration APIs ============================= */
void setInitialization(uint16_t data)
{
	uint8_t regAddress = INIT_REGISTER;  // INIT register address
	data = 0x10 ;               // CID assignment and enabling initialization --> 0001 0000

	// Send SPI write command to INIT register
	SPI_WriteRegister(SPI_used, regAddress, data);
}


void setSystemGlobalConfig(uint16_t config)
{
	uint8_t regAddress = SYS_CFG_GLOBAL;  // Global System Configuration Register
	uint16_t data = config;     // Configuration value   GP_TEAM_ABDULLAH

	// Send SPI write command
	SPI_WriteRegister(SPI0, regAddress, data);
}


void setSystemConfig1(uint16_t config)
{
	uint8_t regAddress = SYS_CFG1;  // System Configuration Register 1
	uint16_t data = config;

	// Send SPI write command
	SPI_WriteRegister(SPI0, regAddress, data);
}

void setSystemConfig2(uint16_t config)
{
	uint8_t regAddress = SYS_CFG2;  // System Configuration Register 1
	uint16_t data = config;

	// Send SPI write command
	SPI_WriteRegister(SPI0, regAddress, data);
}


uint16_t getSystemDiagnostic()
{
	uint8_t regAddress = 0x05;  // System Diagnostic Register
	uint16_t data = SPI_ReadRegister(SPI0, regAddress);

	return data;  // Return full 16-bit diagnostic information
}



/* ============================= ADC Configuration APIs ============================= */
void setADCConfig()
{
	uint8_t regAddress = ADC_CFG;  // ADC configuration register address
	uint16_t config = 0x0000;      // Example configuration value (change based on datasheet settings)

	SPI_WriteRegister(SPI0, regAddress, config);
}


void setADC2OffsetCompensation()
{
	uint8_t regAddress = ADC2_OFFSET_COMP;  // ADC2 offset compensation register address
	uint16_t offsetValue = 0x0000;  // Example offset value (adjust based on datasheet)

	SPI_WriteRegister(SPI0, regAddress, offsetValue);
}



/* ============================= Voltage Fault Monitoring APIs ============================= */
void setOVUVEnable()
{
	uint8_t regAddress = OV_UV_EN;  // OV/UV enable register address
	uint16_t data = 0xFFFF;         // Enable all cells if 1, disable if 0

	SPI_WriteRegister(SPI0, regAddress, data);
}


uint16_t getCellOVFault()
{
	return SPI_ReadRegister(SPI0, CELL_OV_FLT); // Read overvoltage fault register
}


uint16_t getCellUVFault()
{
	return SPI_ReadRegister(SPI0, CELL_UV_FLT); // Read undervoltage fault register
}



/* ============================= Cell Balancing APIs ============================= */
void setCellBalanceConfig(uint8_t CBx_CFG, float duration)
{
	// Ensure the duration is within the valid range (0.5 min to 511.5 min)
	if (duration < 0.5) duration = 0.5;
	if (duration > 511.5) duration = 511.5;

	uint8_t regAddress = CBx_CFG;  // Use predefined register macro (CB1_CFG - CB14_CFG)

	// Convert duration to register value: duration / 0.5 → gives correct 9-bit value
	uint16_t durationBits = (uint16_t)(duration / 0.5);

	// Construct data: Enable balancing (bit 9 = 1) + duration (bits 0-8)
	uint16_t data = (1 << 9) | (durationBits & 0x1FF);

	// Send SPI write command
	SPI_WriteRegister(SPI_used, regAddress, data);
}

