SRC := $(wildcard *.c)

OBJ := ${SRC:.c=.o}

TARGETDIR := ../bin
TARGET := $(TARGETDIR)/espresso

all: prepare $(TARGET)

prepare:
	mkdir $(TARGETDIR)

$(TARGET): $(OBJ)
	$(LINK.c) $(OBJ) -o $(TARGET)

clean: 
	@- $(RM) -r $(TARGETDIR)
	@- $(RM) $(OBJ)

