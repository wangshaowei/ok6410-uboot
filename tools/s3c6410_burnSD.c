#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define BYTES_PER_BLOCK 512
#define SDHC_SIZE 2*1024*1024*1024L

static int is_SDHC;
static off_t BL1_offset;
static off_t BL2_offset;

static int 
test_SD(char *path)
{
	if (NULL == path) {
		return -1;
	}

	struct stat sb;
	if (stat(path, &sb) < 0) {
		perror("can not stat\n");
		return -1;
	}
	if (!S_ISBLK(sb.st_mode)) {
		printf("%s is not block device\n", path);
		return -1;
	}

	int sd_fd = open(path, O_RDONLY);
	if (sd_fd < 0) {
		printf("can not open: %s\n", path);
		return -1;
	}
	long SD_size = lseek(sd_fd, 0, SEEK_END);
	if (SD_size < 0) {
		perror("can not lseek\n");
		return -1;
	}
	if (SD_size > SDHC_SIZE) {
		is_SDHC = 1;
		printf("is SDHC\n");
		BL1_offset = SD_size - 1042 * BYTES_PER_BLOCK;
		BL2_offset = SD_size - 1042 * BYTES_PER_BLOCK - 256 * 1024 - 0x80000;
	} else {
		is_SDHC = 0;
		printf("is SD\n");
		BL1_offset = SD_size - 18 * BYTES_PER_BLOCK;
		BL2_offset = SD_size - 18 * BYTES_PER_BLOCK - 256 * 1024 - 0x80000;
	}
	close(sd_fd);
	return 0;
}



/*
  burnSD /dev/sdb BL1
 */
int
main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("%s /dev/sdb BL1\n", argv[0]);
		exit(-1);
	}
	char *SD_dev = argv[1];
	char *BL1 = argv[2];

	if (test_SD(SD_dev) < 0) {
		printf("SD file: %s error\n", SD_dev);
		exit(-1);
	}

	int sd_fd = open(SD_dev, O_RDWR);
	if (sd_fd < 0) {
		printf("can not open: %s\n", SD_dev);
		exit(-1);
	}
	int bl1_fd = open(BL1, O_RDONLY);
	if (bl1_fd < 0) {
		printf("can not open: %s\n", BL1);
		exit(-1);
	}

	char *mem_buf = malloc(16 * BYTES_PER_BLOCK);
	if (!mem_buf) {
		printf("can not alloc memory");
		exit(-1);
	}
	if (read(bl1_fd, mem_buf, 16 * BYTES_PER_BLOCK) < 0) {
		printf("read error");
		exit(-1);
	}

	if (lseek(sd_fd, BL1_offset, SEEK_SET) < 0) {
		perror("seek error\n");
		exit(-1);
	}
	printf("BL1 offset: %ld\n", BL1_offset);
	if (write(sd_fd, mem_buf, 16 * BYTES_PER_BLOCK) < 0) {
		perror("write error\n");
		exit(-1);
	}


	free(mem_buf);
	mem_buf = malloc(256 * 1024);
	if (!mem_buf) {
		printf("can not alloc memory");
		exit(-1);
	}
	if (lseek(bl1_fd, 0, SEEK_SET) < 0) {
		perror("bl1 can not seek\n");
		exit(-1);
	}
	if (read(bl1_fd, mem_buf, 256 * 1024) < 0) {
		printf("read error");
		exit(-1);
	}
	if (lseek(sd_fd, BL2_offset, SEEK_SET) < 0) {
		perror("seek error\n");
		exit(-1);
	}
	printf("BL2 offset: %ld\n", BL2_offset);
	if (write(sd_fd, mem_buf, 256 * 1024) < 0) {
		perror("write error\n");
		exit(-1);
	}
	close(sd_fd);
	close(bl1_fd);
	sync();
	return 0;

}






















