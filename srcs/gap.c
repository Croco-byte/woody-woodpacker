/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gap.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 12:15:55 by user42            #+#    #+#             */
/*   Updated: 2021/10/18 14:56:47 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

Elf64_Phdr	*elf_find_gap(void *d, int fsize, size_t *gap_offset, size_t *gap_size)
{
	Elf64_Ehdr	*header = (Elf64_Ehdr *)d;
	Elf64_Phdr	*segments = (Elf64_Phdr *)(d + header->e_phoff);
	Elf64_Phdr	*text_segment = 0;
	int			text_seg_end;
	int			gap = fsize;

	for (int i = 0; i < header->e_phnum; i++)
	{
		if (segments[i].p_type == PT_LOAD && segments[i].p_flags & PF_X)
		{
			text_segment = &(segments[i]);
			text_seg_end = (text_segment->p_offset + text_segment->p_filesz);
			printf("[*] Found the text segment at offset:			0x%08lx\n", text_segment->p_offset);
		}
		else if (text_segment && segments[i].p_type == PT_LOAD && segments[i].p_offset - text_seg_end < (size_t)gap)
		{
			gap = segments[i].p_offset - text_seg_end;
			printf("[*] Following segment at offset:			0x%08lx\n", segments[i].p_offset);
		}
	}
	if (!text_segment || gap == fsize)
	{
		fprintf(stderr, RED "[!] Couldn't find text segment or data segment in parent file.\n" RES);
		exit(1);
	}
	*gap_offset = text_seg_end;
	*gap_size = gap;
	printf(GRN "[+] Text segment gap at:				0x%08lx (%lu bytes available)\n" RES, *gap_offset, *gap_size);
	return (text_segment);
}
