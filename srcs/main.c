/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 11:35:47 by user42            #+#    #+#             */
/*   Updated: 2021/10/20 16:25:58 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

void	inject_in_gap(t_elf *elf, t_payload *payload)
{
	printf(GRN "\n[+] Enough space to inject payload in gap. Injecting on the fly!\n" RES);
	memmove(elf->map + elf->gap_offset, payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size);
	printf("[*] Injected payload to offset:				0x%08lu\n", elf->gap_offset);
	printf("[*] Patching ELF entry point to payload\n");
	elf->header->e_entry = (Elf64_Addr)(elf->text_segment->p_vaddr + elf->gap_offset);
	write_to_output(elf->fname, elf->map, elf->fsize);
	printf(GRN "[+] Injection successfull\n" RES);
}

void	extend_and_inject(t_elf *elf, t_payload *payload)
{
	int			pages_to_add = ((payload->txt_sec->sh_size - elf->gap_size) / PAGE_SIZE) + 1;
	int			bytes_to_add = pages_to_add * PAGE_SIZE;
	char		*zeros;

	printf(GRN "\n[+] Not enough space in gap. Extending .text segment\n" RES);
	zeros = malloc((bytes_to_add - payload->txt_sec->sh_size) * sizeof(char));
	bzero(zeros, bytes_to_add - payload->txt_sec->sh_size);
	printf("[*] We need to add this many pages:			%u (we miss %lu bytes)\n", pages_to_add, payload->txt_sec->sh_size - elf->gap_size);
	Elf64_Off offset = extend_segment(elf->map, payload->txt_sec->sh_size, pages_to_add);
	printf("[*] The payload can now be injected at offset:		0x%08lx\n", offset);
	printf("[*] Patching ELF entry point to payload\n");
	elf->header->e_entry = elf->text_segment->p_vaddr + offset;
	printf("[*] Writing until injection point			(%lu bytes)\n", offset);
	write_to_output(elf->fname, elf->map, offset);
	printf("[*] Writing payload at injection point			(%lu bytes)\n",payload->txt_sec->sh_size);
	add_to_output(elf->fname, payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size);
	printf("[*] Writing additionnal padding				(%lu bytes)\n", bytes_to_add - payload->txt_sec->sh_size);
	add_to_output(elf->fname, zeros, bytes_to_add - payload->txt_sec->sh_size);
	printf("[*] Writing rest of parent file				(%lu bytes)\n", (elf->fsize) - (offset));
	add_to_output(elf->fname, elf->map + offset, (elf->fsize) - (offset));
	printf(GRN "[+] Injection successfull\n" RES);
}

void	encrypt_sections(t_elf *elf)
{
	Elf64_Shdr *sec_headers = (Elf64_Shdr *)(elf->map + elf->header->e_shoff);
	Elf64_Shdr *sec_str_h = &(sec_headers[elf->header->e_shstrndx]);
	char * sec_str_p = elf->map + sec_str_h->sh_offset;
	size_t j = 0;

	for (size_t i = 0; i < elf->header->e_shnum; i++)
	{
		if (sec_headers[i].sh_type == SHT_PROGBITS && sec_headers[i].sh_flags & SHF_ALLOC && sec_headers[i].sh_flags & SHF_EXECINSTR
		&& (strcmp(sec_str_p + sec_headers[i].sh_name, ".fini") && strcmp(sec_str_p + sec_headers[i].sh_name, ".init")))
		{
			printf("[*] Encrypting section %s\n", sec_str_p + sec_headers[i].sh_name);
			if (j == 0)
				elf->encrypted_start = (long)(sec_headers[i].sh_addr);
			elf->encrypted_size += sec_headers[i].sh_size;
			j = 0;
			char		*ptr = (char *)(elf->map + sec_headers[i].sh_offset);
			while (j < sec_headers[i].sh_size)
			{
				*ptr ^= 0xa5;
				j++;
				ptr++;
			}
		}
	}
}

void	encrypt_section(void *d, Elf64_Shdr *section)
{
	size_t		i = 0;
	char		*ptr = (char *)(d + section->sh_offset);

	while (i < section->sh_size)
	{
		*ptr ^= 0xa5;
		i++;
		ptr++;
	}
}

void	init_elf(t_elf *elf, char *fname)
{
	elf->fname				= fname;
	elf->map				= 0;
	elf->fsize				= 0;
	elf->header				= 0;
	elf->text_segment		= 0;
	elf->gap_offset			= 0;
	elf->gap_size			= 0;
	elf->encrypted_start	= 0;
	elf->encrypted_size		= 0;
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
	elf_find_gap(elf);
}

void	parse_payload(t_payload *payload)
{
	payload->map			= elf_map_file(payload->fname, &payload->fsize);
	payload->txt_sec		= elf_find_section(payload->map, ".text");
}

void	patch_payload(t_elf *elf, t_payload *payload, Elf64_Shdr *text_sec)
{
	(void)text_sec;
	Elf64_Addr	aligned_addr;

	aligned_addr = (text_sec->sh_addr);
	aligned_addr = ((PAGE_SIZE-1)&aligned_addr) ? ((aligned_addr) & ~(PAGE_SIZE-1)) : aligned_addr;

	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x1111111111111111, (long)elf->header->e_entry);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x2222222222222222, (long)0xa5);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x3333333333333333, elf->encrypted_start);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x4444444444444444, elf->encrypted_size);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x5555555555555555, aligned_addr);
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

	/* === Encryption experimentation === */
	Elf64_Shdr *text_sec = elf_find_section(elf.map, ".text");
	encrypt_sections(&elf);
//	encrypt_section(elf.map, text_sec);
	patch_payload(&elf, &payload, text_sec);
	/* === End of experimentation === */

	if (payload.txt_sec->sh_size < elf.gap_size)
		inject_in_gap(&elf, &payload);
	else
		extend_and_inject(&elf, &payload);

	return (0);
}
