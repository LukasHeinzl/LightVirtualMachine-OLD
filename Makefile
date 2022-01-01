#not very beautiful but it works
OBJ = obj/ArgParser.o obj/Bytecodes.o obj/ByteReader.o obj/ClassLoader.o obj/Comp.o obj/Error.o obj/lni.o obj/lvm.o obj/main.o obj/Runtime.o obj/Startup.o obj/Types.o

#change to your proper compiler
CC = g++

#modify to fit your needs (these are recommended)
CFLAGS = -Wall -Wextra -O3 -fexceptions -std=c++11 -I"${CURDIR}/include/"

all: $(OBJ)
	$(CC) -o lvm $(OBJ)

windows: $(OBJ)
	$(CC) -o lvm $(OBJ)

other: $(OBJ)
	$(CC) -o lvm $(OBJ) -ldl

obj/ArgParser.o: src/ArgParser.cpp
	$(CC) $(CFLAGS) -o obj/ArgParser.o -c src/ArgParser.cpp

obj/Bytecodes.o: src/Bytecodes.cpp
	$(CC) $(CFLAGS) -o obj/Bytecodes.o -c src/Bytecodes.cpp

obj/ByteReader.o: src/ByteReader.cpp
	$(CC) $(CFLAGS) -o obj/ByteReader.o -c src/ByteReader.cpp

obj/ClassLoader.o: src/ClassLoader.cpp
	$(CC) $(CFLAGS) -o obj/ClassLoader.o -c src/ClassLoader.cpp

obj/Comp.o: src/Comp.cpp
	$(CC) $(CFLAGS) -o obj/Comp.o -c src/Comp.cpp

obj/Error.o: src/Error.cpp
	$(CC) $(CFLAGS) -o obj/Error.o -c src/Error.cpp

obj/lni.o: src/lni.cpp
	$(CC) $(CFLAGS) -o obj/lni.o -c src/lni.cpp

obj/lvm.o: src/lvm.cpp
	$(CC) $(CFLAGS) -o obj/lvm.o -c src/lvm.cpp

obj/main.o: src/main.cpp
	$(CC) $(CFLAGS) -o obj/main.o -c src/main.cpp

obj/Runtime.o: src/Runtime.cpp
	$(CC) $(CFLAGS) -o obj/Runtime.o -c src/Runtime.cpp

obj/Startup.o: src/Startup.cpp
	$(CC) $(CFLAGS) -o obj/Startup.o -c src/Startup.cpp

obj/Types.o: src/Types.cpp
	$(CC) $(CFLAGS) -o obj/Types.o -c src/Types.cpp