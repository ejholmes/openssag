all:
	# gcc -o ssag ssag.c -lusb
	g++ -c -o openssag.o openssag.cpp
	g++ -c -o cypress.o cypress.cpp
	ar rcs libopenssag.a openssag.o
	g++ -o ssag ssag.cpp -L. -lopenssag -lusb

clean:
	rm -rf ssag *.o *.a
