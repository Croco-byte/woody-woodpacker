/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   encrypt.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/22 12:01:15 by qroland           #+#    #+#             */
/*   Updated: 2021/10/26 12:20:19 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

uint64_t	rotate_key(uint64_t key)
{
	return ((key >> 8) | (key << 56));
}

void		encrypt_sec(t_elf *elf, Elf64_Shdr *sec)
{
	size_t		j		= 0;
	uint64_t	tmp_key	= elf->key;

	if (!sec)
		return ;
	char *ptr = (char *)(elf->map + sec->sh_offset);
	while (j < sec->sh_size)
	{
		*ptr ^= (char)tmp_key;
		tmp_key = rotate_key(tmp_key);
		j++;
		ptr++;
	}
}

void		encrypt_basic(t_elf *elf)
{
	printf("[*] Encrypt mode is : BASIC\n[*] Encrypting section .text\n");
	Elf64_Shdr  *txt_sec    = elf_find_section(elf->map, ".text");
	elf->enc_start_before   = (long)(txt_sec->sh_addr);
	elf->enc_size_before    = txt_sec->sh_size;
	encrypt_sec(elf, txt_sec);
}

void		encrypt_full(t_elf *elf)
{
	printf("[*] Encrypt mode is : FULL\n");
	Elf64_Shdr *sec_headers = (Elf64_Shdr *)(elf->map + elf->header->e_shoff);
	Elf64_Shdr *sec_str_h = &(sec_headers[elf->header->e_shstrndx]);
	char * sec_str_p = elf->map + sec_str_h->sh_offset;
	size_t j;

	for (size_t i = 0; i < elf->header->e_shnum; i++)
	{
		if (!ft_strcmp(sec_str_p + sec_headers[i].sh_name, ".text"))
		{
			printf("[*] Encrypting section %s\n", sec_str_p + sec_headers[i].sh_name);
			elf->enc_start_before = sec_headers[i].sh_addr;
			elf->enc_size_before += sec_headers[i].sh_size;
			encrypt_sec(elf, &sec_headers[i]);
			j = 1;
			while (can_encrypt(sec_str_p + sec_headers[i - j].sh_name))
			{
				printf("[*] Encrypting section %s\n", sec_str_p + sec_headers[i - j].sh_name);
				elf->enc_start_before = sec_headers[i - j].sh_addr;
				elf->enc_size_before += sec_headers[i - j].sh_size;
				encrypt_sec(elf, &sec_headers[i - j]);
				j++;
			}
			j = 1;
			while (can_encrypt(sec_str_p + sec_headers[i + j].sh_name))
			{
				printf("[*] Encrypting section %s\n", sec_str_p + sec_headers[i + j].sh_name);
				elf->enc_size_before += sec_headers[i + j].sh_size;
				encrypt_sec(elf, &sec_headers[i + j]);
				j++;
			}
		}

		else if (!ft_strcmp(sec_str_p + sec_headers[i].sh_name, ".data"))
		{
			printf("[*] Encrypting section %s\n", sec_str_p + sec_headers[i].sh_name);
			elf->enc_start_after = sec_headers[i].sh_addr;
			elf->enc_size_after += sec_headers[i].sh_size;
			encrypt_sec(elf, &sec_headers[i]);
			j = 1;
			while (can_encrypt(sec_str_p + sec_headers[i - j].sh_name))
			{
				printf("[*] Encrypting section %s\n", sec_str_p + sec_headers[i - j].sh_name);
				elf->enc_start_after = sec_headers[i - j].sh_addr;
				elf->enc_size_after += sec_headers[i - j].sh_size;
				encrypt_sec(elf, &sec_headers[i - j]);
				j++;
			}
			j = 1;
			while (can_encrypt(sec_str_p + sec_headers[i + j].sh_name))
			{
				printf("[*] Encrypting section %s\n", sec_str_p + sec_headers[i + j].sh_name);
				elf->enc_size_after += sec_headers[i + j].sh_size;
				encrypt_sec(elf, &sec_headers[i + j]);
				j++;
			}
		}
	}
}




/*

void	encrypt_full(t_elf *elf)
{
    printf("[*] Encrypt mode is : FULL\n");
	Elf64_Shdr *sec_headers = (Elf64_Shdr *)(elf->map + elf->header->e_shoff);
	Elf64_Shdr *sec_str_h = &(sec_headers[elf->header->e_shstrndx]);
	char * sec_str_p = elf->map + sec_str_h->sh_offset;
	size_t j = 0;

	for (size_t i = 0; i < elf->header->e_shnum; i++)
	{
		if (!ft_strcmp(sec_str_p + sec_headers[i].sh_name, ".text"))
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


*/
