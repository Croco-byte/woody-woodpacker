/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   segment.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 12:31:36 by user42            #+#    #+#             */
/*   Updated: 2021/10/21 23:17:54 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"



void	extend_segment(t_elf *elf, size_t payload_size, int pg_nb)
{
	Elf64_Phdr	*segments = (Elf64_Phdr *)(elf->map + elf->header->e_phoff);
	Elf64_Shdr	*sections = (Elf64_Shdr *)(elf->map + elf->header->e_shoff);
	Elf64_Addr	evaddr;
	int			i = 0;

	while (&segments[i] != elf->text_segment)
		i++;
	printf("[*] Extending .text segment\n");
	evaddr = segments[i].p_vaddr + segments[i].p_filesz;
	segments[i].p_filesz += payload_size;
	segments[i].p_memsz += payload_size;
	
	printf("[*] Patching subsequent segment headers\n");
	i++;
	for (; i < elf->header->e_phnum; i++)
		segments[i].p_offset += pg_nb * PAGE_SIZE;

	printf("[*] Patching subsequent section headers\n");
	for (int i = 0; i < elf->header->e_shnum; i++)
	{
		if (sections[i].sh_offset > elf->injection_point)
			sections[i].sh_offset += pg_nb * PAGE_SIZE;
		else if (sections[i].sh_addr + sections[i].sh_size == evaddr)
			sections[i].sh_size += payload_size;
	}

	printf("[*] Patching section offset in ELF header\n");
	if (elf->header->e_shoff > elf->injection_point)
		elf->header->e_shoff += pg_nb * PAGE_SIZE;

	printf(GRN "[+] Successfully extended .text segment\n" RES);
}
