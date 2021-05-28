
catslashdevslashrandom: catslashdevslashrandom.cpp
	$(CXX) $(CFLAGS) -pthread $@.cpp $(LDFLAGS) -o $@

clean:
	-rm *.o $(objects) catslashdevslashrandom

run: ./catslashdevslashrandom
	./catslashdevslashrandom $(ARGS)