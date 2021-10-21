/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 11:35:47 by user42            #+#    #+#             */
/*   Updated: 2021/10/21 23:33:23 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

void	write_in_gap(t_elf *elf)
{
	write_to_output(elf->fname, elf->map, elf->fsize);
	printf(GRN "[+] Injection complete\n" RES);
}

void	write_after_extension(t_elf *elf, t_payload *payload, int pages_to_add)
{
	int			bytes_to_add = pages_to_add * PAGE_SIZE;
	char		*zeros;

	zeros = malloc((bytes_to_add - payload->txt_sec->sh_size) * sizeof(char));
	bzero(zeros, bytes_to_add - payload->txt_sec->sh_size);
	printf("[*] Writing until injection point			(%lu bytes)\n", elf->injection_point);
	write_to_output(elf->fname, elf->map, elf->injection_point);
	printf("[*] Writing payload at injection point			(%lu bytes)\n",payload->txt_sec->sh_size);
	add_to_output(elf->fname, payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size);
	printf("[*] Writing additionnal padding				(%lu bytes)\n", bytes_to_add - payload->txt_sec->sh_size);
	add_to_output(elf->fname, zeros, bytes_to_add - payload->txt_sec->sh_size);
	printf("[*] Writing rest of parent file				(%lu bytes)\n", (elf->fsize) - (elf->injection_point));
	add_to_output(elf->fname, elf->map + elf->injection_point, (elf->fsize) - (elf->injection_point));
	printf(GRN "[+] Injection complete\n" RES);
	free(zeros);
}

void	inject_in_gap(t_elf *elf, t_payload *payload)
{
	printf(GRN "\n[+] Enough space to inject payload in gap. Injecting on the fly!\n" RES);
	memmove(elf->map + elf->injection_point, payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size);
	printf("[*] Injected payload to offset:				0x%08lu\n", elf->injection_point);
	printf("[*] Patching ELF entry point to payload\n");
	elf->header->e_entry = (Elf64_Addr)(elf->text_segment->p_vaddr + elf->injection_point);
	write_in_gap(elf);
}

void	extend_and_inject(t_elf *elf, t_payload *payload)
{
	int pages_to_add = ((payload->txt_sec->sh_size - elf->gap_size) / PAGE_SIZE) + 1;

	printf(GRN "\n[+] Not enough space in gap. Extending .text segment\n" RES);
	printf("[*] We need to add this many pages:			%u (missing %lu bytes)\n", pages_to_add, payload->txt_sec->sh_size - elf->gap_size);
	extend_segment(elf, payload->txt_sec->sh_size, pages_to_add);
	printf("[*] The payload can now be injected at offset:		0x%08lu\n", elf->injection_point);
	printf("[*] Patching ELF entry point to payload\n");
	elf->header->e_entry = elf->text_segment->p_vaddr + elf->injection_point;
	write_after_extension(elf, payload, pages_to_add);
}

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

void	encrypt_light(t_elf *elf)
{
	Elf64_Shdr *sec_headers = (Elf64_Shdr *)(elf->map + elf->header->e_shoff);
	Elf64_Shdr *sec_str_h = &(sec_headers[elf->header->e_shstrndx]);
	char * sec_str_p = elf->map + sec_str_h->sh_offset;
	size_t j = 0;

	for (size_t i = 0; i < elf->header->e_shnum; i++)
	{
		if (!strcmp(sec_str_p + sec_headers[i].sh_name, ".text") || !strcmp(sec_str_p + sec_headers[i].sh_name, ".plt")
		|| !strcmp(sec_str_p + sec_headers[i].sh_name, ".data") || !strcmp(sec_str_p + sec_headers[i].sh_name, ".bss"))
		{
			printf("[*] Encrypting section %s\n", sec_str_p + sec_headers[i].sh_name);
			if (sec_headers[i].sh_offset > (elf->text_segment->p_offset + elf->text_segment->p_filesz))
			{
				printf("[DEBUG] This section is AFTER the end of the first PT_LOAD segment (so after our injection point)\n");
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
				printf("[DEBUG] This section is BEFORE the end of the first PT_LOAD segment (so before our injection point)\n");
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

void	init_elf(t_elf *elf, char *fname)
{
	elf->fname				= fname;
	elf->map				= 0;
	elf->fsize				= 0;
	elf->header				= 0;
	elf->text_segment		= 0;
	elf->injection_point	= 0;
	elf->gap_size			= 0;
	elf->enc_start_before	= 0;
	elf->enc_size_before	= 0;
	elf->enc_start_after	= 0;
	elf->enc_size_after		= 0;
}

void	init_payload(t_payload *payload, char *fname)
{
	payload->fname			= fname;
	payload->map			= 0;
	payload->fsize			= 0;
	payload->txt_sec		= 0;
}

void	parse_elf(t_elf *elf)
{
	elf->map				= elf_map_file(elf->fname, &elf->fsize);
	elf->header				= (Elf64_Ehdr *)elf->map;
	elf->text_segment		= elf_find_text_segment(elf);
	elf->injection_point	= elf->text_segment->p_offset + elf->text_segment->p_filesz;
	elf_find_gap(elf);
}

void	parse_payload(t_payload *payload)
{
	payload->map			= elf_map_file(payload->fname, &payload->fsize);
	payload->txt_sec		= elf_find_section(payload->map, ".text");
}

void	patch_payload(t_elf *elf, t_payload *payload)
{
	Elf64_Addr	aligned_addr_before;
	Elf64_Addr	aligned_addr_after;

	aligned_addr_before = (elf->enc_start_before);
	aligned_addr_before = ((PAGE_SIZE-1)&aligned_addr_before) ? ((aligned_addr_before) & ~(PAGE_SIZE-1)) : aligned_addr_before;
	aligned_addr_after = (elf->enc_start_after);
	aligned_addr_after = ((PAGE_SIZE-1)&aligned_addr_after) ? ((aligned_addr_after) & ~(PAGE_SIZE-1)) : aligned_addr_after;

	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x1111111111111111, (long)elf->header->e_entry);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x2222222222222222, (long)0xa5);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x3333333333333333, elf->enc_start_before);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x4444444444444444, elf->enc_size_before);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x5555555555555555, aligned_addr_before);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x6666666666666666, elf->enc_start_after);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x7777777777777777, elf->enc_size_after);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x8888888888888888, aligned_addr_after);
}


int		main(int argc, char **argv)
{
	t_elf		elf;
	t_payload	payload;

	printf("[*] ... Launching woody ...\n");
	if (argc != 3)
	{
		fprintf(stderr, "[!] Usage: %s <elf-file> <payload>\n", argv[0]);
		exit(1);
	}

	init_elf(&elf, argv[1]);
	init_payload(&payload, argv[2]);
	parse_elf(&elf);
	parse_payload(&payload);
	printf("[*] Shellcode in payload file at:			0x%08lx\n", payload.txt_sec->sh_offset);
	printf(YEL "[*] Size of payload (in bytes):				%lu\n" RES, payload.txt_sec->sh_size);

	encrypt_light(&elf);
	patch_payload(&elf, &payload);

	if (payload.txt_sec->sh_size < elf.gap_size)
		inject_in_gap(&elf, &payload);
	else
		extend_and_inject(&elf, &payload);
	return (0);
}
