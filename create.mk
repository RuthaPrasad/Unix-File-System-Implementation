a.out: create.o btree.o hashtable.o
	gcc create.o btree.o hashtable.o
create.o: create.c hashtable.h
	gcc -c create.c hashtable.h
hashtable.o: hashtable.c hashtable.h
	gcc -c hashtable.c hashtable.h
btree.o: btree.c hashtable.h
	gcc -c btree.c hashtable.h
