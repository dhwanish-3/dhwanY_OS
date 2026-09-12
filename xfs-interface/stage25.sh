load --os ../Stage13/os_startup.xsm
load --init ../Stage21/Assgn1/shell2.xsm
load --idle ../Stage14/idle.xsm
load --int=10 ../Stage20/exit.xsm
load --int=11 ../Stage21/int11.xsm
load --int=13 ../Stage22/int13.xsm
load --int=14 ../Stage22/int14.xsm
load --int=15 ../Stage25/int15.xsm
load --int=9 ../Stage19/int9.xsm
load --int=8 ../Stage24/fork.xsm
load --exhandler ../Stage21/exhandler.xsm
load --int=7 ../Stage25/int7.xsm
load --int=6 ../Stage24/int6.xsm
load --int=4 ../Stage23/int4.xsm
load --int=5 ../Stage25/int5.xsm
load --int=disk ../Stage18/int2.xsm
load --int=console ../Stage16/console_int.xsm
load --module 5 ../Stage20/scheduler.xsm
load --module 7 ../Stage24/mod7.xsm
load --module 0 ../Stage24/mod0.xsm
load --module 1 ../Stage25/mod1.xsm
load --module 2 ../Stage25/mod2.xsm
load --module 4 ../Stage23/mod4.xsm
load --module 3 ../Stage25/file_manager.xsm
load --int=timer ../Stage14/timer.xsm
rm ls.xsm
load --exec ../Stage25/expl/ls.xsm
rm cat.xsm
load --exec ../Stage25/expl/cat.xsm
rm rm.xsm
load --exec ../Stage25/expl/rm.xsm
rm cp.xsm
load --exec ../Stage25/expl/cp.xsm
rm assgn1.xsm
load --exec ../Stage25/Assgn/assgn1.xsm
rm assgn2.xsm
load --exec ../Stage25/Assgn/assgn2.xsm
rm test6.xsm
load --exec ../Stage25/Assgn/test6.xsm
rm test15.xsm
load --exec ../Stage25/Assgn/test15.xsm