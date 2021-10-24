# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: user42 <user42@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/10/05 11:40:11 by user42            #+#    #+#              #
#    Updated: 2021/10/24 13:17:38 by user42           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = woody-woodpacker

CC = clang

CFLAGS = -Wall -Werror -Wextra -fsanitize=address -g3 -fno-omit-frame-pointer -I includes/
# -fsanitize=address -g3 -fno-omit-frame-pointer 

HEADER = woody-woodpacker.h
MAIN = inject patch encrypt init parse c_utils file_utils elf_utils segment gap main

SRCS =	$(addsuffix .c, $(addprefix srcs/, $(MAIN)))

OBJS = ${SRCS:.c=.o}

all:		$(NAME)

$(NAME):	$(OBJS) build_payloads
			$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o:%.c
				${CC} ${CFLAGS} -c $< -o $@

build_payloads:
			nasm -f elf64 payloads/pdc_basic.asm -o payloads/pdc_basic.o && ld -o payloads/pdc_basic payloads/pdc_basic.o
			nasm -f elf64 payloads/pdc_full.asm -o payloads/pdc_full.o && ld -o payloads/pdc_full payloads/pdc_full.o
			nasm -f elf64 payloads/pic_basic.asm -o payloads/pic_basic.o && ld -o payloads/pic_basic payloads/pic_basic.o

clean:
				rm -f $(OBJS)
				rm -f payloads/*.o

fclean:
				rm -f $(OBJS)
				rm -f payloads/*.o
				rm -f $(NAME)
				rm -f woody

re:				fclean all

.PHONY:			all clean fclean re
