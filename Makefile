# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: user42 <user42@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/10/05 11:40:11 by user42            #+#    #+#              #
#    Updated: 2021/10/19 15:03:12 by user42           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = woody-woodpacker

CC = clang

CFLAGS = -Wall -Werror -Wextra -I includes/
# -fsanitize=address -g3 -fno-omit-frame-pointer 

HEADER = woody-woodpacker.h

MAIN = c_utils file_utils elf_utils segment gap main

SRCS =	$(addsuffix .c, $(addprefix srcs/, $(MAIN)))

OBJS = ${SRCS:.c=.o}

all:		$(NAME)

$(NAME):	$(OBJS)
			$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o:%.c
				${CC} ${CFLAGS} -c $< -o $@

clean:
				rm -f $(OBJS)

fclean:
				rm -f $(OBJS)
				rm -f $(NAME)

re:				fclean all

.PHONY:			all clean fclean re
