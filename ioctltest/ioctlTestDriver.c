#include "ntddk.h"
#include "ioctlTest.h"

/*Prototypes*/
VOID OnUnload(PDRIVER_OBJECT);
NTSTATUS OnStubDispatch(PDEVICE_OBJECT, PIRP);
BOOLEAN isStringTerminated(PCHAR, UINT, UINT*);
NTSTATUS deviceControl(PDEVICE_OBJECT,PIRP);
NTSTATUS handleIOCTLDirectOutIo(PIRP,PIO_STACK_LOCATION , UINT*);

/* Constants to name the driver */
const WCHAR deviceNameBuffer[] = L"\\Device\\IOCTLTestDriver"; 
const WCHAR deviceLinkBuffer[] = L"\\DosDevices\\IOCTLTestDriver";

/*Global pointer to device*/
PDEVICE_OBJECT g_ioctlTestDevice; 

NTSTATUS DriverEntry( PDRIVER_OBJECT driverObject, PUNICODE_STRING registryPath) {
	int i;
	UNICODE_STRING  deviceNameString;
	UNICODE_STRING	deviceLinkString;
	NTSTATUS nts;
	
	DbgPrint("DriverEntry Called\r\n");
	
	
	/*Set up name and symbolic link*/
	RtlInitUnicodeString(&deviceNameString,deviceNameBuffer);
	RtlInitUnicodeString(&deviceLinkString,deviceLinkBuffer);

	/*Create a device*/
	nts = IoCreateDevice(driverObject,
						0,							//driver extension
						&deviceNameString,
						FILE_DEVICE_UNKNOWN	,		//device type
						FILE_DEVICE_SECURE_OPEN,	//device characteristics
						FALSE,						//exclusive device?
						&g_ioctlTestDevice);
	
	if(NT_SUCCESS(nts)) {
		nts = IoCreateSymbolicLink(&deviceLinkString, &deviceNameString);
	}
	
	/*Define the unload function*/
	driverObject->DriverUnload = OnUnload;
	
	/*Define major functions for the driver */
	for(i=0; i<IRP_MJ_MAXIMUM_FUNCTION;i++) {
		/*Defines a default function to all of driver's major functions*/
		driverObject->MajorFunction[i] = OnStubDispatch;
	}
	
	/*Define IOCTL major function*/
	driverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = deviceControl;
	
	return STATUS_SUCCESS;
}

/*This function defines what to based on the type of IOCTL received */
NTSTATUS deviceControl(PDEVICE_OBJECT deviceObject,PIRP irp){
	NTSTATUS nts = STATUS_NOT_SUPPORTED;
	PIO_STACK_LOCATION irpStackPointer = NULL;
	UINT dataWritten = 0;
	
	DbgPrint("deviceControl Called\r\n");
	
	irpStackPointer = IoGetCurrentIrpStackLocation(irp);
	
	if(irpStackPointer) {
		/*Normally this switch has cases for other IOCTL types*/
		switch(irpStackPointer->Parameters.DeviceIoControl.IoControlCode) {
			case IOCTL_TEST_DIRECT_OUT_IO:
				nts = handleIOCTLDirectOutIo(irp,irpStackPointer,&dataWritten);
				break;
			default:
				break;
		}
	}
	irp->IoStatus.Status = nts;
	irp->IoStatus.Information = dataWritten;

	IoCompleteRequest(irp,IO_NO_INCREMENT);
	
	return nts;
}
/*This function triggers when an IOCTL_TEST_DIRECT_OUT_IO is recieved */
NTSTATUS handleIOCTLDirectOutIo(PIRP irp, PIO_STACK_LOCATION irpStackPointer, UINT *pdwDataWritten){
	NTSTATUS nts = STATUS_UNSUCCESSFUL;
	PCHAR pInputBuffer;
	PCHAR pOutputBuffer;
	UINT dwDataRead = 0, dwDataWritten = 0;
	PCHAR pReturnData = "IOCTL - Direct Out I/O From Kernel!";
	UINT dwDataSize = sizeof("IOCTL - Direct Out I/O From Kernel!");
	DbgPrint("handleIOCTLDirectOutIo Called \r\n");

	/*Get kernel mode copy of user space input buffer*/
	pInputBuffer = irp->AssociatedIrp.SystemBuffer;
	
	pOutputBuffer = NULL;

	if(irp->MdlAddress) {
		/*Get kernel mode pointer to user space output buffer*/
		pOutputBuffer = MmGetSystemAddressForMdlSafe(irp->MdlAddress,NormalPagePriority);
	}

	if(pInputBuffer && pOutputBuffer) {

		if(isStringTerminated(pInputBuffer,irpStackPointer->Parameters.DeviceIoControl.InputBufferLength,&dwDataRead)) {
			/*Print out message sent from user mode program*/
			DbgPrint("UserModeMessage = '%s'", pInputBuffer);

			/*Send message back*/
			DbgPrint("%i >= %i",irpStackPointer->Parameters.DeviceIoControl.OutputBufferLength,dwDataSize); //print out size of output buffer vs dataSize

			/*Make sure ouput buffer is big enough*/
			if(irpStackPointer->Parameters.DeviceIoControl.OutputBufferLength >= dwDataSize){
				/*Copy in the message*/
				RtlCopyMemory(pOutputBuffer, pReturnData, dwDataSize);
				*pdwDataWritten = dwDataSize;
				nts = STATUS_SUCCESS;
			}else {
				*pdwDataWritten = dwDataSize;
				nts = STATUS_BUFFER_TOO_SMALL;
			}
		}
	}

	return nts;
}
/*This function makes sure a string is NULL terminated*/
BOOLEAN isStringTerminated(PCHAR s, UINT len, UINT *stringLen) {
	BOOLEAN isTerminated = FALSE;
	UINT index = 0;
	
	DbgPrint("isStringTerminated(0x%0x, %d)\r\n",s,len);
	
	while(index < len && isTerminated == FALSE) {
		if(s[index] == '\0'){
			*stringLen = index +1; //total count read
			isTerminated = TRUE;
			DbgPrint("  String is terminated!\r\n");
		}else {
			index++;
		}
	}
	return isTerminated;
}
NTSTATUS OnStubDispatch(PDEVICE_OBJECT driverObject, PIRP irp) {
	DbgPrint("stub called\n");
	irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(irp,IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
VOID OnUnload(PDRIVER_OBJECT driverObject ) {
	UNICODE_STRING deviceLinkName;
	
	DbgPrint("OnUnload Called\n");
	RtlInitUnicodeString(&deviceLinkName, L"\\DosDevices\\IOCTLTestDriver");
	IoDeleteSymbolicLink(&deviceLinkName);
	IoDeleteDevice(driverObject->DeviceObject);
}