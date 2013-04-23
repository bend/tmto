CXXFLAGS+=-W -Wall -O3
LDFLAGS+= -lpthread -lcrypto
EXEC_GEN=rainbow_gen
EXEC_CRK=rainbow_crack
SRCGEN=rainbow_gen.cpp utils.cpp
OBJGEN=$(SRCGEN:.cpp=.o)
SRCCRK=rainbow_crack.cpp utils.cpp
OBJCRK=$(SRCCRK:.cpp=.o)
CC=g++

all: $(EXEC_GEN) $(EXEC_CRK)

debug: CXXFLAGS+= -DDEBUG -g
debug: $(EXEC_GEN) $(EXEC_CRK)

$(EXEC_GEN): $(OBJGEN)
	$(CC) -o $@ $(OBJGEN) $(LDFLAGS)

$(EXEC_CRK): $(OBJCRK)
	$(CC) -o $@ $(OBJCRK) $(LDFLAGS)


%.o: %.cpp
	$(CC) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean mrproper

clean:
	@rm -f $(OBJGEN) $(OBJCRK)

mrproper:
	@rm -f $(EXEC_CRK) $(EXEC_GEN)
