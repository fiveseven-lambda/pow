#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdio.h>

double (*make_pow(int n))(double, double){
	int fd = open("/dev/zero", O_RDONLY);
	const char data1[] = {0x55, 0x48, 0x89, 0xe5, 0xf2, 0x0f, 0x11, 0x45, 0xf8, 0xf2, 0x0f, 0x11, 0x4d, 0xf0}, data2[] = {0xf2, 0x0f, 0x10, 0x45, 0, 0xf2, 0x0f, 0x59, 0x45, 0xf0, 0xf2, 0x0f, 0x11, 0x45, 0}, data3[] = {0xf2, 0x0f, 0x10, 0x45, 0xf8, 0x5d, 0xc3};
	int mapsize = 6;
	for(int i = n; i; i >>= 1) mapsize += (i & 1 ? 30 : 15);
	char *map = mmap(NULL, mapsize, PROT_WRITE | PROT_EXEC, MAP_PRIVATE, fd, 0);
	memcpy(map, data1, sizeof data1);
	int cursor = sizeof data1;
	if(n & 1){
		memcpy(map + cursor, data2, sizeof data2);
		map[cursor + 4] = map[cursor + 14] = 0xf8;
		cursor += sizeof data2;
	}
	n >>= 1;
	while(n){
		memcpy(map + cursor, data2, sizeof data2);
		map[cursor + 4] = map[cursor + 14] = 0xf0;
		cursor += sizeof data2;
		if(n & 1){
			memcpy(map + cursor, data2, sizeof data2);
			map[cursor + 4] = map[cursor + 14] = 0xf8;
			cursor += sizeof data2;
		}
		n >>= 1;
	}
	memcpy(map + cursor, data3, sizeof data3);
	return (double(*)(double, double))map;
}

int main(int argc, char *argv[]){
	int n;
	if(argc > 1) sscanf(argv[1], "%d", &n);
	else scanf("%d", &n);
	double (*fnc)(double, double) = make_pow(n);
	double a;
	scanf("%lf", &a);
	printf("%f\n", fnc(1, a));
}
