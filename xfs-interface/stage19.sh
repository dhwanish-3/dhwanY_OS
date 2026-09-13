load --os ../Stage13/os_startup.xsm
load --init ../Stage18/init.xsm
load --idle ../Stage14/idle.xsm
load --int=10 ../Stage14/int10.xsm
load --int=9 ../Stage19/int9.xsm
load --exhandler ../Stage19/exhandler.xsm
load --int=7 ../Stage15/int7.xsm
load --int=6 ../Stage16/int6.xsm
load --int=disk ../Stage18/int2.xsm
load --int=console ../Stage16/console_int.xsm
load --module 5 ../Stage14/module_5.xsm
load --module 7 ../Stage19/module_7.xsm
load --module 0 ../Stage18/module_0.xsm
load --module 1 ../Stage19/module_1.xsm
load --module 2 ../Stage19/module_2.xsm
load --module 4 ../Stage18/module_4.xsm
load --int=timer ../Stage14/timer.xsm
load --library ../expl/library.lib
rm exec.xsm
load --exec ../Stage19/exec.xsm
rm list.xsm
load --exec ../Stage19/Assgn1/list.xsm