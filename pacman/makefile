OBJDIR = obj
SRCDIR = src
LIBGRDIR = lib

SOURCES = $(shell find $(SRCDIR) -name *.c)
OBJ = $(subst $(SRCDIR),$(OBJDIR),$(SOURCES:.c=.o))
OUTPUT = resultat
LIB = -lm -lSDL -lSDL_ttf
OPT = -Wall
SDL_CFLAGS = $(shell sdl-config --cflags)
SDL_LDFLAGS = $(shell sdl-config --libs)
VERBOSE = TRUE

all: $(OUTPUT)

clean: 
	$(RM) $(OUTPUT) $(OBJDIR)/*.o $(LIBGRDIR)/*.o 

$(OUTPUT): $(LIBGRDIR)/libgraphique.o $(OBJ)
	gcc $^  -o $(OUTPUT)  $(SDL_CFLAGS) $(SDL_LDFLAGS) $(LIB)

$(LIBGRDIR)/libgraphique.o: $(LIBGRDIR)/libgraphique.c
	gcc   -c $(LIBGRDIR)/libgraphique.c -o $(LIBGRDIR)/libgraphique.o 

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/%.h
	gcc $(OPT) -c $<  -o $@






