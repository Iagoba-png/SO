# Compilador y opciones
CC = gcc
CFLAGS = -Wall -Werror

# Archivos fuente y objeto
SRCS = p2.c list.c filefunctions.c firstshellfunctions.c memfunctions.c processesfunctions.c
OBJS = $(SRCS:.c=.o)

# Nombre del ejecutable
TARGET = p2

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

