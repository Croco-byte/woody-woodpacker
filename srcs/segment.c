/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   segment.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 12:31:36 by user42            #+#    #+#             */
/*   Updated: 2021/10/18 15:21:09 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"



Elf64_Off	extend_segment(void *d, size_t payload_size, int pg_nb)
{
	Elf64_Ehdr	*header = (Elf64_Ehdr *)d;
	Elf64_Phdr	*segments = (Elf64_Phdr *)(d + header->e_phoff);
	Elf64_Shdr	*sections = (Elf64_Shdr *)(d + header->e_shoff);
	Elf64_Off	offset = 0;
	Elf64_Addr	evaddr;

	printf("[*] Extending .text segment\n");
	printf("[*] Patching subsequent segment headers\n");
	for (int i = 0; i < header->e_phnum ; i++)
	{
		if (offset)
			segments[i].p_offset += pg_nb * PAGE_SIZE;
		else if (segments[i].p_type == PT_LOAD && segments[i].p_offset == 0)
		{
			evaddr = segments[i].p_vaddr + segments[i].p_filesz;
			offset = segments[i].p_offset + segments[i].p_filesz;
			segments[i].p_filesz += payload_size;
			segments[i].p_memsz += payload_size;
		}
	}

	printf("[*] Patching subsequent section headers\n");
	for (int i = 0; i < header->e_shnum; i++)
	{
		if (sections[i].sh_offset > offset)
			sections[i].sh_offset += pg_nb * PAGE_SIZE;
		else if (sections[i].sh_addr + sections[i].sh_size == evaddr)
			sections[i].sh_size += payload_size;
	}

	printf("[*] Patching section offset in ELF header\n");
	if (header->e_shoff > offset)
		header->e_shoff += pg_nb * PAGE_SIZE;

	printf(GRN "[+] Successfully extended .text segment\n" RES);
	return (offset);
}
