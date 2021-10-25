/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody-woodpacker.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qroland <qroland@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 11:36:10 by user42            #+#    #+#             */
/*   Updated: 2021/10/25 13:46:50 by qroland          ###   ########.fr       */
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

# define RES 		"\x1B[0m"
# define RED  		"\x1B[31m"
# define GRN 		"\x1B[32m"
# define YEL  		"\x1B[33m"
# define BLU 		"\x1B[34m"
# define MAG 		"\x1B[35m"
# define CYN 		"\x1B[36m"
# define WHT 	 	"\x1B[37m"

# define PAGE_SIZE		4096

# define PDC_BASIC		0
# define PDC_FULL		1
# define PIC_BASIC		2

typedef struct		s_elf
{
	char			*fname;
	void			*map;
	uint64_t		fsize;
	Elf64_Ehdr		*header;
	Elf64_Phdr		*text_segment;
	Elf64_Off		injection_point;
	Elf64_Xword		gap_size;

	Elf64_Addr		enc_start_before;
	Elf64_Xword		enc_size_before;
	Elf64_Addr		enc_start_after;
	Elf64_Xword		enc_size_after;
}					t_elf;

typedef struct		s_payload
{
	char			*fname;
	void			*map;
	uint64_t		fsize;
	Elf64_Shdr		*txt_sec;
}					t_payload;

/* INIT FUNCTIONS */
void			init_elf(t_elf *elf, char *fname);
void			init_payload(t_payload *payload, char *fname);


/* PARSE FUNCTIONS */
void			parse_elf(t_elf *elf);
void			parse_payload(t_payload *payload);


/* ENCRYPTION FUNCTIONS */
void			encrypt_sec(t_elf *elf, Elf64_Shdr *sec);
void			encrypt_full(t_elf *elf);
void			encrypt_basic(t_elf *elf);


/* PALOAD PATCH FUNCTIONS */
void			patch_payload_basic(t_elf *elf, t_payload *payload);
void			patch_payload_full(t_elf *elf, t_payload *payload);
void			patch_payload_pic(t_elf *elf, t_payload *payload);


/* INJECTION FUNCTIONS */
void			inject_in_gap(t_elf *elf, t_payload *payload);
void			extend_and_inject(t_elf *elf, t_payload *payload);

/* SEGMENT FUNCTIONS (manipulate elf segment, extend it) */
void			extend_segment(t_elf *elf, size_t payload_size, int pg_nb);


/* GAP FUNCTIONS (find the existing padding space after text segment) */
void			elf_find_gap(t_elf *elf);


/* ELF UTILITIES (find section, modify elf content) */
Elf64_Shdr		*elf_find_section(void *data, char *sec_name);
int				elf_mem_subst(void *m, int size, long pat, long val);
Elf64_Phdr		*elf_find_text_segment(t_elf *elf);
int				can_encrypt(char *sec_name);



/* FILE UTILITIES (read-write-open-map) */
int				get_file_size(int fd, char *fname);
void			write_to_output(void *data, size_t size);
void			add_to_output(void *data, size_t size);
void			*elf_map_file(char *fname, uint64_t *size);


/* C UTILITIES */
size_t			ft_strlen(char *s);
int				ft_strncmp(const char *s1, const char *s2, size_t n);



#endif
