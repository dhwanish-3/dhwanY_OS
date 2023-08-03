XFS Interface
=============

**Introduction** : XFS (eXperimental File System) Interface is an external interface to access the file system of the eXpOS. The filesystem is simulated on a binary file called ”disk.xfs”. The interface can format the disk, load/remove files, list files and copy blocks to a UNIX file.

Prerequisites :
-------------
	• GCC (GNU project C and C++ compiler)
	• libreadline-dev (readline library for command completion)

Compiling and Running :
---------------------
Run the following commands to compile and run the XFS interface:
1. `make`
2. `./xfs-interface`

Commands :
--------
Type `help` in the interface to display the list of commands.