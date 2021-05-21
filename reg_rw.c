/*
 * This file is part of the Xilinx DMA IP Core driver tools for Linux
 *
 * Copyright (c) 2016-present,  Xilinx, Inc.
 * All rights reserved.
 *
 * This source code is licensed under BSD-style license (found in the
 * LICENSE file in the root directory of this source tree)
 */

//オリジナル：https://github.com/Xilinx/dma_ip_drivers/blob/master/XDMA/linux-kernel/tools/reg_rw.c
//2021/05/14 M2 中井

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <byteswap.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>

#include <sys/types.h>
#include <sys/mman.h>

/* ltoh: little to host */
/* htol: little to host */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define ltohl(x)       (x)
#define ltohs(x)       (x)
#define htoll(x)       (x)
#define htols(x)       (x)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define ltohl(x)     __bswap_32(x)
#define ltohs(x)     __bswap_16(x)
#define htoll(x)     __bswap_32(x)
#define htols(x)     __bswap_16(x)
#endif

#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", __LINE__, __FILE__, errno, strerror(errno)); exit(1); } while(0)

//#define DEBUG 1

// 1 MiB
#define MAP_SIZE (1*1024*1024UL)
#define MAP_MASK (MAP_SIZE - 1)

typedef enum {
	MODE_READ,
	MODE_WRITE
} rwmode;

void* map_base;

uint32_t read_from_device(off_t target_addr){
	/* calculate the virtual address to be accessed */
	void* virt_addr = map_base + target_addr;
	uint32_t read_result = *((uint32_t *) virt_addr);
	/* swap 32-bit endianess if host is not little-endian */
	read_result = ltohl(read_result);
#ifdef DEBUG
	printf("Read 32-bit value at address 0x%08x (%p): 0x%08x\n",
			(unsigned int)target_addr, virt_addr,
			(unsigned int)read_result);
	fflush(stdout);
#endif
	return read_result;
}

void write_to_device(off_t target_addr, uint32_t writeval){
	/* calculate the virtual address to be accessed */
	void* virt_addr = map_base + target_addr;
#ifdef DEBUG
	printf("Write 32-bits value 0x%08x to 0x%08x (0x%p)\n",
			(unsigned int)writeval, (unsigned int)target_addr,
			virt_addr);
#endif
	/* swap 32-bit endianess if host is not little-endian */
	writeval = htoll(writeval);
	*((uint32_t *) virt_addr) = writeval;
	fflush(stdout);
}

int main(int argc, char **argv)
{
	int fd;
	uint32_t read_result;
	uint32_t writeval = 0x00000000;
	off_t target_addr = 0x0000002c;
	char *device = "/dev/xdma0_user"; // xdma*_user : DMA/Bridge IPのaxi4-liteマスターに対応するデバイスノード
#ifdef DEBUG
	printf("device: %s\n", device);
	printf("address: 0x%08x\n", (unsigned int)target_addr);
#endif

	if ((fd = open(device, O_RDWR | O_SYNC)) == -1) {
		FATAL;
	}
#ifdef DEBUG
	printf("character device %s opened.\n", device);
	fflush(stdout);
#endif

	/* map one page */
	map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (map_base == (void *)-1) {
		FATAL;
	}
#ifdef DEBUG
	printf("Memory mapped at address %p.\n", map_base);
	fflush(stdout);
#endif

	uint32_t counter1 = read_from_device(target_addr);
	uint32_t counter2 = read_from_device(target_addr);
	int counter_diff = counter2 - counter1;

	printf("Counter diff:%d = %.2f ns\n", counter_diff, (double)counter_diff/250);

	if (munmap(map_base, MAP_SIZE) == -1) {
		FATAL;
	}
	close(fd);
	return 0;
}
