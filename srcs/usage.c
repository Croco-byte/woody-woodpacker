/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   usage.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/26 12:17:44 by user42            #+#    #+#             */
/*   Updated: 2021/10/26 12:52:10 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

void	print_usage(char *prog_name)
{
	fprintf(stderr, BLU "\n	=== WOODY-WOPACKER --- Informations ===\n" RES);
	fprintf(stderr, "Woody-woopacker is a packer that encrypts an ELF64 executable (this type\n");
	fprintf(stderr, "of packer is known as a Crypter). After encrypting, it will inject in the file a\n");
	fprintf(stderr, "small piece of executable code that will allow the binary to decrypt itself\n");
	fprintf(stderr, "before executing. This executable code will be injected at the end of the text\n");
	fprintf(stderr, "segment. ELF64 segments are aligned to page length (4096 bytes) ; which means\n");
	fprintf(stderr, "that there often will be some padding at the end of the segment. To maximize stealth\n");
	fprintf(stderr, "and efficiency, if the padding is big enough for our payload, we will use this space\n");
	fprintf(stderr, "to inject the payload. If it isn't big enough, we will extend the text segment of the\n");
	fprintf(stderr, "original file in order to fit the payload.\n");
	fprintf(stderr, "This crypter can also encrypt PIC (Position Independent Code), by leaking AT RUNTIME\n");
	fprintf(stderr, "the address space of the program and deducing the rest with offsets. The leak is triggered\n");
	fprintf(stderr, "by pushing RIP on the stack, and popping it in a general purpose register.\n");

	fprintf(stderr, BLU "\n	=== WOODY-WOPACKER --- Usage ===\n" RES);
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
	fprintf(stderr, "Position-dependent:	" YEL "pdc_basic | pdc_full\n" RES);
	fprintf(stderr, "Position-independent:	" YEL "pic_basic\n" RES);

	fprintf(stderr, BLU "\n	=== WOODY-WOPACKER --- Testing ===\n" RES);
	fprintf(stderr, "Supported payloads are in ./payloads ; some testing exectutables are in ./binaries.\n\n");
	fprintf(stderr, ">> Testing a classic executable, with basic encryption :\n");
	fprintf(stderr, "	./woody-woodpacker binaries/sample64 payloads/pdc_basic --PDC --enc-basic\n");
	fprintf(stderr, ">> Testing a classic executable, with full encryption :\n");
	fprintf(stderr, "	./woody-woodpacker binaries/sample64 payloads/pdc_full --PDC --enc-full\n");
	fprintf(stderr, ">> Testing a PIC executable, with basic encryption :\n");
	fprintf(stderr, "	./woody-woodpacker binaries/sample64_pic payloads/pic_basic --PIC --enc-basic\n");
	fprintf(stderr, "\nNote : the segment extension won't trigger very often, since there will generally be enough\n");
	fprintf(stderr, "space in the text segment padding for the payload. To trigger it, use artificially big payloads\n");
	fprintf(stderr, "in the ./payloads/big_payloads directory. The synthax is the same, for example :\n");
	fprintf(stderr, "\n>> Testing a classic executable, with basic encryption, and a big payload to trigger segment extension :\n");
	fprintf(stderr, "	./woody-woodpacker binaries/sample64 payloads/big_payloads/pdc_basic_big --PDC --enc-basic\n");
}
