/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inject.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/22 12:07:26 by qroland           #+#    #+#             */
/*   Updated: 2021/10/26 12:05:44 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

void	write_in_gap(t_elf *elf)
{
	write_to_output(elf->map, elf->fsize);
	printf(GRN "[+] Injection complete\n" RES);
	printf(YEL "[*] Encryption key is 0x%016lx\n" RES, elf->key);
}

int		get_padding(int pages_to_add, uint64_t payload_size, uint64_t gap_size)
{
	int 		bytes_in_last_added_page;
	int			padding;

	bytes_in_last_added_page = (payload_size - gap_size) - (PAGE_SIZE * (pages_to_add - 1));
	padding = PAGE_SIZE - bytes_in_last_added_page;
	return (padding);
}

void	write_after_extension(t_elf *elf, t_payload *payload, int pages_to_add)
{
	int			padding = get_padding(pages_to_add, payload->txt_sec->sh_size, elf->gap_size);
	char		*zeros;

	zeros = malloc(padding * sizeof(char));
	bzero(zeros, padding);
	printf("[*] Writing until injection point			(%lu bytes)\n", elf->injection_point);
	write_to_output(elf->map, elf->injection_point);
	printf("[*] Writing payload at injection point			(%lu bytes)\n",payload->txt_sec->sh_size);
	add_to_output(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size);
	printf("[*] Writing additionnal padding				(%d bytes)\n", padding);
	add_to_output(zeros, padding);
	printf("[*] Writing rest of parent file				(%lu bytes)\n", (elf->fsize) - (elf->injection_point));
	add_to_output(elf->map + elf->injection_point + elf->gap_size, (elf->fsize) - (elf->injection_point));
	printf(GRN "[+] Injection complete\n" RES);
	printf(YEL "[*] Encryption key is 0x%016lx\n" RES, elf->key);
	free(zeros);
}

void	inject_in_gap(t_elf *elf, t_payload *payload)
{
	printf(GRN "[+] Enough space to inject payload in gap. Injecting on the fly!\n" RES);
	memmove(elf->map + elf->injection_point, payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size);
	printf("[*] Injected payload to offset:				0x%08lx\n", elf->injection_point);
	printf("[*] Patching ELF entry point to payload			0x%08lx\n", elf->text_segment->p_vaddr + (elf->injection_point - elf->text_segment->p_offset));
	elf->header->e_entry = (Elf64_Addr)(elf->text_segment->p_vaddr + (elf->injection_point - elf->text_segment->p_offset));
	write_in_gap(elf);
}

void	extend_and_inject(t_elf *elf, t_payload *payload)
{
	int pages_to_add = ((payload->txt_sec->sh_size - elf->gap_size) / PAGE_SIZE) + 1;

	printf(GRN "[+] Not enough space in gap. Extending .text segment\n" RES);
	printf("[*] We need to add this many pages:			%u (missing %lu bytes)\n", pages_to_add, payload->txt_sec->sh_size - elf->gap_size);
	extend_segment(elf, payload->txt_sec->sh_size, pages_to_add);
	printf("[*] The payload can now be injected at offset:		0x%08lx (%lu bytes into file)\n", elf->injection_point, elf->injection_point);
	printf("[*] Patching ELF entry point to payload			0x%08lx\n", elf->text_segment->p_vaddr + (elf->injection_point - elf->text_segment->p_offset));
	elf->header->e_entry = elf->text_segment->p_vaddr + (elf->injection_point - elf->text_segment->p_offset);
	write_after_extension(elf, payload, pages_to_add);
}
