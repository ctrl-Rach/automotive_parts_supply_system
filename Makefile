# Compiler
CC = gcc

# Executables
MAIN = mymain
ORDER = myorder
MONITOR = mymonitor

# Source files
SRC_MAIN = main.c
SRC_ORDER = order.c
SRC_MONITOR = monitor.c


all: $(MAIN) $(ORDER) $(MONITOR)

# Build main
$(MAIN): $(SRC_MAIN)
	$(CC) $(SRC_MAIN) -o $(MAIN) 

# Build order
$(ORDER): $(SRC_ORDER)
	$(CC) $(SRC_ORDER) -o $(ORDER) 

# Build monitor
$(MONITOR): $(SRC_MONITOR)
	$(CC) $(SRC_MONITOR) -o $(MONITOR) 

# Run system
run: all
	./$(MAIN)

# Clean files
clean:
	rm -f $(MAIN) $(ORDER) $(MONITOR) main_log.txt
