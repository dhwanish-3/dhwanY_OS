load --os $HOME/myexpos/Stage13/os_startup.xsm
load --init $HOME/myexpos/Stage20/Assgn2/init.xsm
load --idle $HOME/myexpos/Stage14/idle.xsm
load --int=10 $HOME/myexpos/Stage20/exit.xsm
load --int=9 $HOME/myexpos/Stage19/int9.xsm
load --int=8 $HOME/myexpos/Stage20/fork.xsm
load --exhandler $HOME/myexpos/Stage19/exhandler.xsm
load --int=7 $HOME/myexpos/Stage15/int7.xsm
load --int=6 $HOME/myexpos/Stage16/int6.xsm
load --int=disk $HOME/myexpos/Stage18/int2.xsm
load --int=console $HOME/myexpos/Stage16/console_int.xsm
load --module 5 $HOME/myexpos/Stage20/scheduler.xsm
load --module 7 $HOME/myexpos/Stage20/mod7.xsm
load --module 0 $HOME/myexpos/Stage18/module_0.xsm
load --module 1 $HOME/myexpos/Stage20/mod1.xsm
load --module 2 $HOME/myexpos/Stage19/module_2.xsm
load --module 4 $HOME/myexpos/Stage18/module_4.xsm
load --int=timer $HOME/myexpos/Stage14/timer.xsm
load --library $HOME/myexpos/expl/library.lib
rm exec.xsm
load --exec $HOME/myexpos/Stage19/exec.xsm
rm list.xsm
load --exec $HOME/myexpos/Stage19/Assgn1/list.xsm
rm odd.xsm
load --exec $HOME/myexpos/Stage20/Assgn1/odd.xsm
rm even.xsm
load --exec $HOME/myexpos/Stage20/Assgn1/even.xsm