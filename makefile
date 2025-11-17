CC = C:\MinGW\bin\g++

FLAGS ?= -Wall -Wextra -g -pipe -fexceptions \
         -Wno-missing-field-initializers \
         -Wno-unused-parameter \
         -D _DEBUG -D _EJUDGE_CLIENT_SIDE -DTX_USE_SPEAK

# Библиотеки для TXLib speech
LIBS = -lole32 -loleaut32 -luuid

all: main.exe

main.exe: main.o tree_tests.o tree.o speech.o graphics.o
	$(CC) $(FLAGS) main.o tree_tests.o tree.o speech.o graphics.o -o main.exe $(LIBS)

main.o: main.cpp tree.h speech.h graphics.h
	$(CC) $(FLAGS) -c main.cpp

tree_tests.o: tree_tests.cpp tree.h speech.h
	$(CC) $(FLAGS) -c tree_tests.cpp

tree.o: tree.cpp tree.h tree_error_type.h graphics.h
	$(CC) $(FLAGS) -c tree.cpp

speech.o: speech.cpp speech.h
	$(CC) $(FLAGS) -c speech.cpp

graphics.o: graphics.cpp graphics.h
	$(CC) $(FLAGS) -c graphics.cpp

clean:
	rm -rf *.o *.exe

# Правило для перекомпиляции всего
rebuild: clean all
