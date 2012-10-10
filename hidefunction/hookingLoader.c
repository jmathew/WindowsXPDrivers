#include <windows.h>
#include <stdio.h>

int _cdecl main(void) {
	HANDLE handleSCManager;
	HANDLE handleService;  
	SERVICE_STATUS ss;

	handleSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE);

	printf("Loading Driver\n");

	if(handleSCManager) {
		printf("Creating Service\n");

		handleService = CreateService(
		handleSCManager,
		"HookingSampleService",
		"HookingSample",
		SERVICE_START | DELETE | SERVICE_STOP,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_IGNORE,
		"C:\\ImportedDrivers\\HookingSample.sys",//location of driver on VM	
		NULL,NULL,NULL,NULL,NULL);

		if(!handleService){
			handleService = OpenService(handleSCManager,
			"HookingSampleService",
			SERVICE_START | DELETE | SERVICE_STOP);
		}
		if(handleService) {
			printf("Starting Service\n");
			StartService(handleService,0,NULL);
			
			getchar();//pause to see hook messages in windbg
			
			ControlService(handleService,SERVICE_CONTROL_STOP,&ss);
			CloseServiceHandle(handleService);
			DeleteService(handleService);				   
		}
		CloseServiceHandle(handleSCManager);
		getchar(); //pause to see messages

	}
	return 0;
}