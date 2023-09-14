#include "helpers.h"
void colorize(int height, int width, RGBTRIPLE image[height][width]) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			// find the black pixels that formed the smiley face pattern
			if (image[i][j].rgbtRed == 0x00 && image[i][j].rgbtGreen == 0x00 && image[i][j].rgbtBlue == 0x00) {
				
				// set the original black pixel to different colour
				image[i][j].rgbtRed = 0xff;
				image[i][j].rgbtGreen = 0x99;
				image[i][j].rgbtRed = 0xff;

			}
		}
	}
}


