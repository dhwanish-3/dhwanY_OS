load --os $HOME/myexpos/Stage13/os_startup.xsm
load --library $HOME/myexpos/expl/library.lib
load --init $HOME/myexpos/Stage13/Assgn1/init.xsm
load --idle $HOME/myexpos/Stage13/Assgn1/idle.xsm
rm even.xsm
load --exec $HOME/myexpos/Stage13/Assgn1/even.xsm
load --int=10 $HOME/myexpos/Stage6/haltprog.xsm
load --int=7 $HOME/myexpos/Stage10/int7.xsm
load --module 7 $HOME/myexpos/Stage13/Assgn1/module_7.xsm
load --int=timer $HOME/myexpos/Stage13/Assgn1/timer.xsm