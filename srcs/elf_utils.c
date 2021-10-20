/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 12:22:13 by user42            #+#    #+#             */
/*   Updated: 2021/10/20 12:26:06 by user42           ###   ########.fr       */
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

int		elf_mem_subst(void *m, int size, long pat, long val)
{
	unsigned char *p = (unsigned char *)m;

	for (int i = 0; i < size; i++)
	{
		if (*(long *)(p + i) == pat)
		{
			printf("[*] Patching magic pattern in payload:			0x%016lx -> 0x%016lx\n", pat, val);
			*(long *)(p + i) = val;
			return (0);
		}
	}
	return (-1);
}
