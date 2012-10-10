#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include "ioctlTest.h"


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
				"IOCTLTest",
				"IOCTLTestDriver",
				SERVICE_START | DELETE | SERVICE_STOP,
				SERVICE_KERNEL_DRIVER,
				SERVICE_DEMAND_START,
				SERVICE_ERROR_IGNORE,
				"C:\\ImportedDrivers\\IOCTLTestDriver.sys",//location of driver on VM	
				NULL,NULL,NULL,NULL,NULL);

            if(!handleService){
                    handleService = OpenService(handleSCManager,
                    "IOCTLTest",
                    SERVICE_START | DELETE | SERVICE_STOP);
            }
            if(handleService) {
                   printf("Starting Service\n");
                   StartService(handleService,0,NULL);
			
                   
				   /*Open file handle to device*/
				   HANDLE hf = CreateFile("\\\\.\\IOCTLTestDriver", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
				   
				   //send message to kernel
				   char in[256] = "hello fromo user land";
				   char out[256] = {0};
				   ZeroMemory(out,sizeof(out));
				   DWORD dwReturn;
				   DeviceIoControl(hf,
								   IOCTL_TEST_DIRECT_OUT_IO,
								   in,sizeof(in),
								   out,sizeof(out),
								   &dwReturn,
								   NULL);
					printf(out);
					printf("\n");
					CloseHandle(hf);
									 
	 
                   ControlService(handleService,SERVICE_CONTROL_STOP,&ss);
                   CloseServiceHandle(handleService);
		     DeleteService(handleService);				   
            }
			CloseServiceHandle(handleSCManager);
			getchar(); //pause to see messages
			
    }
    return 0;
}