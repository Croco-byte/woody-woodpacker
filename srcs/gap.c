/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gap.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 12:15:55 by user42            #+#    #+#             */
/*   Updated: 2021/10/21 23:05:30 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

void	elf_find_gap(t_elf *elf)
{
	Elf64_Phdr	*segments = (Elf64_Phdr *)(elf->map + elf->header->e_phoff);
	elf->gap_size = elf->fsize;
	int			i = 0;

	while (&segments[i] != elf->text_segment)
		i++;
	i++;
	for (; i < elf->header->e_phnum; i++)
	{
		if (segments[i].p_type == PT_LOAD && segments[i].p_offset - elf->injection_point < elf->gap_size)
		{
			elf->gap_size = segments[i].p_offset - elf->injection_point;
			printf("[*] Following segment at offset:			0x%08lx\n", segments[i].p_offset);
		}
	}
	if (elf->gap_size == elf->fsize)
	{
		fprintf(stderr, RED "[!] Couldn't find data segment in parent file.\n" RES);
		exit(1);
	}
	printf(GRN "[+] Text segment gap at:				0x%08lx (%lu bytes available)\n" RES, elf->injection_point, elf->gap_size);
}
