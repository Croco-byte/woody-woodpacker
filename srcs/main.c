/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 11:35:47 by user42            #+#    #+#             */
/*   Updated: 2021/10/19 15:26:15 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

void	inject_in_gap(	void *d,
						void *d1,
						size_t gap_offset,
						Elf64_Shdr *payload_shellcode,
						Elf64_Phdr *text_segment)
{
	Elf64_Ehdr *header = (Elf64_Ehdr *)d;

	printf(GRN "\n[+] Enough space to inject payload in gap. Injecting on the fly!\n" RES);
	memmove(d + gap_offset, d1 + payload_shellcode->sh_offset, payload_shellcode->sh_size);
	printf("[*] Injected payload to offset:				0x%08lu\n", gap_offset);
//	elf_mem_subst(d + gap_offset, payload_shellcode->sh_size, 0x1111111111111111, (long)header->e_entry);
	printf("[*] Patching ELF entry point to payload\n");
	header->e_entry = (Elf64_Addr)(text_segment->p_vaddr + gap_offset);
}

void	extend_and_inject(	void *d,
							void *d1,
							size_t gap_size,
							Elf64_Shdr *payload_text_sec,
							Elf64_Phdr *text_segment,
							char *fname,
							int fsize)
{
	Elf64_Ehdr	*header = (Elf64_Ehdr *)d;
	int			pages_to_add = ((payload_text_sec->sh_size - gap_size) / PAGE_SIZE) + 1;
	int			bytes_to_add = pages_to_add * PAGE_SIZE;
	char		*zeros;

	printf(GRN "\n[+] Not enough space in gap. Extending .text segment\n" RES);
	zeros = malloc((bytes_to_add - payload_text_sec->sh_size) * sizeof(char));
	bzero(zeros, bytes_to_add - payload_text_sec->sh_size);
	printf("[*] We need to add this many pages:			%u (we miss %lu bytes)\n", pages_to_add, payload_text_sec->sh_size - gap_size);
	Elf64_Off offset = extend_segment(d, payload_text_sec->sh_size, pages_to_add);
	printf("[*] The payload can now be injected at offset:		0x%08lx\n", offset);
//	elf_mem_subst(d1 + payload_text_sec->sh_offset, payload_text_sec->sh_size, 0x1111111111111111, (long)header->e_entry);
	printf("[*] Patching ELF entry point to payload\n");
	header->e_entry = text_segment->p_vaddr + offset;
	printf("[*] Writing until injection point			(%lu bytes)\n", offset);
	write_to_output(fname, d, offset);
	printf("[*] Writing payload at injection point			(%lu bytes)\n", payload_text_sec->sh_size);
	add_to_output(fname, d1 + payload_text_sec->sh_offset, payload_text_sec->sh_size);
	printf("[*] Writing additionnal padding				(%lu bytes)\n", bytes_to_add - payload_text_sec->sh_size);
	add_to_output(fname, zeros, bytes_to_add - payload_text_sec->sh_size);
	printf("[*] Writing rest of parent file				(%lu bytes)\n", (fsize) - (offset));
	add_to_output(fname, d + offset, (fsize) - (offset));
	printf(GRN "[+] Injection successfull\n" RES);
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

int		main(int argc, char **argv)
{
	void	*d, *d1;
	int		fsize, fsize1;
	size_t	gap_offset, gap_size;

	printf("[*] ... Launching woody ...\n");
	if (argc != 3)
	{
		fprintf(stderr, "[!] Usage: %s <elf-file> <payload>\n", argv[0]);
		exit(1);
	}

	d = elf_map_file(argv[1], &fsize);
	d1 = elf_map_file(argv[2], &fsize1);
	Elf64_Ehdr *header = (Elf64_Ehdr *)d;
	Elf64_Phdr *text_segment = elf_find_gap(d, fsize, &gap_offset, &gap_size);
	Elf64_Shdr *payload_text_sec = elf_find_section(d1, ".text");
	printf("[*] Shellcode in payload file at:			0x%08lx\n", payload_text_sec->sh_offset);
	printf(YEL "[*] Size of payload (in bytes):				%lu\n" RES,  payload_text_sec->sh_size);

	/* === Encryption experimentation === */
	Elf64_Shdr *text_sec = elf_find_section(d, ".text");
	encrypt_section(d, text_sec);
	elf_mem_subst(d1 + payload_text_sec->sh_offset, payload_text_sec->sh_size, 0x1111111111111111, (long)header->e_entry);
	elf_mem_subst(d1 + payload_text_sec->sh_offset, payload_text_sec->sh_size, 0x2222222222222222, (long)0xa5);
	elf_mem_subst(d1 + payload_text_sec->sh_offset, payload_text_sec->sh_size, 0x3333333333333333, text_sec->sh_addr);
	elf_mem_subst(d1 + payload_text_sec->sh_offset, payload_text_sec->sh_size, 0x4444444444444444, text_sec->sh_size);
	/* === End of experimentation === */

	if (payload_text_sec->sh_size < gap_size)
	{
		inject_in_gap(d, d1, gap_offset, payload_text_sec, text_segment);
		write_to_output(argv[1], d, fsize);
		printf(GRN "[+] Injection successfull\n" RES);
	}
	else
		extend_and_inject(d, d1, gap_size, payload_text_sec, text_segment, argv[1], fsize);

	return (0);
}
