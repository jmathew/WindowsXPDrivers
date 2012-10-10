#include "ntddk.h"
VOID OnUnload( PDRIVER_OBJECT driverObject ) {
	DbgPrint("OnUnload Called\n");
}
NTSTATUS DriverEntry( PDRIVER_OBJECT driverObject, PUNICODE_STRING registryPath) {
	DbgPrint("Hello World\n");
	driverObject->DriverUnload = OnUnload;
	return STATUS_SUCCESS;
}