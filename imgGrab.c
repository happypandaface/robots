#include <stdio.h>
#include <stdlib.h>
#define SIZE 307200
int main()
{
	FILE *camera, *grab;
	camera = fopen("/dev/video0", "rb");
	grab = fopen("grab.raw", "wb");
	float data[SIZE];
	fread(data, sizeof(data[0]), SIZE, camera);
	fwrite(data, sizeof(data[0]), SIZE, grab);
	fclose(camera);
	fclose(grab);
	return 0;
}
