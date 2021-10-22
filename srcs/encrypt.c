/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   encrypt.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qroland <qroland@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/22 12:01:15 by qroland           #+#    #+#             */
/*   Updated: 2021/10/22 13:19:11 by qroland          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

void	encrypt_sec(t_elf *elf, Elf64_Shdr *sec)
{
	size_t j = 0;
	if (!sec)
		return ;
	char *ptr = (char *)(elf->map + sec->sh_offset);
	while (j < sec->sh_size)
	{
		*ptr ^= 0xa5;
		j++;
		ptr++;
	}
}

void    encrypt_basic(t_elf *elf)
{
    printf("[*] Encrypt mode is : BASIC\n[*] Encrypting section .text\n");
    Elf64_Shdr  *txt_sec    = elf_find_section(elf->map, ".text");
    elf->enc_start_before   = (long)(txt_sec->sh_addr);
    elf->enc_size_before    = txt_sec->sh_size;
    encrypt_sec(elf, txt_sec);
}

void	encrypt_light(t_elf *elf)
{
    printf("[*] Encrypt mode is : FULL\n");
	Elf64_Shdr *sec_headers = (Elf64_Shdr *)(elf->map + elf->header->e_shoff);
	Elf64_Shdr *sec_str_h = &(sec_headers[elf->header->e_shstrndx]);
	char * sec_str_p = elf->map + sec_str_h->sh_offset;
	size_t j = 0;

	for (size_t i = 0; i < elf->header->e_shnum; i++)
	{
		if (!strcmp(sec_str_p + sec_headers[i].sh_name, ".text")
		|| !strcmp(sec_str_p + sec_headers[i].sh_name, ".data") || !strcmp(sec_str_p + sec_headers[i].sh_name, ".bss"))
		{
			printf("[*] Encrypting section %s\n", sec_str_p + sec_headers[i].sh_name);
			if (sec_headers[i].sh_offset > (elf->text_segment->p_offset + elf->text_segment->p_filesz))
			{
					if (elf->enc_size_after == 0)
				elf->enc_start_after = (long)(sec_headers[i].sh_addr);
				j = 0;
				char		*ptr = (char *)(elf->map + sec_headers[i].sh_offset);
				while (j < sec_headers[i].sh_size)
				{
					*ptr ^= 0xa5;
					j++;
					ptr++;
					elf->enc_size_after++;
				}
			}
			else
			{
				if (elf->enc_size_before == 0)
					elf->enc_start_before = (long)(sec_headers[i].sh_addr);
				j = 0;
				char		*ptr = (char *)(elf->map + sec_headers[i].sh_offset);
				while (j < sec_headers[i].sh_size)
				{
					*ptr ^= 0xa5;
					j++;
					ptr++;
					elf->enc_size_before++;
				}
			}
		}
	}
}