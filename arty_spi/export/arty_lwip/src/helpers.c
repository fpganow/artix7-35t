/*
 * helpers.c
 *
 *  Created on: Dec 30, 2017
 *      Author: johns
 */



#include "helpers.h"

XGpio gpio_1;
XGpio gpio_2;

//XLlFifo fifo_1;
//XLlFifo fifo_2;

XIntc intController;

//void echoData(XLlFifo *fifo)
//{
//	u32 recv_len;
//	char buffer[MAX_FRAME_SIZE];
//	u32 words[100];
//
//	recv_len = (XLlFifo_iRxGetLen(fifo))/WORD_SIZE;
//	XGpio_DiscreteWrite(&gpio_2, 2, 0x202);
//	XGpio_DiscreteWrite(&gpio_2, 2, recv_len);
//	XGpio_DiscreteWrite(&gpio_2, 2, 0x203);
//	if (recv_len > 0)
//	{
//		XGpio_DiscreteWrite(&gpio_2, 2, 0x204);
//
//		// Receive entire buffer amount
//		XLlFifo_Read(fifo, buffer, (recv_len * WORD_SIZE));
//		XGpio_DiscreteWrite(&gpio_2, 2, 0x210 + recv_len);
//
//		int i;
//		u32 word = 0;
//
//		u32 sumOfValues = 0;
//		for( i = 0; i < recv_len; i++) {
//			int index = (i*4);
//			word =  (buffer[index+3] << 24) |
//					(buffer[index+2] << 16) |
//					(buffer[index+1] <<  8) |
//					(buffer[index+0])
//					;
//
//			XGpio_DiscreteWrite(&gpio_2, 2, i);
//			XGpio_DiscreteWrite(&gpio_2, 2, word);
//
//			words[i] = word;
//			sumOfValues += word;
//		}
//
//		XGpio_DiscreteWrite(&gpio_2, 2, recv_len);
//		XGpio_DiscreteWrite(&gpio_2, 2, 0xFF01);
//		XGpio_DiscreteWrite(&gpio_2, 2, sumOfValues);
//		XGpio_DiscreteWrite(&gpio_2, 2, 0xFF02);
//
//		// Now echo the data back out
//		XGpio_DiscreteWrite(&gpio_2, 2, 0xFE01);
//		if (XLlFifo_iTxVacancy(fifo)) {
//			XGpio_DiscreteWrite(&gpio_2, 2, 0xFE02);
//			for ( i = 0; i < recv_len; i++) {
//				XGpio_DiscreteWrite(&gpio_2, 2, i);
//				XGpio_DiscreteWrite(&gpio_2, 2, words[i]);
//				XLlFifo_TxPutWord(fifo, words[i]);
//			}
//			XGpio_DiscreteWrite(&gpio_2, 2, 0xFE03);
//
//			XLlFifo_iTxSetLen(fifo, (4 * recv_len));
//			XGpio_DiscreteWrite(&gpio_2, 2, 0xFE04);
//		}
//	}
//	XGpio_DiscreteWrite(&gpio_2, 2, 0x220);
//}

void handler_0(void *CallbackRef)
{
	static int counter_0 = 0;

	XGpio_DiscreteWrite(&gpio_2, 2, 0x4000 + counter_0++);
	XIntc_Acknowledge(&intController, 0);
}

void handler_1(void *CallbackRef)
{
	static int counter_1 = 0;

	XGpio_DiscreteWrite(&gpio_2, 2, 0x5000 + counter_1++);
	XIntc_Acknowledge(&intController, 1);
}

