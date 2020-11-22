OBJDIR = obj
SRCDIR = src
LIBGRDIR = lib

SOURCES = $(shell find $(SRCDIR) -name *.c)
OBJ = $(subst $(SRCDIR),$(OBJDIR),$(SOURCES:.c=.o))
OUTPUT = resultat
LIB = -lm -lSDL -lSDL_ttf
OPT = -Wall -Werror
SDL_CFLAGS = $(shell sdl-config --cflags)
SDL_LDFLAGS = $(shell sdl-config --libs)
VERBOSE = TRUE

all: release

clean: 
	$(RM) $(OUTPUT) $(OBJDIR)/*.o $(LIBGRDIR)/*.o 

release: $(LIBGRDIR)/libgraphique.o $(OBJ)
	@echo "Compilation en mode release"
	gcc $^ -o $(OUTPUT)  $(SDL_CFLAGS) $(SDL_LDFLAGS) $(LIB)

debug: $(wildcard src/*.c) $(wildcard src/*.h) $(LIBGRDIR)/libgraphique.o
	@echo "Compilation en mode debug"
	gcc -g -DDEBUG $^ -o $(OUTPUT) $(SDL_CFLAGS) $(SDL_LDFLAGS) $(LIB)

$(LIBGRDIR)/libgraphique.o: $(LIBGRDIR)/libgraphique.c
	gcc -c $(LIBGRDIR)/libgraphique.c -o $(LIBGRDIR)/libgraphique.o 

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/%.h
	gcc  $(OPT) -c $<  -o $@

windows: $(wildcard src/*.c) $(wildcard src/*.h) 
	gcc -c -L ./lib -I ./include $(LIBGRDIR)/libgraphique.c -o $(LIBGRDIR)/libgraphique.o 
	gcc -L ./lib -I ./include $^ $(LIBGRDIR)/libgraphique.o -o w.exe -lmingw32 -lws2_32 $(LIB)
