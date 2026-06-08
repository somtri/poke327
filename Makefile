CC = gcc
CXX = g++
ECHO = echo
RM = rm -f

CPPFLAGS =
CFLAGS = -std=gnu11 -Wall -O2 -g
CXXFLAGS = -std=gnu++17 -Wall -O2 -g

LDFLAGS = -lncurses

BIN = poke327
OBJS = poke327.o heap.o io.o character.o db_parse.o pokemon.o

all: $(BIN)

$(BIN): $(OBJS)
	@$(ECHO) Linking $@
	@$(CXX) $^ -o $@ $(LDFLAGS)

-include $(OBJS:.o=.d)

%.o: %.c
	@$(ECHO) Compiling $<
	@$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MF $*.d -c $<

%.o: %.cpp
	@$(ECHO) Compiling $<
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MF $*.d -c $<

.PHONY: all run clean clobber smoke-test etags

run: $(BIN)
	@./$(BIN)

smoke-test: $(BIN)
	@./$(BIN) --smoke-test

clean:
	@$(ECHO) Removing all generated files
	@$(RM) *.o $(BIN) *.d TAGS core vgcore.* gmon.out

clobber: clean
	@$(ECHO) Removing backup files
	@$(RM) *~ \#* *pgm

etags:
	@$(ECHO) Updating TAGS
	@etags *.c *.cpp *.h
