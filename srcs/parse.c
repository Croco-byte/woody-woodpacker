/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/22 11:56:58 by qroland           #+#    #+#             */
/*   Updated: 2021/10/26 12:48:26 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

int			is_elf64(void *p)
{
	unsigned char	*magic;

	magic = (unsigned char *)p;
	if (ft_strncmp((char *)magic, ELFMAG, 4) != 0)
		return (0);
	if (magic[EI_CLASS] != ELFCLASS64)
		return (0);
	return (1);
}

uint64_t	rand_uint64(void)
{
	uint64_t r = 0;

	for (int i = 0; i < 5; i++)
	{
		r = (r << 15) | (rand() & 0x7FFF);
	}
	return r & 0xFFFFFFFFFFFFFFFFULL;
}

void	gen_key(t_elf *elf)
{
	elf->key = rand_uint64();
}

void	parse_elf(t_elf *elf)
{
	elf->map				= elf_map_file(elf->fname, &elf->fsize);
	if (!is_elf64(elf->map))
	{
		fprintf(stderr, RED "[!] File architecture not suported. x86_64 only\n" RES);
		exit(1);
	}
	elf->header				= (Elf64_Ehdr *)elf->map;
	gen_key(elf);
	elf->text_segment		= elf_find_text_segment(elf);
	elf->injection_point	= elf->text_segment->p_offset + elf->text_segment->p_filesz;
	elf_find_gap(elf);
}

void	parse_payload(t_payload *payload)
{
	payload->map			= elf_map_file(payload->fname, &payload->fsize);
	payload->txt_sec		= elf_find_section(payload->map, ".text");
}
