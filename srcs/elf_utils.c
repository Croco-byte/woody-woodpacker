/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qroland <qroland@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 12:22:13 by user42            #+#    #+#             */
/*   Updated: 2021/10/22 12:20:46 by qroland          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

Elf64_Shdr	*elf_find_section(void *data, char *sec_name)
{
	Elf64_Ehdr	*header = (Elf64_Ehdr *)data;
	Elf64_Shdr	*sections = (Elf64_Shdr *)(data + header->e_shoff);
	Elf64_Shdr	*sec_strtab_h = &sections[header->e_shstrndx];
	const char	*sec_strtab_p = data + sec_strtab_h->sh_offset;

	for (int i = 0; i < header->e_shnum; i++)
	{
		if (ft_strncmp(sec_strtab_p + (sections[i].sh_name), sec_name, ft_strlen(sec_name)) == 0)
			return (&sections[i]);
	}
	return (0);
}

Elf64_Phdr	*elf_find_text_segment(t_elf *elf)
{
	Elf64_Phdr	*segments = (Elf64_Phdr *)(elf->map + elf->header->e_phoff);

	for (int i = 0; i < elf->header->e_phnum; i++)
	{
		if (segments[i].p_type == PT_LOAD && segments[i].p_flags & PF_X)
		{
			printf("[*] Found the text segment at offset:			0x%08lx\n", segments[i].p_offset);
			return (&segments[i]);
		}
	}
	fprintf(stderr, RED "[!] Couldn't find text segment in parent file.\n" RES);
	exit(1);
	return (0);
}

int		elf_mem_subst(void *m, int size, long pat, long val)
{
	unsigned char *p = (unsigned char *)m;

	for (int i = 0; i < size; i++)
	{
		if (*(long *)(p + i) == pat)
		{
			*(long *)(p + i) = val;
			return (0);
		}
	}
	return (-1);
}

int		danger_section(char *sec_name)
{
	char	*sections[] = {".interp", ".init", ".fini", ".got", ".eh_frame"};

	for (size_t i = 0; i < sizeof(sections)/sizeof(sections[0]); i++)
	{
		if (!strncmp(sec_name, sections[i], strlen(sections[i])))
			return (1);
	}
	return (0);
}

int		can_encrypt(char *sec_name)
{
	char	*sections[] = {".text", ".plt", ".rodata", ".data"};

	for (size_t i = 0; i < sizeof(sections)/sizeof(sections[0]); i++)
	{
		if (!strncmp(sec_name, sections[i], strlen(sections[i])))
			return (1);
	}
	return (0);
}
