load --os ../Stage13/os_startup.xsm
load --library ../expl/library.lib
load --init ../Stage13/Assgn1/init.xsm
load --idle ../Stage13/Assgn1/idle.xsm
rm even.xsm
load --exec ../Stage13/Assgn1/even.xsm
load --int=10 ../Stage6/haltprog.xsm
load --int=7 ../Stage10/int7.xsm
load --module 7 ../Stage13/Assgn1/module_7.xsm
load --int=timer ../Stage13/Assgn1/timer.xsm