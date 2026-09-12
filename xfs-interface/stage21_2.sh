load --os ../Stage13/os_startup.xsm
load --init ../Stage21/Assgn1/shell2.xsm
load --idle ../Stage14/idle.xsm
load --int=10 ../Stage20/exit.xsm
load --int=11 ../Stage21/int11.xsm
load --int=15 ../Stage21/int15.xsm
load --int=9 ../Stage19/int9.xsm
load --int=8 ../Stage20/fork.xsm
load --exhandler ../Stage21/exhandler.xsm
load --int=7 ../Stage15/int7.xsm
load --int=6 ../Stage16/int6.xsm
load --int=disk ../Stage18/int2.xsm
load --int=console ../Stage16/console_int.xsm
load --module 5 ../Stage20/scheduler.xsm
load --module 7 ../Stage21/mod7.xsm
load --module 0 ../Stage18/module_0.xsm
load --module 1 ../Stage21/mod1.xsm
load --module 2 ../Stage19/module_2.xsm
load --module 4 ../Stage18/module_4.xsm
load --int=timer ../Stage14/timer.xsm
load --library ../expl/library.lib
rm pid.xsm
load --exec ../Stage21/Assgn2/pid.xsm
rm fork3.xsm
load --exec ../Stage21/Assgn2/fork3.xsm