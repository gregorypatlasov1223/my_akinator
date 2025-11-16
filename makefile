CC = C:\MinGW\bin\g++

FLAGS ?= -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef \
-Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations \
-Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ \
-Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion \
-Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 \
-Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers \
-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo \
-Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits \
-Wwrite-strings -Werror=vla -D _DEBUG -D _EJUDGE_CLIENT_SIDE -DTX_USE_SPEAK

# Библиотеки для TXLib speech
LIBS = -lole32 -loleaut32 -luuid

all: main.exe

main.exe: main.o tree_tests.o tree.o speech.o
	$(CC) $(FLAGS) main.o tree_tests.o tree.o speech.o -o main.exe $(LIBS)

main.o: main.cpp tree.h speech.h
	$(CC) $(FLAGS) -c main.cpp

tree_tests.o: tree_tests.cpp tree.h speech.h
	$(CC) $(FLAGS) -c tree_tests.cpp

tree.o: tree.cpp tree.h tree_error_type.h
	$(CC) $(FLAGS) -c tree.cpp

speech.o: speech.cpp speech.h
	$(CC) $(FLAGS) -c speech.cpp

clean:
	rm -rf *.o *.exe

# Правило для перекомпиляции всего
rebuild: clean all
