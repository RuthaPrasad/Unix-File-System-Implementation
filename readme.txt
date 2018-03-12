The following steps are to be followed to execute the files and test the system.
Each "test-run" is considered to check the File System(FS) by:
mounting it, running linux commands, unmounting it,repeat.


1)to start afresh, remove all executable files created by last test-run (only once ,when starting a new test-run).
Run 
	$make -f clean.mk
	
2) to create FS : (only once for each test-run)
Run
	$ make -f create.mk
	$ ./a.out

3) to create the shell that mounts the FS: (only once for each test-run)
	$ make -f makefile.mk

4)To start the shell and mount the FS onto the shell
	$ ./a.out
5)To unmount the FS from the shell, type "unmount" in the shell. To mount it back, run ./a.out
	../$ unmount
	   $ ./a.out

NOTE : steps (4) and (5) are the repeatable steps in each test-run.
	once the test-run is over, go back to step(1). 
	


