# Files' names
MAIN = main
LIBDYN = arrayDynamic
LIBSTAT = arrayStatic
OUTPUTNAME = main

# Project Paths
SRCPATH = ../src/
OBJPATH = ../obj/
SHAREDPATH = ../lib/shared/
STATICPATH = ../lib/static/
BINPATH = ../bin/

#FLAGS of compilator
FLAGS = O0

# Make output program simply
simple: $(BINPATH)$(OUTPUTNAME).out

$(BINPATH)$(OUTPUTNAME).out: $(OBJPATH)$(MAIN).o $(OBJPATH)$(LIBDYN).o $(OBJPATH)$(LIBSTAT).o
	gcc -$(FLAGS) $(OBJPATH)$(MAIN).o $(OBJPATH)$(LIBDYN).o $(OBJPATH)$(LIBSTAT).o -o $(BINPATH)$(OUTPUTNAME).out

$(OBJPATH)$(MAIN).o: $(SRCPATH)$(MAIN).c $(SRCPATH)$(LIBDYN).h $(SRCPATH)$(LIBSTAT).h
	gcc -$(FLAGS) -c $(SRCPATH)$(MAIN).c -o $(OBJPATH)$(MAIN).o

$(OBJPATH)$(LIBDYN).o: $(SRCPATH)$(LIBDYN).c $(SRCPATH)$(LIBDYN).h
	gcc -$(FLAGS) -c $(SRCPATH)$(LIBDYN).c -o $(OBJPATH)$(LIBDYN).o

$(OBJPATH)$(LIBSTAT).o: $(SRCPATH)$(LIBSTAT).c $(SRCPATH)$(LIBSTAT).h
	gcc -$(FLAGS) -c $(SRCPATH)$(LIBSTAT).c -o $(OBJPATH)$(LIBSTAT).o


# Make static libraries
staticLibs: $(OBJPATH)$(LIBSTAT).o $(OBJPATH)$(LIBDYN).o
	ar rcs $(STATICPATH)lib$(LIBSTAT).a $(OBJPATH)$(LIBSTAT).o
	ar rcs $(STATICPATH)lib$(LIBDYN).a $(OBJPATH)$(LIBDYN).o
	
# Make output program with static libraries
withStatic: $(OBJPATH)$(MAIN).o  staticLibs
	gcc -$(FLAGS) $(OBJPATH)$(MAIN).o -static -L$(STATICPATH) -l$(LIBSTAT) -l$(LIBDYN) -o $(BINPATH)$(OUTPUTNAME)Static.out

# Make shared libraries
sharedLibs: $(SRCPATH)$(LIBDYN).c $(SRCPATH)$(LIBDYN).h
	gcc -$(FLAGS) -fPIC -shared $(SRCPATH)$(LIBSTAT).c -o $(SHAREDPATH)lib$(LIBSTAT).so
	gcc -$(FLAGS) -fPIC -shared $(SRCPATH)$(LIBDYN).c -o $(SHAREDPATH)lib$(LIBDYN).so

# Make output program with shared libraries
withShared: $(OBJPATH)$(MAIN).o  sharedLibs
	gcc -$(FLAGS) $(OBJPATH)$(MAIN).o -L$(SHAREDPATH) -Wl,-rpath=$(SHAREDPATH) -l$(LIBSTAT) -l$(LIBDYN) -o $(BINPATH)$(OUTPUTNAME)Shared.out

run: withStatic withShared
	echo "\n"
	echo "tests on shared"
	echo "\n"
	time ./$(BINPATH)$(OUTPUTNAME)Shared.out -c 100000 20 -m static
	time ./$(BINPATH)$(OUTPUTNAME)Shared.out -c 100000 20 -m dynamic
	time ./$(BINPATH)$(OUTPUTNAME)Shared.out -c 100000 20 -m static -s 2000
	time ./$(BINPATH)$(OUTPUTNAME)Shared.out -c 100000 20 -m dynamic -r 10000 -a 10000
	time ./$(BINPATH)$(OUTPUTNAME)Shared.out -c 100000 20 -m dynamic -R 10000

	echo "\n"
	echo "tests on static"
	echo "\n"
	time ./$(BINPATH)$(OUTPUTNAME)Static.out -c 100000 20 -m static
	time ./$(BINPATH)$(OUTPUTNAME)Static.out -c 100000 20 -m dynamic
	time ./$(BINPATH)$(OUTPUTNAME)Static.out -c 100000 20 -m static -s 2000
	time ./$(BINPATH)$(OUTPUTNAME)Static.out -c 100000 20 -m dynamic -r 10000 -a 10000
	time ./$(BINPATH)$(OUTPUTNAME)Static.out -c 100000 20 -m dynamic -R 10000

runOptTests: withStatic withShared
	time ./../bin/mainShared.out -m static -c 100000 20 -s 2000 -r 1000 -a 1000 -R 1000
	time ./../bin/mainShared.out -m dynamic -c 100000 20 -s 2000 -r 1000 -a 1000 -R 1000
	time ./../bin/mainStatic.out -m static -c 100000 20 -s 2000 -r 1000 -a 1000 -R 1000
	time ./../bin/mainStatic.out -m dynamic -c 100000 20 -s 2000 -r 1000 -a 1000 -R 1000

# Clean
.PHONY: clean
clean:
	-rm -f $(OBJPATH)*.o
	-rm -f $(STATICPATH)*.a
	-rm -f $(SHAREDPATH)*.so
	-rm -f $(BINPATH)*
	