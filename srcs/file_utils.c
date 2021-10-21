/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 11:47:53 by user42            #+#    #+#             */
/*   Updated: 2021/10/21 23:06:53 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody-woodpacker.h"

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
	close(fdout);
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
	close(fdout);
	free(outname);
}

void	*elf_map_file(char *fname, uint64_t *size)
{
	int		fd;
	void	*map;

	if ((fd = open(fname, O_APPEND | O_RDWR, 0)) < 0)
	{
		fprintf(stderr, "[!] Error: couldn't open file %s\n", fname);
		exit(1);
	}
	*size = get_file_size(fd, fname);
	map = mmap(0, *size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, fd, 0);
	if (map == MAP_FAILED)
	{
		fprintf(stderr, "[!] Error: couldn't map file %s\n", fname);
		exit(1);
	}
	printf("[*] Mapped file %s to				%p (%lu bytes)\n", fname, map, *size);
	close(fd);
	return (map);
}
