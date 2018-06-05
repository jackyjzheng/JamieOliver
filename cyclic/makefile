CPPFLAGS=-pthread 
LDLIBS=-luuid

default: RamseyIncrementServer RamseySearchServer

RamseyIncrementServer: RamseyIncrementServer.cpp 
	g++ $(CPPFLAGS) -o RamseyIncrementServer RamseyIncrementServer.cpp $(LDLIBS)

RamseySearchServer: RamseySearchServer.cpp 
	g++ $(CPPFLAGS) -o RamseySearchServer RamseySearchServer.cpp $(LDLIBS)

clean:
	rm RamseyIncrementServer RamseySearchServer