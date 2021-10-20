/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody-woodpacker.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 11:36:10 by user42            #+#    #+#             */
/*   Updated: 2021/10/20 11:29:11 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WOODY_WOODPACKER_H
# define WOODY_WOODPACKER_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/mman.h>
# include <fcntl.h>
# include <string.h>	// Pour memmove ; à supprimer
# include "elf.h"

# define RES  "\x1B[0m"
# define RED  "\x1B[31m"
# define GRN  "\x1B[32m"
# define YEL  "\x1B[33m"
# define BLU  "\x1B[34m"
# define MAG  "\x1B[35m"
# define CYN  "\x1B[36m"
# define WHT  "\x1B[37m"

# define PAGE_SIZE		4096

typedef struct		s_elf
{
	char			*fname;
	void			*map;
	int				fsize;
	Elf64_Ehdr		*header;
	Elf64_Phdr		*text_segment;
	Elf64_Off		gap_offset;
	Elf64_Xword		gap_size;
}					t_elf;

typedef struct		s_payload
{
	char			*fname;
	void			*map;
	int				fsize;
	Elf64_Shdr		*text_section;
}					t_payload;


/* SEGMENT FUNCTIONS (manipulate elf segment, extend it) */
Elf64_Off		extend_segment(void *d, size_t payload_size, int pg_nb);

/* GAP FUNCTIONS (find the existing padding space after text segment) */
Elf64_Phdr		*elf_find_gap(void *d, int fsize, size_t *gap_offset, size_t *gap_size);


/* ELF UTILITIES (find section, modify elf content) */
Elf64_Shdr		*elf_find_section(void *data, char *sec_name);
int				elf_mem_subst(void *m, int size, long pat, long val);


/* FILE UTILITIES (read-write-open-map) */
int				get_file_size(int fd, char *fname);
void			write_to_output(char *name, void *data, size_t size);
void			add_to_output(char *name, void *data, size_t size);
void			*elf_map_file(char *fname, int *size);


/* C UTILITIES */
size_t			ft_strlen(char *s);
int				ft_strncmp(const char *s1, const char *s2, size_t n);



#endif
