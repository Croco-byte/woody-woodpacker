/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qroland <qroland@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/22 11:56:58 by qroland           #+#    #+#             */
/*   Updated: 2021/10/22 11:57:26 by qroland          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

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