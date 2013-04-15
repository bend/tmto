CXXFLAGS+=-W -Wall
LDFLAGS+= -lpthread -lcrypto
GENERATOR=tmto
TESTER=tester
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
CC=g++

all: $(GENERATOR) $(TESTER)

debug: CXXFLAGS+= -DDEBUG -g
debug: $(GENERATOR) $(TESTER)

$(GENERATOR): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

$(TESTER): $(GENERATOR)


%.o: %.cpp
	$(CC) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean mrproper

clean:
	@rm -f $(OBJECTS)

mrproper:
	@rm -f $(GENERATOR) $(TESTER)
