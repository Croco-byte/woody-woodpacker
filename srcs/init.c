/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qroland <qroland@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/22 11:56:05 by qroland           #+#    #+#             */
/*   Updated: 2021/10/22 11:56:51 by qroland          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

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