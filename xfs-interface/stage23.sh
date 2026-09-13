load --os ../Stage13/os_startup.xsm
load --init ../Stage21/Assgn1/shell2.xsm
load --idle ../Stage14/idle.xsm
load --int=10 ../Stage20/exit.xsm
load --int=11 ../Stage21/int11.xsm
load --int=13 ../Stage22/int13.xsm
load --int=14 ../Stage22/int14.xsm
load --int=15 ../Stage23/int15.xsm
load --int=9 ../Stage19/int9.xsm
load --int=8 ../Stage22/fork.xsm
load --exhandler ../Stage21/exhandler.xsm
load --int=7 ../Stage15/int7.xsm
load --int=6 ../Stage16/int6.xsm
load --int=4 ../Stage23/int4.xsm
load --int=disk ../Stage18/int2.xsm
load --int=console ../Stage16/console_int.xsm
load --module 5 ../Stage20/scheduler.xsm
load --module 7 ../Stage23/mod7.xsm
load --module 0 ../Stage23/mod0.xsm
load --module 1 ../Stage22/mod1.xsm
load --module 2 ../Stage19/module_2.xsm
load --module 4 ../Stage23/mod4.xsm
load --int=timer ../Stage14/timer.xsm
rm create.xsm
load --exec ../Stage23/Assgn1/create.xsm
rm delete.xsm
load --exec ../Stage23/Assgn2/delete.xsm