#include "ntddk.h"
#include "hookingSample.h"

/*Prototypes*/
VOID OnUnload(PDRIVER_OBJECT);

/* Constants to name the driver */
const WCHAR deviceNameBuffer[] = L"\\Device\\HookingSample"; 
const WCHAR deviceLinkBuffer[] = L"\\DosDevices\\HookingSample";

/*Global pointer to device*/
PDEVICE_OBJECT g_hookingTestDevice; 

/*Flag for status of the SSDT*/
int g_ssdt_status;

/*Pointer to the first mapped memory page*/
unsigned long *g_MappedSSDT;

/*Memory location of the SSDT*/
MDL *g_mdlSSDT;

/*Import KeServiceDescriptorTable*/
__declspec(dllimport) SSDT KeServiceDescriptorTable;

/*Original ZwQuerySystemInformation*/
ZwQuerySystemInformationPrototype origZwQuerySystemInformation;

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
	&g_hookingTestDevice);
	
	if(NT_SUCCESS(nts)) {
		nts = IoCreateSymbolicLink(&deviceLinkString, &deviceNameString);
	}
	
	/*Define the unload function*/
	driverObject->DriverUnload = OnUnload;
	
	/*Modify the SSDT and print result*/
	DbgPrint("ssdt_modify() ran with result: %d\r\n",ssdt_modify());
	
	/*Replace ZwQuerySystemInformation with new function*/
	DbgPrint("replacing original ZwQuerySystemInformation with hookZwQuerySystemInformation\r\n");
	(unsigned long) origZwQuerySystemInformation = ssdt_writeService((unsigned long) ZwQuerySystemInformation,(long)hookZwQuerySystemInformation);
	
	
	
	return STATUS_SUCCESS;
}
/*This function changes the permissions on the SSDT function so it is writable*/
int ssdt_modify() {
	/*If the SSDT is already writable*/
	if(g_ssdt_status) {
		return g_ssdt_status;
	}
	/*Get the memory location of the SSDT */
	g_mdlSSDT = MmCreateMdl(0,KeServiceDescriptorTable.ServiceTableBase,KeServiceDescriptorTable.NumberOfServices * 4);
	
	if(!g_mdlSSDT) {
		g_ssdt_status = -1;
		return -1;
	}
	
	/*Indicate the memory location described by g_mdlSSDT is going to be changed*/
	MmBuildMdlForNonPagedPool(g_mdlSSDT);
	
	/*Change mdl flags to allow write*/
	g_mdlSSDT->MdlFlags |= MDL_MAPPED_TO_SYSTEM_VA;
	
	/*Returns a pointer to the first mapped memory page*/
	g_MappedSSDT = MmMapLockedPages(g_mdlSSDT,KernelMode);
	
	g_ssdt_status = 1;
	return g_ssdt_status;
}
/*This function restores the SSDT to its original state*/
int ssdt_restore() {
	if(g_ssdt_status) {
		MmUnmapLockedPages(g_MappedSSDT, g_mdlSSDT);
		IoFreeMdl(g_mdlSSDT);
	}else {
		return -1;
	}
	return 1;
}
/*This function replaces a function described in the SSDT with a custom defined one*/
unsigned long ssdt_writeService(unsigned long sysServiceFunction, unsigned long hookFunctionAddr) {
	unsigned long ret;
	
	if(!g_ssdt_status) {
		return 0;
	}
	ret = (unsigned long) InterlockedExchange((PULONG) &g_MappedSSDT[SYSCALL_INDEX(sysServiceFunction)], (long) hookFunctionAddr);
	
	return ret;
}
NTSTATUS hookZwQuerySystemInformation( ULONG systemInformationClass, PVOID systemInformation, ULONG systemInformationLength, PULONG returnLength ) {
	DbgPrint("ZwQuerySystemInformation hook called. Redirecting to orignal method.\r\n"); 
    return origZwQuerySystemInformation(systemInformationClass, systemInformation, systemInformationLength, returnLength); 
}
VOID OnUnload(PDRIVER_OBJECT driverObject ) {
	UNICODE_STRING deviceLinkName;
	
	/*Restore original ZwQuerySystemInformation*/
	ssdt_writeService((unsigned long)ZwQuerySystemInformation, (long) origZwQuerySystemInformation);
	origZwQuerySystemInformation = 0;
	
	/*Restore SSDT*/
	ssdt_restore();
	
	DbgPrint("OnUnload Called\n");
	RtlInitUnicodeString(&deviceLinkName, L"\\DosDevices\\IOCTLTestDriver");
	IoDeleteSymbolicLink(&deviceLinkName);
	IoDeleteDevice(driverObject->DeviceObject);
}