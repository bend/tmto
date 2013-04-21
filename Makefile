CXXFLAGS+=-W -Wall -O3
LDFLAGS+= -lpthread -lcrypto
GENERATOR=generator
TESTER=tester
SOURCES_GEN=generator.cpp utils.cpp
OBJECTS_GEN=$(SOURCES_GEN:.cpp=.o)
SOURCES_TES=tester.cpp utils.cpp
OBJECTS_TES=$(SOURCES_TES:.cpp=.o)
CC=g++

all: $(GENERATOR) $(TESTER)

debug: CXXFLAGS+= -DDEBUG -g
debug: $(GENERATOR) $(TESTER)

$(GENERATOR): $(OBJECTS_GEN)
	$(CC) -o $@ $(OBJECTS_GEN) $(LDFLAGS)

$(TESTER): $(OBJECTS_TES)
	$(CC) -o $@ $(OBJECTS_TES) $(LDFLAGS)


%.o: %.cpp
	$(CC) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean mrproper

clean:
	@rm -f $(OBJECTS_GEN) $(OBJECTS_TES)

mrproper:
	@rm -f $(GENERATOR) $(TESTER)
