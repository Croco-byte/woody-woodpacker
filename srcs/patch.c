/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   patch.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/22 12:04:18 by qroland           #+#    #+#             */
/*   Updated: 2021/10/26 13:03:00 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

void    patch_payload_basic(t_elf *elf, t_payload *payload)
{
    Elf64_Addr	aligned_addr;

	aligned_addr = (elf->enc_start_before);
	aligned_addr = ((PAGE_SIZE-1)&aligned_addr) ? ((aligned_addr) & ~(PAGE_SIZE-1)) : aligned_addr;

	printf("[*] Patching ret addr to original entry point:		0x%016lx -> 0x%016lx\n", 0x1111111111111111, (long)elf->header->e_entry);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x1111111111111111, (long)elf->header->e_entry);
	printf("[*] Patching key:					0x%016lx -> 0x%016lx\n", 0x2222222222222222, elf->key);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x2222222222222222, elf->key);
	printf("[*] Patching encrypted sections start (before):		0x%016lx -> 0x%016lx\n", 0x3333333333333333, elf->enc_start_before);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x3333333333333333, elf->enc_start_before);
	printf("[*] Patching encrypted sections size (before):		0x%016lx -> 0x%016lx\n", 0x4444444444444444, elf->enc_size_before);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x4444444444444444, elf->enc_size_before);
	printf("[*] Patching mprotect aligned addr (before):		0x%016lx -> 0x%016lx\n", 0x555555555555555, aligned_addr);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x5555555555555555, aligned_addr);
}

void	patch_payload_full(t_elf *elf, t_payload *payload)
{
	Elf64_Addr	aligned_addr_before;
	Elf64_Addr	aligned_addr_after;

	aligned_addr_before = (elf->enc_start_before);
	aligned_addr_before = ((PAGE_SIZE-1)&aligned_addr_before) ? ((aligned_addr_before) & ~(PAGE_SIZE-1)) : aligned_addr_before;
	aligned_addr_after = (elf->enc_start_after);
	aligned_addr_after = ((PAGE_SIZE-1)&aligned_addr_after) ? ((aligned_addr_after) & ~(PAGE_SIZE-1)) : aligned_addr_after;

	printf("[*] Patching ret addr to original entry point:		0x%016lx -> 0x%016lx\n", 0x1111111111111111, (long)elf->header->e_entry);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x1111111111111111, (long)elf->header->e_entry);
	printf("[*] Patching key:					0x%016lx -> 0x%016lx\n", 0x2222222222222222, elf->key);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x2222222222222222, elf->key);
	printf("[*] Patching encrypted sections start (before):		0x%016lx -> 0x%016lx\n", 0x3333333333333333, elf->enc_start_before);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x3333333333333333, elf->enc_start_before);
	printf("[*] Patching encrypted sections size (before):		0x%016lx -> 0x%016lx\n", 0x4444444444444444, elf->enc_size_before);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x4444444444444444, elf->enc_size_before);
	printf("[*] Patching mprotect aligned addr (before):		0x%016lx -> 0x%016lx\n", 0x555555555555555, aligned_addr_before);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x5555555555555555, aligned_addr_before);
	printf("[*] Patching encrypted sections start (after):		0x%016lx -> 0x%016lx\n", 0x6666666666666666, elf->enc_start_after);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x6666666666666666, elf->enc_start_after);
	printf("[*] Patching encrypted sections size (after):		0x%016lx -> 0x%016lx\n", 0x7777777777777777, elf->enc_size_after);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x7777777777777777, elf->enc_size_after);
	printf("[*] Patching mprotect aligned addr (after):		0x%016lx -> 0x%016lx\n", 0x8888888888888888, aligned_addr_after);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x8888888888888888, aligned_addr_after);
}

void	patch_payload_pic(t_elf *elf, t_payload *payload)
{
	printf("[*] Patching ret addr offset:				0x%016lx -> 0x%016lx\n", 0x1111111111111111, elf->injection_point - elf->text_segment->p_offset);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x1111111111111111, elf->injection_point - elf->text_segment->p_offset);
	printf("[*] Patching key:					0x%016lx -> 0x%016lx\n", 0x2222222222222222, elf->key);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x2222222222222222, elf->key);
	printf("[*] Patching encrypted code offset:			0x%016lx -> 0x%016lx\n", 0x3333333333333333, elf->enc_start_before - elf->text_segment->p_vaddr);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x3333333333333333, elf->enc_start_before - elf->text_segment->p_vaddr);
	printf("[*] Patching encrypted size:				0x%016lx -> 0x%016lx\n", 0x4444444444444444, elf->enc_size_before);
	elf_mem_subst(payload->map + payload->txt_sec->sh_offset, payload->txt_sec->sh_size, 0x4444444444444444, elf->enc_size_before);
}
