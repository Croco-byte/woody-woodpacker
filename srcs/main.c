/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qroland <qroland@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 11:35:47 by user42            #+#    #+#             */
/*   Updated: 2021/10/22 15:07:34 by qroland          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

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

	encrypt_basic(&elf);
	patch_payload_basic(&elf, &payload);

	if (payload.txt_sec->sh_size < elf.gap_size)
		inject_in_gap(&elf, &payload);
	else
		extend_and_inject(&elf, &payload);
	return (0);
}

/* Regler le cas ou la taille du payload est superieure a bytes_to_add in extend_and_inject
gcc (Ubuntu 10.3.0-1ubuntu1~20.04) 10.3.0
 */