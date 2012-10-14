Windows XP Drivers
==============================

These are a series of simple Windows XP drivers that culminates with a driver that hides a process from the TaskManager. Each is largely described by the name but here's a list anyway:

* **helloworld**
  * prints 'helloworld' to the kernel log


* **simpleloader**
  * a userspace program that loads a driver


* **hookfunction**
  * hooks a system function (ZwQuerySystemInformation)


* **hidefunction**
  * builds upon the hookfunction driver, hides a process from the TaskManager by modifying the results of ZwQuerySystemInformation


* **ioctltest**
  * demonstrates use of IOCTLs to communicate between user mode and kernel mode


### **Setup Environment**

 * Windows Driver Kit, the tool used to build the driver can be accessed by:
  * Start - Windows Driver Kits - **WDK install version** - Build Environments - Windows XP

 * WinDbg
  * Create a shortcut with the following target:
```
C:\WinDDK\7600.16385.1\Debuggers\windbg.exe -b -k com:pipe,port=\\.\pipe\com_1,resets=0
```

 * Windows XP with debugging enabled 
  * Modify *boot.ini*:
```
[boot loader]  return
timeout=30  return
default=multi(0)disk(0)rdisk(0)partition(1)\WINDOWS  return
[operating systems]  return
multi(0)disk(0)rdisk(0)partition(1)\WINDOWS="Microsoft Windows XP Professional" /noexecute=optin /fastdetect  return
multi(0)disk(0)rdisk(0)partition(1)\WINDOWS="Microsoft Windows XP Professional - DEBUG" /noexecute=optin /fastdetect /debug /debugport=com1 /baudrate=115200  return
```


### **Build**

 * Startup the (x86) Checked Build Environment
 * Navigate to the source directory
 * Type *bcz* (for colored output) or *build* (for uncolored output).


The driver file will be in the **source folder**\objchk_wxp_x86\i386\ folder and it will be called TheDriverName.sys

The user space programs can be build using: gcc -o “programname.exe” programname.c
