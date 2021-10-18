/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 11:35:47 by user42            #+#    #+#             */
/*   Updated: 2021/10/17 15:14:51 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

size_t	ft_strlen(char *s)
{
	size_t	i;

	i = 0;
	if (!s)
		return (0);
	while (s[i])
		i++;
	return (i);
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	if (n == 0)
		return (0);
	while (s1[i] && s2[i] && s1[i] == s2[i] && i < (n - 1))
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

int		get_file_size(int fd, char *fname)
{
	struct stat		st;
	
	if (fstat(fd, &st) != 0)
	{
		fprintf(stderr, "[!] Error: couldn't read informations for file %s\n", fname);
		exit(1);
	}
	return (st.st_size);
}

void	write_to_output(char *name, void *data, size_t size)
{
	int		fdout;
	char	*outname;

	outname = malloc((ft_strlen(name) + 8) * sizeof(char));
	sprintf(outname, "%s.packed", name);
	if ((fdout = open(outname, O_CREAT | O_WRONLY | O_TRUNC, 0666)) == -1)
	{
		fprintf(stderr, "[!] Error: couldn't open outfile for writing\n");
		exit(1);
	}
	write(fdout, data, size);
	free(outname);
}

void	add_to_output(char *name, void *data, size_t size)
{
	int		fdout;
	char	*outname;

	outname = malloc((ft_strlen(name) + 8) * sizeof(char));
	sprintf(outname, "%s.packed", name);
	if ((fdout = open(outname, O_CREAT | O_WRONLY | O_APPEND, 0666)) == -1)
	{
		fprintf(stderr, "[!] Error: couldn't open outfile for writing\n");
		exit(1);
	}
	write(fdout, data, size);
	free(outname);
}

int		elf_open_and_map(char *fname, void **data, int *size)
{
	int		fd;

	if ((fd = open(fname, O_APPEND | O_RDWR, 0)) < 0)
	{
		fprintf(stderr, "[!] Error: couldn't open file %s\n", fname);
		exit(1);
	}
	*size = get_file_size(fd, fname);
	*data = mmap(0, *size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, fd, 0);
	if (*data == MAP_FAILED)
	{
		fprintf(stderr, "[!] Error: couldn't map file %s\n", fname);
		exit(1);
	}
	printf("[+] Mapped file %s to		%p (%d bytes)\n", fname, *data, *size);
	return (fd);
}

Elf64_Phdr	*elf_find_gap(void *d, int fsize, int *gap_offset, size_t *gap_size)
{
	Elf64_Ehdr	*header = (Elf64_Ehdr *)d;
	Elf64_Phdr	*segments = (Elf64_Phdr *)(d + header->e_phoff);
	Elf64_Phdr	*text_segment = 0;
	int			text_seg_end;
	int			gap = fsize;

	for (int i = 0; i < header->e_phnum; i++)
	{
		if (segments[i].p_type == PT_LOAD && segments[i].p_flags & PF_X)
		{
			printf("[*] .text segment (#%d) at offset	0x%08lx\n", i, segments[i].p_offset);
			text_segment = &(segments[i]);
			text_seg_end = (text_segment->p_offset + text_segment->p_filesz);
			printf("[*] .text segment size :		0x%08lx\n", segments[i].p_filesz);
			printf("[*] .text segment end :			0x%08x\n", text_seg_end);
		}
		else if (text_segment && segments[i].p_type == PT_LOAD && segments[i].p_offset - text_seg_end < (size_t)gap)
		{
			printf("[*] following loadable segment (#%d):	0x%08lx\n", i, segments[i].p_offset);
			gap = segments[i].p_offset - text_seg_end;
			printf("[*] gap between .text and following:	%d\n", gap);

		}
	}
	*gap_offset = text_seg_end;
	*gap_size = gap;
	printf("[+] Text segment gap available at:	0x%08x (%lu bytes available)\n", *gap_offset, *gap_size);
	return (text_segment);
}

Elf64_Shdr	*elf_find_section(void *data, char *sec_name)
{
	Elf64_Ehdr	*header = (Elf64_Ehdr *)data;
	Elf64_Shdr	*sections = (Elf64_Shdr *)(data + header->e_shoff);
	Elf64_Shdr	*sec_strtab_h = &sections[header->e_shstrndx];
	const char	*sec_strtab_p = data + sec_strtab_h->sh_offset;

	for (int i = 0; i < header->e_shnum; i++)
	{
		if (ft_strncmp(sec_strtab_p + (sections[i].sh_name), sec_name, ft_strlen(sec_name)) == 0)
			return (&sections[i]);
	}
	return (0);
}

int		elf_mem_subst(void *m, int size, long pat, long val)
{
	unsigned char *p = (unsigned char *)m;

	for (int i = 0; i < size; i++)
	{
		if (*(long *)(p + i) == pat)
		{
			printf("[*] Found pattern at offset %d | 0x%016lx -> 0x%016lx\n", i, pat, val);
			*(long *)(p + i) = val;
			return (0);
		}
	}
	printf("Couldn't find pattern\n");
	return (-1);
}

uint64_t	extend_segment(void *d, size_t payload_size)
{
	Elf64_Ehdr	*header = (Elf64_Ehdr *)d;
	Elf64_Phdr	*segments = (Elf64_Phdr *)(d + header->e_phoff);
	Elf64_Shdr	*sections = (Elf64_Shdr *)(d + header->e_shoff);
	uint64_t offset = 0;
	uint64_t evaddr;

	for (int i = 0; i < header->e_phnum ; i++)
	{
		if (offset)
			segments[i].p_offset += PAGE_SIZE;
		else if (segments[i].p_type == PT_LOAD && segments[i].p_offset == 0)
		{
			evaddr = segments[i].p_vaddr + segments[i].p_filesz;
			offset = segments[i].p_offset + segments[i].p_filesz;
			printf("[DEBUG] The offset at which we will inject the payload is : %lu\n", offset);
			segments[i].p_filesz += payload_size;
			segments[i].p_memsz += payload_size;
		}
	}

	for (int i = 0; i < header->e_shnum; i++)
	{
		if (sections[i].sh_offset > offset)
		{
			printf("[DEBUG] Patching offset for section #%d\n", i);
			sections[i].sh_offset += PAGE_SIZE;
		}
		else if (sections[i].sh_addr + sections[i].sh_size == evaddr)
		{
			printf("[DEBUG] Adding payload size to sh_size for section #%d\n", i);
			sections[i].sh_size += payload_size;
		}
	}

	if (header->e_shoff > offset)
		header->e_shoff += PAGE_SIZE;

	return (offset);
}


int		main(int argc, char **argv)
{
	void	*d;
	void	*d1;
	int		targetfd;
	int		payloadfd;
	int		fsize;
	int		fsize1;

	int		gap_offset;
	size_t	gap_size;

	printf("[*] Basic segment padding infection\n");
	if (argc != 3)
	{
		fprintf(stderr, "[!] Usage: %s <elf-file> <payload>\n", argv[0]);
		exit(1);
	}

	targetfd = elf_open_and_map(argv[1], &d, &fsize);
	payloadfd = elf_open_and_map(argv[2], &d1, &fsize1);

	Elf64_Ehdr *header = (Elf64_Ehdr *)d;
	printf("[*] Entry point of program is		0x%08lx\n", header->e_entry);

	Elf64_Phdr *text_segment = elf_find_gap(d, fsize, &gap_offset, &gap_size);

	Elf64_Shdr *payload_text_sec = elf_find_section(d1, ".text");
	printf("[*] Payload file, shellcode at		0x%08lx (%lu bytes)\n", payload_text_sec->sh_offset, payload_text_sec->sh_size);

/*	if (payload_text_sec->sh_size < gap_size)
	{
		printf("[+] Padding gap after .text segment big enough for payload. Injecting on the fly\n");
		memmove(d + gap_offset, d1 + payload_text_sec->sh_offset, payload_text_sec->sh_size);
		elf_mem_subst(d + gap_offset, payload_text_sec->sh_size, 0x1111111111111111, (long)header->e_entry);
		header->e_entry = (Elf64_Addr)(text_segment->p_vaddr + gap_offset);
		printf("[*] e_entry is now 0x%08lx\n", header->e_entry);
		write_to_output(argv[1], d, fsize);
		printf("[+] Infection successfull\n");
	}
	else
	{ */
		char test[PAGE_SIZE];
		bzero((void *)test, PAGE_SIZE);
		uint64_t offset = extend_segment(d, payload_text_sec->sh_size);
		printf("[DEBUG] Size of original binary is %d\n", fsize);
		printf("[DEBUG] Writing file start to output for len %lu\n", offset);
		write_to_output(argv[1], d, offset);

		elf_mem_subst(d1 + payload_text_sec->sh_offset, payload_text_sec->sh_size, 0x1111111111111111, (long)header->e_entry);

		printf("[DEBUG] Writing payload to output for len %lu\n", payload_text_sec->sh_size);
		add_to_output(argv[1], d1 + payload_text_sec->sh_offset, payload_text_sec->sh_size);

		printf("[DEBUG] Writing padding after payload for len %lu\n", PAGE_SIZE - payload_text_sec->sh_size);
		add_to_output(argv[1], test, PAGE_SIZE - payload_text_sec->sh_size);

//		add_to_output(argv[1], test, PAGE_SIZE);
		printf("[DEBUG] Writing rest of file to output for len %lu\n", (fsize) - (offset));
		add_to_output(argv[1], d + offset, (fsize) - (offset));

		int osize, outfd;
		void *o;

		outfd = open("binary64.packed", O_APPEND | O_RDWR, 0);
		osize = get_file_size(outfd, "");
		o = mmap(0, osize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, outfd, 0);

		Elf64_Ehdr *outheader = (Elf64_Ehdr *)o;
		outheader->e_entry = text_segment->p_vaddr + offset;
		printf("[DEBUG] Setting the entry point to 0x%08lx\n", outheader->e_entry);

//	}


	close(targetfd);
	close(payloadfd);
	return (0);
}
