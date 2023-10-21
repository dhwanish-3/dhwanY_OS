load --os $HOME/myexpos/Stage13/os_startup.xsm
load --init $HOME/myexpos/Stage21/Assgn1/shell2.xsm
load --idle $HOME/myexpos/Stage14/idle.xsm
load --int=10 $HOME/myexpos/Stage20/exit.xsm
load --int=11 $HOME/myexpos/Stage21/int11.xsm
load --int=13 $HOME/myexpos/Stage22/int13.xsm
load --int=14 $HOME/myexpos/Stage22/int14.xsm
load --int=15 $HOME/myexpos/Stage23/int15.xsm
load --int=9 $HOME/myexpos/Stage19/int9.xsm
load --int=8 $HOME/myexpos/Stage22/fork.xsm
load --exhandler $HOME/myexpos/Stage21/exhandler.xsm
load --int=7 $HOME/myexpos/Stage15/int7.xsm
load --int=6 $HOME/myexpos/Stage16/int6.xsm
load --int=4 $HOME/myexpos/Stage23/int4.xsm
load --int=disk $HOME/myexpos/Stage18/int2.xsm
load --int=console $HOME/myexpos/Stage16/console_int.xsm
load --module 5 $HOME/myexpos/Stage20/scheduler.xsm
load --module 7 $HOME/myexpos/Stage23/mod7.xsm
load --module 0 $HOME/myexpos/Stage23/mod0.xsm
load --module 1 $HOME/myexpos/Stage22/mod1.xsm
load --module 2 $HOME/myexpos/Stage19/module_2.xsm
load --module 4 $HOME/myexpos/Stage23/mod4.xsm
load --int=timer $HOME/myexpos/Stage14/timer.xsm
rm create.xsm
load --exec $HOME/myexpos/Stage23/Assgn1/create.xsm
rm delete.xsm
load --exec $HOME/myexpos/Stage23/Assgn2/delete.xsm