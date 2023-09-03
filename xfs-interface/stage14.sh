load --os $HOME/myexpos/Stage13/os_startup.xsm
load --init $HOME/myexpos/Stage14/init.xsm
load --idle $HOME/myexpos/Stage14/idle.xsm
rm even.xsm
load --exec $HOME/myexpos/Stage14/even.xsm
load --int=10 $HOME/myexpos/Stage14/int10.xsm
load --int=7 $HOME/myexpos/Stage10/int7.xsm
load --module 5 $HOME/myexpos/Stage14/module_5.xsm
load --module 7 $HOME/myexpos/Stage14/module_7.xsm
load --int=timer $HOME/myexpos/Stage14/timer.xsm