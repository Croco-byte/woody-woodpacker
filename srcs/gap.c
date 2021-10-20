/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gap.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 12:15:55 by user42            #+#    #+#             */
/*   Updated: 2021/10/20 11:48:35 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

void	elf_find_gap(t_elf *elf)
{
	Elf64_Phdr	*segments = (Elf64_Phdr *)(elf->map + elf->header->e_phoff);
	int			text_seg_end;
	int			gap = elf->fsize;

	for (int i = 0; i < elf->header->e_phnum; i++)
	{
		if (segments[i].p_type == PT_LOAD && segments[i].p_flags & PF_X)
		{
			elf->text_segment = &(segments[i]);
			text_seg_end = (elf->text_segment->p_offset + elf->text_segment->p_filesz);
			printf("[*] Found the text segment at offset:			0x%08lx\n", elf->text_segment->p_offset);
		}
		else if (elf->text_segment && segments[i].p_type == PT_LOAD && segments[i].p_offset - text_seg_end < (size_t)gap)
		{
			gap = segments[i].p_offset - text_seg_end;
			printf("[*] Following segment at offset:			0x%08lx\n", segments[i].p_offset);
		}
	}
	if (!elf->text_segment || gap == elf->fsize)
	{
		fprintf(stderr, RED "[!] Couldn't find text segment or data segment in parent file.\n" RES);
		exit(1);
	}
	elf->gap_offset = text_seg_end;
	elf->gap_size = gap;
	printf(GRN "[+] Text segment gap at:				0x%08lx (%lu bytes available)\n" RES, elf->gap_offset, elf->gap_size);
}
