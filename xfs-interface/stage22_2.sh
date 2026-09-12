load --os ../Stage13/os_startup.xsm
load --init ../Stage21/Assgn1/shell2.xsm
load --idle ../Stage14/idle.xsm
load --int=10 ../Stage20/exit.xsm
load --int=11 ../Stage21/int11.xsm
load --int=13 ../Stage22/int13.xsm
load --int=14 ../Stage22/int14.xsm
load --int=15 ../Stage22/Assgn2/int15.xsm
load --int=9 ../Stage19/int9.xsm
load --int=8 ../Stage22/fork.xsm
load --exhandler ../Stage21/exhandler.xsm
load --int=7 ../Stage15/int7.xsm
load --int=6 ../Stage16/int6.xsm
load --int=disk ../Stage18/int2.xsm
load --int=console ../Stage16/console_int.xsm
load --module 5 ../Stage22/Assgn2/mod5.xsm
load --module 7 ../Stage22/Assgn2/mod7.xsm
load --module 0 ../Stage22/Assgn2/mod0.xsm
load --module 1 ../Stage22/mod1.xsm
load --module 2 ../Stage19/module_2.xsm
load --module 4 ../Stage18/module_4.xsm
load --int=timer ../Stage22/Assgn2/timer.xsm
rm test4.xsm
load --exec ../Stage22/Assgn1/test4.xsm
rm parent.xsm
load --exec ../Stage22/Assgn2/parent.xsm
rm child.xsm
load --exec ../Stage22/Assgn2/child.xsm
rm con.xsm
load --exec ../Stage22/Assgn3/con.xsm
rm seq.xsm
load --exec ../Stage22/Assgn3/seq.xsm