int Intc_Setup(u16 DeviceId)
{
	int Status;

	XGpio_DiscreteWrite(&gpio_1, 2, 0x1007);

	/*
	 * Initialize the interrupt controller driver so that it is ready to use.
	 */
	Status = XIntc_Initialize(&intController, DeviceId);
	if (Status != XST_SUCCESS)
	{
		XGpio_DiscreteWrite(&gpio_1, 2, 0xFFFF0005);
		return -1;
	}

	// 0 to XPAR_INTC_MAX_NUM_INTR_INPUTS - 1
	Status = XIntc_Connect(&intController, 0,
					   (XInterruptHandler)handler_0,
					   (void *)0);
	Status = XIntc_Connect(&intController, 1,
						   (XInterruptHandler)handler_1,
						   (void *)0);
	if (Status != XST_SUCCESS)
	{
		XGpio_DiscreteWrite(&gpio_1, 2, 0xFFFF0006);
		return -2;
	}

	/*
	 * Start the interrupt controller such that interrupts are enabled for
	 * all devices that cause interrupts, specify simulation mode so that
	 * an interrupt can be caused by software rather than a real hardware
	 * interrupt.
	 */
	Status = XIntc_Start(&intController, XIN_REAL_MODE);
	if (Status != XST_SUCCESS)
	{
		XGpio_DiscreteWrite(&gpio_1, 2, 0xFFFF0007);
		return -3;
	}

	/*
	 * Enable the interrupt for the device and then cause (simulate) an
	 * interrupt so the handlers will be called.
	 */
	XIntc_Enable(&intController, 0);
	XIntc_Enable(&intController, 1);

	/*
	 * Initialize the exception table.
	 */
	Xil_ExceptionInit();

	/*
	 * Register the interrupt controller handler with the exception table.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler)XIntc_DeviceInterruptHandler,
			(void*) 0);

	/*
	 * Enable exceptions.
	 */
	Xil_ExceptionEnable();

	XGpio_DiscreteWrite(&gpio_1, 2, 0x1008);
	return XST_SUCCESS;
}

int init_all()
{
	/*
	 *  Initialize GPIO #1, and #2
	**/
	// Initialize GPIO #1
	XGpio_Initialize(&gpio_1, XPAR_AXI_GPIO_0_DEVICE_ID);
	XGpio_SetDataDirection(&gpio_1, 1, 0xFFFFFFFF); /* 0xFFFFFFFF means input */

	XGpio_Initialize(&gpio_1, XPAR_AXI_GPIO_0_DEVICE_ID);
	XGpio_SetDataDirection(&gpio_1, 2, 0x0); /* 0 means output */

	// Initialize GPIO #2
	XGpio_Initialize(&gpio_2, XPAR_AXI_GPIO_1_DEVICE_ID);
	XGpio_SetDataDirection(&gpio_2, 1, 0xFFFFFFFF); /* 0xFFFFFFFF means input */

	XGpio_Initialize(&gpio_2, XPAR_AXI_GPIO_1_DEVICE_ID);
	XGpio_SetDataDirection(&gpio_2, 2, 0x0); /* 0 means output */
//	XGpio_DiscreteWrite(&gpio_1, 2, 0x1001);

	XGpio_DiscreteWrite(&gpio_1, 2, 0x1);
	XGpio_DiscreteWrite(&gpio_2, 2, 0x1);

//	/*
//	 *  Initialize FIFOs
//	 *
//	**/
//	int Status;
//	XLlFifo_Config *Config;
//	Status = XST_SUCCESS;
//
//	// Initialize FIFO #1
////	XGpio_DiscreteWrite(&gpio_1, 2, 0x1002);
//	Config = XLlFfio_LookupConfig(XPAR_AXI_FIFO_0_DEVICE_ID);
//	if (!Config)
//	{
////		XGpio_DiscreteWrite(&gpio_1, 2, 0xFFFF0001);
//		return -1;
//	}
////	XGpio_DiscreteWrite(&gpio_1, 2, 0x1003);
//	Status = XLlFifo_CfgInitialize(&fifo_1, Config, Config->BaseAddress);
//	if (Status != XST_SUCCESS)
//	{
//		XGpio_DiscreteWrite(&gpio_1, 2, 0xFFFF0002);
//		return -2;
//	}
//
//	// Initialize FIFO #2
////	XGpio_DiscreteWrite(&gpio_1, 2, 0x1004);
//	Config = XLlFfio_LookupConfig(XPAR_AXI_FIFO_1_DEVICE_ID);
//	if (!Config)
//	{
////		XGpio_DiscreteWrite(&gpio_1, 2, 0xFFFF0003);
//		return -3;
//	}
////	XGpio_DiscreteWrite(&gpio_1, 2, 0x1005);
//	Status = XLlFifo_CfgInitialize(&fifo_2, Config, Config->BaseAddress);
//	if (Status != XST_SUCCESS)
//	{
////		XGpio_DiscreteWrite(&gpio_1, 2, 0xFFFF0004);
//		return -1;
//	}

	/*
	 *  Initialize Interrupts
	 *
		**/
//	XGpio_DiscreteWrite(&gpio_1, 2, 0x1006);
	Intc_Setup(XPAR_INTC_SINGLE_DEVICE_ID);

	XGpio_DiscreteWrite(&gpio_1, 2, 0x3);
	return XST_SUCCESS;
}
