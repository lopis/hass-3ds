#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

int main(int argc, char *argv[])
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	// printf("Welcome to Home Assistant for 3DS!\n");
	// printf("The default address of your instance is\n");
	// printf(" 192.168.178.39:8123");
	// printf("Is this correct? (Y/n)");

	// char answer[1];
	// fgets(answer, sizeof(answer), stdin);
	// if (strcmp(answer, "y") * strcmp(answer, "Y") * strcmp(answer, "") == 0) {
	// 	printf("Using default address.");
	// 	printf("Loading camera to copy QR code access token.");
	// }

	// printf("Loading camera to copy QR code access token.");
	// if (R_SUCCEEDED(camInit()))
	// {
	// 	init_qr();
	// }
	// else
	// {
	// 	printf("Your camera failed to init");
	// }

	toggleLights();

	printf("Press START to exit");
	// Main loop
	while (aptMainLoop())
	{
		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();

		// Your code goes here
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
	}

	gfxExit();
	return 0;
}