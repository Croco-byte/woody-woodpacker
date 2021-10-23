/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inject.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/22 12:07:26 by qroland           #+#    #+#             */
/*   Updated: 2021/10/22 17:23:44 by user42           ###   ########.fr       */
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
    printf("DEBUG requesting %lu bytes | bytes_to_add is %d | payload size is %lu\n", (bytes_to_add - payload->txt_sec->sh_size), bytes_to_add, payload->txt_sec->sh_size);
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
	printf("[*] Injected payload to offset:				0x%08lx\n", elf->injection_point);
	printf("[*] Patching ELF entry point to payload 0x%08lx\n", elf->text_segment->p_vaddr + (elf->injection_point - elf->text_segment->p_offset));
	elf->header->e_entry = (Elf64_Addr)(elf->text_segment->p_vaddr + (elf->injection_point - elf->text_segment->p_offset));
	write_in_gap(elf);
}

void	extend_and_inject(t_elf *elf, t_payload *payload)
{
	int pages_to_add = ((payload->txt_sec->sh_size - elf->gap_size) / PAGE_SIZE) + 1;

	printf(GRN "\n[+] Not enough space in gap. Extending .text segment\n" RES);
	printf("[*] We need to add this many pages:			%u (missing %lu bytes)\n", pages_to_add, payload->txt_sec->sh_size - elf->gap_size);
	extend_segment(elf, payload->txt_sec->sh_size, pages_to_add);
	printf("[*] The payload can now be injected at offset:		0x%08lx (%lu bytes into file)\n", elf->injection_point, elf->injection_point);
	printf("[*] Patching ELF entry point to payload\n");
	elf->header->e_entry = elf->text_segment->p_vaddr + elf->injection_point;
	write_after_extension(elf, payload, pages_to_add);
}
