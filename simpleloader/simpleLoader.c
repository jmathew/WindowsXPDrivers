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
				"HelloWorld",
				"HelloWorldDriver",
				SERVICE_START | DELETE | SERVICE_STOP,
				SERVICE_KERNEL_DRIVER,
				SERVICE_DEMAND_START,
				SERVICE_ERROR_IGNORE,
				"C:\\ImportedDrivers\\helloworld.sys",	
				NULL,NULL,NULL,NULL,NULL);

            if(!handleService){
                    handleService = OpenService(handleSCManager,
                    "HelloWorld",
                    SERVICE_START | DELETE | SERVICE_STOP);
            }
            if(handleService) {
                   printf("Starting Service\n");
                   StartService(handleService,0,NULL);
				   getchar();
				  /*
				   printf("Press enter to talk\r\n");
				   getchar();
				   
				   //load driver
				   HANDLE hf;
				   DWORD dwReturn;
				   hf = CreateFile("\\\\.\\_TalkingDevice", GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
				   if(hf) {
						WriteFile(hf,"hell\n",sizeof("hell\n"),&dwReturn,NULL);
						CloseHandle(hf);
				   }
                   printf("Press enter to close service\r\n");
                   getchar();*/
				   
				   //end
                   ControlService(handleService,SERVICE_CONTROL_STOP,&ss);
                   CloseServiceHandle(handleService);
				   DeleteService(handleService);				   
            }
			CloseServiceHandle(handleSCManager);
			
    }
    return 0;
}