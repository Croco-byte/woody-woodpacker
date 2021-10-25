/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qroland <qroland@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 11:35:47 by user42            #+#    #+#             */
/*   Updated: 2021/10/25 13:15:07 by qroland          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

void	print_usage(char *prog_name)
{
	fprintf(stderr, "Usage: %s <elf-file> <payload> [TYPE] [ENCRYPTION MODE]\n\n",prog_name);
	fprintf(stderr, "Available options for TYPE :\n");
	fprintf(stderr, "--PDC:				(default) The binary to pack is position-dependent code.\n");
	fprintf(stderr, "				This means that the code is run from a particular address\n");
	fprintf(stderr, "				(often 0x400000 in x86_64). This is generally the way\n");
	fprintf(stderr, "				basic executables are compiled by default.\n\n");
	fprintf(stderr, "--PIC:				The binary to pack is position-independent code. This means\n");
	fprintf(stderr, "				that each time the executable runs, the process will have a\n");
	fprintf(stderr, "				different (random) address space. This is the way shared objects\n");
	fprintf(stderr, "				are compiled by default (and some 'normal' executables recently).\n\n");
	fprintf(stderr, "Available options for ENCRYPTION MODE :\n");
	fprintf(stderr, "--enc-basic:			(default) We will encrypt the text section of the binary. This\n");
	fprintf(stderr, "				is generally what encryption packers do, and generally enough.\n\n");
	fprintf(stderr, "--enc-full:			We will try to encrypt more sections. This will at least encrypt\n");
	fprintf(stderr, "				the .text and .data sections, and will also try to encrypt all the\n");
	fprintf(stderr, "				contingent sections that can be encrypted without breaking the binary.\n");
	fprintf(stderr, "				Still a bit experimental, and not available for the --PIC mode.\n\n");

	fprintf(stderr, "Supported PAYLOADS :\n");
	fprintf(stderr, "Position-dependent, basic encryption:	" YEL "pdc_basic\n" RES);
	fprintf(stderr, "Position-dependent, full encryption:	" YEL "pdc_full\n" RES);
	fprintf(stderr, "Position-independent, basic encryption:	" YEL "pic_basic\n" RES);
}

int		parse_flags(int argc, char **argv)
{
	if (argc == 3)
		return (PDC_BASIC);
	else if (argc == 4)
	{
		if (!strcmp(argv[3], "--PDC"))
			return (PDC_BASIC);
		else if (!strcmp(argv[3], "--PIC"))
			return (PIC_BASIC);
		else if (!strcmp(argv[3], "--enc-basic"))
			return (PDC_BASIC);
		else if (!strcmp(argv[3], "--enc-full"))
			return (PDC_FULL);
		else
			return (-1);
	}
	else if (argc == 5)
	{
		if (!strcmp(argv[3], "--PDC") && !strcmp(argv[4], "--enc-basic"))
			return (PDC_BASIC);
		else if (!strcmp(argv[3], "--PDC") && !strcmp(argv[4], "--enc-full"))
			return (PDC_FULL);
		else if (!strcmp(argv[3], "--PIC") && !strcmp(argv[4], "--enc-basic"))
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
	printf(YEL "[*] Size of payload (in bytes):				%lu\n" RES, payload.txt_sec->sh_size);

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

/*
> Regler le cas ou la taille du payload est superieure a bytes_to_add dans extend_and_inject		--> faire un abs()
> Transformer les strcmp en ft_strcmp
> Ajouter la description de la mécanique d'extension de segment dans Usage + ajouter des payloads qui illustrent ça
> Générer la clé aléatoirement
> Dans "usage", ajouter une section "testing" pour indiquer au correcteur comment tester
 */
