CC = g++
CFLAGS = -pthread

produtorCons: prodConsThreadMatMul.cpp
	$(CC) $(CFLAGS) -o produtorCons prodConsThreadMatMul.cpp

procedural: proceduralMatMul.cpp
	$(CC) -o procedural proceduralMatMul.cpp

threadBasic: threadBasicMatMul.cpp
	$(CC) $(CFLAGS) -o threadBasic threadBasicMatMul.cpp

threadLimited: threadLimitedMatMul.cpp
	$(CC) $(CFLAGS) -o threadLimited threadLimitedMatMul.cpp

mutex: threadMutexMatMul.cpp
	$(CC) $(CFLAGS) -o mutex threadMutexMatMul.cpp

clean:
	@if [ -f ./mutex ]; then rm mutex; fi
	@if [ -f ./threadLimited ]; then rm threadLimited; fi
	@if [ -f ./threadBasic ]; then rm threadBasic; fi
	@if [ -f ./procedural ]; then rm procedural; fi
	@if [ -f ./produtorCons ]; then rm produtorCons; fi
