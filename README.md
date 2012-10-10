Windows XP Drivers
==============================

These are a series of simple Windows XP drivers that culminates with a driver that hides a process from the TaskManager. Each is largely described by the name but here's a list anyway:

* helloworld 
    prints 'helloworld' to the kernel log

* simpleloader
    a userspace program that loads a driver

* hookfunction 
    hooks a system function (ZwQuerySystemInformation)

* hidefunction 
    builds upon the hookfunction driver, hides a process from the TaskManager by modifying the results of ZwQuerySystemInformation

* ioctltest 
    demonstrates use of IOCTLs to communicate between user mode and kernel mode
