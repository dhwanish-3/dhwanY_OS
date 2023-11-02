load --os $HOME/myexpos/Stage13/os_startup.xsm
load --init $HOME/myexpos/Stage21/Assgn1/shell2.xsm
load --idle $HOME/myexpos/Stage14/idle.xsm
load --int=10 $HOME/myexpos/Stage20/exit.xsm
load --int=11 $HOME/myexpos/Stage21/int11.xsm
load --int=13 $HOME/myexpos/Stage22/int13.xsm
load --int=14 $HOME/myexpos/Stage22/int14.xsm
load --int=15 $HOME/myexpos/Stage25/int15.xsm
load --int=9 $HOME/myexpos/Stage19/int9.xsm
load --int=8 $HOME/myexpos/Stage24/fork.xsm
load --exhandler $HOME/myexpos/Stage21/exhandler.xsm
load --int=7 $HOME/myexpos/Stage25/int7.xsm
load --int=6 $HOME/myexpos/Stage24/int6.xsm
load --int=4 $HOME/myexpos/Stage23/int4.xsm
load --int=5 $HOME/myexpos/Stage25/int5.xsm
load --int=disk $HOME/myexpos/Stage18/int2.xsm
load --int=console $HOME/myexpos/Stage16/console_int.xsm
load --module 5 $HOME/myexpos/Stage20/scheduler.xsm
load --module 7 $HOME/myexpos/Stage24/mod7.xsm
load --module 0 $HOME/myexpos/Stage24/mod0.xsm
load --module 1 $HOME/myexpos/Stage25/mod1.xsm
load --module 2 $HOME/myexpos/Stage25/mod2.xsm
load --module 4 $HOME/myexpos/Stage23/mod4.xsm
load --module 3 $HOME/myexpos/Stage25/file_manager.xsm
load --int=timer $HOME/myexpos/Stage14/timer.xsm
rm ls.xsm
load --exec $HOME/myexpos/Stage25/expl/ls.xsm
rm cat.xsm
load --exec $HOME/myexpos/Stage25/expl/cat.xsm
rm rm.xsm
load --exec $HOME/myexpos/Stage25/expl/rm.xsm
rm cp.xsm
load --exec $HOME/myexpos/Stage25/expl/cp.xsm
rm assgn1.xsm
load --exec $HOME/myexpos/Stage25/Assgn/assgn1.xsm