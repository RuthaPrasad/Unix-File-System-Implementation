a.out: mount.o btree.o hashtable.o
	gcc mount.o btree.o hashtable.o
mount.o: mount.c hashtable.h
	gcc -c mount.c hashtable.h
hashtable.o: hashtable.c hashtable.h
	gcc -c hashtable.c hashtable.h
btree.o: btree.c hashtable.h
	gcc -c btree.c hashtable.h
clean:
	rm a.out 
