/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qroland <qroland@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 11:47:53 by user42            #+#    #+#             */
/*   Updated: 2021/10/25 15:24:42 by qroland          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

int		get_file_size(int fd, char *fname)
{
	struct stat		st;
	
	if (fstat(fd, &st) != 0)
	{
		fprintf(stderr, RED "[!] Error: couldn't read informations for file %s\n" RES, fname);
		exit(1);
	}
	return (st.st_size);
}

void	write_to_output(void *data, size_t size)
{
	int		fdout;

	if ((fdout = open("woody", O_CREAT | O_WRONLY | O_TRUNC, 0755)) == -1)
	{
		fprintf(stderr, RED "[!] Error: couldn't open outfile for writing\n" RES);
		exit(1);
	}
	write(fdout, data, size);
	close(fdout);
}

void	add_to_output(void *data, size_t size)
{
	int		fdout;

	if ((fdout = open("woody", O_CREAT | O_WRONLY | O_APPEND, 0755)) == -1)
	{
		fprintf(stderr, RED "[!] Error: couldn't open outfile for writing\n" RES);
		exit(1);
	}
	write(fdout, data, size);
	close(fdout);
}

void	*elf_map_file(char *fname, uint64_t *size)
{
	int		fd;
	void	*map;

	if ((fd = open(fname, O_APPEND | O_RDWR, 0)) < 0)
	{
		fprintf(stderr, RED "[!] Error: couldn't open file %s\n" RES, fname);
		exit(1);
	}
	*size = get_file_size(fd, fname);
	map = mmap(0, *size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, fd, 0);
	if (map == MAP_FAILED)
	{
		fprintf(stderr, RED "[!] Error: couldn't map file %s\n" RES, fname);
		exit(1);
	}
	printf("[*] Mapped file %-35s	%p (%lu bytes)\n", fname, map, *size);
	close(fd);
	return (map);
}
