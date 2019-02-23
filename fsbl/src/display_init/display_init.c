#include "xstatus.h"
#include "xvtc.h"
#include "xaxivdma.h"

#define DMA_DEVICE_ID	XPAR_AXIVDMA_0_DEVICE_ID
#define XVTC_DEVICE_ID	XPAR_VTC_0_DEVICE_ID

#define FB_ADDR 0x3F000000
#define HSIZE 1920
#define VSIZE 1080

u32 display_init(void)
{
	u32 Status;
	Status = XST_SUCCESS;

	XVtc Vtc;
	XVtc_Config *VtcConfig;
	XAxiVdma Vdma;
	XAxiVdma_Config *VdmaConfig;
	XAxiVdma_DmaSetup VdmaSetup;

    xil_printf("Starting VTC and VDMA\n\r");

    VtcConfig = XVtc_LookupConfig(XVTC_DEVICE_ID);
	if (NULL == VtcConfig) {
		xil_printf("XVtc_LookupConfig failure\r\n");
		return XST_FAILURE;
	}

	Status = XVtc_CfgInitialize(&Vtc, VtcConfig, VtcConfig->BaseAddress);
	if (Status != XST_SUCCESS) {
		xil_printf("XVtc_CfgInitialize failure\r\n");
		return XST_FAILURE;
	}

	XVtc_RegUpdateEnable(&Vtc);
	XVtc_DisableSync(&Vtc);
	XVtc_EnableGenerator(&Vtc);


	VdmaConfig = XAxiVdma_LookupConfig(DMA_DEVICE_ID);
	if (NULL == VdmaConfig) {
		xil_printf("XAxiVdma_LookupConfig failure\r\n");
		return XST_FAILURE;
	}

	Status = XAxiVdma_CfgInitialize(&Vdma, VdmaConfig, VdmaConfig->BaseAddress);
	if (Status != XST_SUCCESS) {
		xil_printf("XAxiVdma_CfgInitialize failure\r\n");
		return XST_FAILURE;
	}

	VdmaSetup.EnableCircularBuf = 1;
	VdmaSetup.EnableFrameCounter = 0;
	VdmaSetup.FixedFrameStoreAddr = 0;
	VdmaSetup.EnableSync = 1;
	VdmaSetup.PointNum = 1;
	VdmaSetup.FrameDelay = 0;
	VdmaSetup.VertSizeInput = VSIZE;
	VdmaSetup.HoriSizeInput = HSIZE * 4;
	VdmaSetup.Stride = HSIZE * 4;

	Status = XAxiVdma_DmaConfig(&Vdma, XAXIVDMA_READ, &VdmaSetup);
	if (Status != XST_SUCCESS) {
		xdbg_printf(XDBG_DEBUG_ERROR,"Read channel config failed %d\r\n", Status);
		return XST_FAILURE;
	}

	VdmaSetup.FrameStoreStartAddr[0] = FB_ADDR;

	Status = XAxiVdma_DmaSetBufferAddr(&Vdma, XAXIVDMA_READ, VdmaSetup.FrameStoreStartAddr);
	if (Status != XST_SUCCESS) {
		xdbg_printf(XDBG_DEBUG_ERROR,"Read channel set buffer address failed %d\r\n", Status);
		return XST_FAILURE;
	}


   Status = XAxiVdma_DmaStart(&Vdma, XAXIVDMA_READ);
   if (Status != XST_SUCCESS)
   {
      xil_printf("Start read transfer failed %d\n\r", Status);
      return XST_FAILURE;
   }

   xil_printf("VTC and VDMA running\n\r");

	return (Status);
}
