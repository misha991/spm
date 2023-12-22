INC=-I ./include
spm:main.o
	g++ main.o -lstdc++ -I /usr/local/ssl/include -L /usr/local/ssl/lib -lssl -lcurl -lcrypto -Wall -o spm
main.o:main.cpp
	g++ -c main.cpp httplib.h indicators.hpp -lstdc++ -I /usr/local/ssl/include -L /usr/local/ssl/lib -lssl -lcurl -lcrypto -Wall 
clean:
	rm main.o