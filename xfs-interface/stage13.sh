load --os ../Stage13/os_startup.xsm
load --init ../Stage12/init.xsm
load --idle ../Stage12/idle.xsm
load --int=10 ../Stage6/haltprog.xsm
load --int=7 ../Stage10/int7.xsm
load --module 7 ../Stage13/module_7.xsm
load --int=timer ../Stage12/timer.xsm