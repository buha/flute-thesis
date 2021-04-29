CC = gcc
CFLAGS = -Wall -I/usr/include/libxml2/
LDFLAGS = -lm -lgsl -lgslcblas -lxml2 
OBJDIR=build

SRC = Complex.c \
	Woodwind.c \
	Map.c \
	Vector.c \
	TransferMatrix.c \
	Acoustics.c 

SRC_IMPEDANCE = $(SRC) \
	ParseXML.c \
	Impedance.c

SRC_PLAYEDIMPEDANCE = $(SRC) \
	ParseXML.c \
	PlayedImpedance.c

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.d: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MM -MT $@ -MF $<

Impedance: $(patsubst %.c,$(OBJDIR)/%.o,$(SRC_IMPEDANCE))
	$(CC) $(LDFLAGS) $^ -o $@

PlayedImpedance: $(patsubst %.c,$(OBJDIR)/%.o,$(SRC_PLAYEDIMPEDANCE))
	$(CC) $(LDFLAGS) $^ -o $@

all: Impedance PlayedImpedance

.PHONY: clean
clean:
	rm -r $(OBJDIR)
