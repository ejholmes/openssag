all:
	g++ -c -o openssag.o openssag.cpp
	g++ -c -o loader.o loader.cpp
	ar rcs libopenssag.a openssag.o loader.o
	g++ -o ssag ssag.cpp -L. -lopenssag -lusb

clean:
	rm -rf ssag *.o *.a
