load --os ../Stage13/os_startup.xsm
load --init ../Stage14/init.xsm
load --idle ../Stage14/idle.xsm
rm even.xsm
load --exec ../Stage14/even.xsm
load --int=10 ../Stage14/int10.xsm
load --int=7 ../Stage10/int7.xsm
load --module 5 ../Stage14/module_5.xsm
load --module 7 ../Stage14/module_7.xsm
load --int=timer ../Stage14/timer.xsm