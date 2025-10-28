# Compilador y opciones
CC = gcc
CFLAGS = -Wall -Wextra

# Archivos fuente y objeto
SRCS = p1.c list.c filefunctions.c firstshellfunctions.c
OBJS = $(SRCS:.c=.o)

# Nombre del ejecutable
TARGET = p1

# Regla principal
all: $(TARGET)

# Enlazado
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compilación individual
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpieza
clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(TARGET) a.out

rebuild: fclean all

.PHONY: all clean fclean rebuild

