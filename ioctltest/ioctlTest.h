typedef unsigned int UINT;
typedef char* PCHAR;
#define IOCTL_TEST_DIRECT_OUT_IO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801,METHOD_OUT_DIRECT,FILE_READ_DATA | FILE_WRITE_DATA) 
