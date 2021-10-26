/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 11:35:47 by user42            #+#    #+#             */
/*   Updated: 2021/10/26 12:58:43 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

int		parse_flags(int argc, char **argv)
{
	if (argc == 3)
		return (PDC_BASIC);
	else if (argc == 4)
	{
		if (!ft_strcmp(argv[3], "--PDC"))
			return (PDC_BASIC);
		else if (!ft_strcmp(argv[3], "--PIC"))
			return (PIC_BASIC);
		else if (!ft_strcmp(argv[3], "--enc-basic"))
			return (PDC_BASIC);
		else if (!ft_strcmp(argv[3], "--enc-full"))
			return (PDC_FULL);
		else
			return (-1);
	}
	else if (argc == 5)
	{
		if (!ft_strcmp(argv[3], "--PDC") && !ft_strcmp(argv[4], "--enc-basic"))
			return (PDC_BASIC);
		else if (!ft_strcmp(argv[3], "--PDC") && !ft_strcmp(argv[4], "--enc-full"))
			return (PDC_FULL);
		else if (!ft_strcmp(argv[3], "--PIC") && !ft_strcmp(argv[4], "--enc-basic"))
			return (PIC_BASIC);
		else
			return (-1);
	}
	else
		return (-1);
}

void	enc_patch_pdc_basic(t_elf *elf, t_payload *payload)
{
	encrypt_basic(elf);
	patch_payload_basic(elf, payload);
}

void	enc_patch_pdc_full(t_elf *elf, t_payload *payload)
{
	encrypt_full(elf);
	patch_payload_full(elf, payload);
}

void	enc_patch_pic_basic(t_elf *elf, t_payload *payload)
{
	encrypt_basic(elf);
	patch_payload_pic(elf, payload);
}

int		main(int argc, char **argv)
{
	t_elf		elf;
	t_payload	payload;
	int			options;

	srand(time(NULL));
	if (argc < 3)
	{
		print_usage(argv[0]);
		exit(1);
	}
	options = parse_flags(argc, argv);
	if (options == -1)
	{
		fprintf(stderr, RED "[!] Invalid flags in woody command\n\n" RES);
		print_usage(argv[0]);
		exit(1);
	}
	printf("[*] ... Launching woody ...\n");

	init_elf(&elf, argv[1]);
	init_payload(&payload, argv[2]);
	parse_elf(&elf);
	parse_payload(&payload);
	printf("[*] Shellcode in payload file at:			0x%08lx\n", payload.txt_sec->sh_offset);
	printf("[*] Size of payload (in bytes):				%lu\n", payload.txt_sec->sh_size);

	if (options == PDC_BASIC)
		enc_patch_pdc_basic(&elf, &payload);
	if (options == PDC_FULL)
		enc_patch_pdc_full(&elf, &payload);
	if (options == PIC_BASIC)
		enc_patch_pic_basic(&elf, &payload);

	if (payload.txt_sec->sh_size < elf.gap_size)
		inject_in_gap(&elf, &payload);
	else
		extend_and_inject(&elf, &payload);
	return (0);
}
