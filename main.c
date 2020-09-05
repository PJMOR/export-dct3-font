#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

void main()
{
    FILE *flash;
    unsigned long lenFlash;

    flash = fopen("input", "rb");
    fseek(flash, 0, SEEK_END);
    lenFlash = ftell(flash);
    rewind(flash);

    //printf("\n%li bytes\n\n", lenFlash);

    unsigned char bytesFlash[lenFlash];
    fread(bytesFlash, lenFlash, 1, flash);
    fclose(flash);

    unsigned char fontChunkFind[9] = {0x46, 0x4F, 0x4E, 0x54, 0x66, 0x63, 0x6F, 0x6E, 0x76};
    unsigned long fontChunkStart = (unsigned long)memmem(bytesFlash, lenFlash, fontChunkFind, 9) - (unsigned long)&bytesFlash + 28;
    unsigned char fontSizeNum = bytesFlash[fontChunkStart + 3];

    printf("# font style/weights: %i\n\n", fontSizeNum);

    unsigned long fontCharsIndex[fontSizeNum][3];
    unsigned char fontSpaceBeforeWidth = 0;
    int i, j, k, l, m, n;

    for (i = 0; i < fontSizeNum; i++)
    {
        fontCharsIndex[i][0] = (bytesFlash[fontChunkStart + 14 + 44 * i] << 8 | bytesFlash[fontChunkStart + 15 + 44 * i]) + 4 + 44 * i;
        //printf("%li\n", fontCharsIndex[i][0]); //index offset for font chars index start
        fontCharsIndex[i][1] = ((bytesFlash[fontChunkStart + 18 + 44 * i] << 8 | bytesFlash[fontChunkStart + 19 + 44 * i]) + 1) * 8;
        //printf("%li\n", fontCharsIndex[i][1]); //length of block
        fontCharsIndex[i][2] = fontCharsIndex[i][0] + fontCharsIndex[i][1];
        //printf("%li\n", fontCharsIndex[i][2]); //start of next block
        fontSpaceBeforeWidth += bytesFlash[fontChunkStart + 17 + 44 * i] + 1;
        //printf("%i\n", fontSpaceBeforeWidth); //num of end chars? before next part starts
        //printf("\n");
    };
    char folderName[fontSizeNum][19];
    for (i = 0; i < fontSizeNum; i++)
    {
        k = 0;
        for (j = 0; j < 17; j++)
        {
            folderName[i][2 + j] = 0;

            if (bytesFlash[fontChunkStart + 28 + j + 44 * i] > 0)
            {
                folderName[i][2 + j - k] = bytesFlash[fontChunkStart + 28 + j + 44 * i];
            }
            else
            {
                k++;
            }
        }
        folderName[i][0] = 0x2E;
        folderName[i][1] = 0x2F;
        //printf("%s\n", folderName[i]);
        mkdir(folderName[i], 0777);
    }

    unsigned short count = 0;
    //printf("fontCharsIndex start: %li\n", fontCharsIndex[0][0]);
    printf("# char groups: %li\n\n", (fontCharsIndex[fontSizeNum - 1][2] - fontCharsIndex[0][0]) / 8);

    for (i = 0; i < (fontCharsIndex[fontSizeNum - 1][2] - fontCharsIndex[0][0]) / 8; i++)
    {
        count += ((bytesFlash[fontChunkStart + fontCharsIndex[0][0] + 2 + i * 8] << 8 | bytesFlash[fontChunkStart + fontCharsIndex[0][0] + 3 + i * 8]) - (bytesFlash[fontChunkStart + fontCharsIndex[0][0] + i * 8] << 8 | bytesFlash[fontChunkStart + fontCharsIndex[0][0] + 1 + i * 8]) + 1);
    }
    printf("# chars: %i\n\n", count);

    unsigned long fontWidthStart = fontChunkStart + fontCharsIndex[fontSizeNum - 1][2] + fontSpaceBeforeWidth * 8;
    unsigned char fontWidthNum = 0;

    while (1)
    {
        if (bytesFlash[fontWidthStart + 4 + fontWidthNum * 12] == 0 && bytesFlash[fontWidthStart + 5 + fontWidthNum * 12] == 1 && bytesFlash[fontWidthStart + 6 + fontWidthNum * 12] == 0)
        {
            fontWidthNum++;
        }
        else
        {
            break;
        }
    };
    printf("# font widths: %i\n\n", fontWidthNum);

    unsigned long fontWidthInfo[fontWidthNum][4];

    for (i = 0; i < fontWidthNum; i++)
    {
        fontWidthInfo[i][0] = (bytesFlash[fontWidthStart + 12 * i] << 24 | bytesFlash[fontWidthStart + 1 + 12 * i] << 16 | bytesFlash[fontWidthStart + 2 + 12 * i] << 8 | bytesFlash[fontWidthStart + 3 + 12 * i]) + 12 * i;
        //printf("%li\n", fontWidthInfo[i][0]); //mem offset from fontWidthStart
        fontWidthInfo[i][1] = bytesFlash[fontWidthStart + 7 + 12 * i];
        //printf("%lx\n", fontWidthInfo[i][1]); //font width
        fontWidthInfo[i][2] = bytesFlash[fontWidthStart + 8 + 12 * i] << 24 | bytesFlash[fontWidthStart + 9 + 12 * i] << 16 | bytesFlash[fontWidthStart + 10 + 12 * i] << 8 | bytesFlash[fontWidthStart + 11 + 12 * i];
        //printf("%lx\n", fontWidthInfo[i][2]); //bmp height
        fontWidthInfo[i][3] = (fontWidthInfo[i][1] * fontWidthInfo[i][2]) / 8;
        //printf("%li\n\n", fontWidthInfo[i][3]); //mem length
    };

    unsigned long currentNum;
    bool match;

    int firstCharNum[fontWidthNum];
    unsigned short numOfChars[fontWidthNum];
    unsigned short numPerWidth[fontWidthNum];

    unsigned int charLocation[fontWidthNum][count];
    unsigned short charName[fontWidthNum][count];
    unsigned char charWeight[fontWidthNum][count];
    unsigned char charHeight[fontWidthNum][count];
    unsigned char charNumPerFont[fontSizeNum];

    unsigned char height1;
    unsigned char height2;
    unsigned char height;

    for (i = 0; i < fontWidthNum; i++)
    {
        numPerWidth[i] = 0;
        numOfChars[i] = 0;
        firstCharNum[i] = -1;
    }
    int x = 0;
    for (x = 0; x < 2; x++)
    {
        for (i = 0; i < fontSizeNum; i++)
        {
            for (j = 0; j < (fontCharsIndex[i][2] - fontCharsIndex[i][0]) / 8; j++)
            {
                currentNum = (bytesFlash[fontChunkStart + fontCharsIndex[i][0] + 4 + j * 8] << 16 | bytesFlash[fontChunkStart + fontCharsIndex[i][0] + 5 + j * 8] << 8 | bytesFlash[fontChunkStart + fontCharsIndex[i][0] + 6 + j * 8]);
                if (currentNum % 2 == 0)
                {
                    if (currentNum % 4 == 0)
                    {
                        height1 = 0;
                    }
                    else
                    {
                        height1 = 2;
                    }
                }
                else
                {
                    if ((currentNum - 1) % 4 == 0)
                    {
                        height1 = 1;
                    }
                    else
                    {
                        height1 = 3;
                    }
                }
                //printf("%x\n", ((bytesFlash[fontChunkStart + fontCharsIndex[i][0] + 7 + j * 8] & 240) >> 4));
                if (((bytesFlash[fontChunkStart + fontCharsIndex[i][0] + 7 + j * 8] & 240) >> 4) % 2 == 0)
                {
                    height2 = (bytesFlash[fontChunkStart + fontCharsIndex[i][0] + 7 + j * 8] & 240) >> 4;
                }
                else
                {
                    height2 = ((bytesFlash[fontChunkStart + fontCharsIndex[i][0] + 7 + j * 8] & 240) >> 4) - 1;
                }
                height = ((height1 << 4) | height2) / 2;
                //printf("%i\n", height);
                for (k = 0; k < fontWidthNum; k++)
                {
                    match = false;
                    if (((currentNum - height1) / 4) + fontWidthInfo[0][1] == fontWidthInfo[k][1])
                    {
                        charLocation[k][0] = 0;
                        firstCharNum[k] = currentNum - height1;
                        match = true;
                    }
                    else if ((currentNum - height1 - firstCharNum[k]) % 64 == 0 && firstCharNum[k] >= 0)
                    {
                        charLocation[k][numPerWidth[k]] = (currentNum - height1 - firstCharNum[k]) / 64;
                        match = true;
                    }
                    if (match == true)
                    {
                        numOfChars[k] = ((bytesFlash[fontChunkStart + fontCharsIndex[i][0] + 2 + j * 8] << 8 | bytesFlash[fontChunkStart + fontCharsIndex[i][0] + 3 + j * 8]) - (bytesFlash[fontChunkStart + fontCharsIndex[i][0] + j * 8] << 8 | bytesFlash[fontChunkStart + fontCharsIndex[i][0] + 1 + j * 8]) + 1);
                        for (l = 0; l < numOfChars[k]; l++)
                        {
                            charName[k][numPerWidth[k] + l] = (bytesFlash[fontChunkStart + fontCharsIndex[i][0] + j * 8] << 8 | bytesFlash[fontChunkStart + fontCharsIndex[i][0] + 1 + j * 8]) + l;
                            charWeight[k][numPerWidth[k] + l] = i;
                            charHeight[k][numPerWidth[k] + l] = height;
                            if (firstCharNum[k] > 0)
                            {
                                charLocation[k][numPerWidth[k] + l] = charLocation[k][numPerWidth[k]] + l * height;
                                //printf("%i\n", charLocation[k][numPerWidth[k] + l]);
                            }
                        }
                        numPerWidth[k] += numOfChars[k];
                    }
                }
            }
            charNumPerFont[i] = j;
            //printf("%i\n", charNumPerFont[i]);
        }
    }

    FILE *output;

    char filename[50];
    //mkdir("./column", 0777);

    for (i = 0; i < fontWidthNum; i++)
    {
        int multiple = 0;
        unsigned char bitmapMatrix[fontWidthInfo[i][2]][fontWidthInfo[i][1]];

        //sprintf(filename, "./column/Font%lx.pbm", fontWidthInfo[i][1]);
        //output = fopen(filename, "wb");
        //fprintf(output, "P1\n %ld\n %ld\n", fontWidthInfo[i][1], fontWidthInfo[i][2]);

        for (j = 0; j < fontWidthInfo[i][3]; j += fontWidthInfo[i][1])
        {
            for (k = 0; k < 8; k++)
            {
                for (l = j; l < j + fontWidthInfo[i][1]; l++)
                {
                    int result = 1;
                    for (int exp = k; exp != 0; exp--)
                    {
                        result *= 2;
                    };
                    bitmapMatrix[k + multiple * 8][l - j] = (bytesFlash[fontWidthStart + fontWidthInfo[i][0] + l] & result) >> k;
                    //fprintf(output, "%i ", bitmapMatrix[k + multiple * 8][l - j]);
                };
            };
            multiple++;
        };
        //fclose(output);

        for (l = 0; l < numPerWidth[i]; l++)
        {
            sprintf(filename, "%s/%04x.pbm", folderName[charWeight[i][l]], charName[i][l]);
            output = fopen(filename, "wb");
            fprintf(output, "P1\n %ld\n %d\n", fontWidthInfo[i][1], charHeight[i][l]);
            for (m = 0; m < charHeight[i][l]; m++)
            {
                for (n = 0; n < fontWidthInfo[i][1]; n++)
                {
                    fprintf(output, "%i ", bitmapMatrix[charLocation[i][l] + m][n]);
                    //printf("%c ", bitmapMatrix[charLocation[i][l] + m][n] * 3 + 0x20);
                }
                //printf("\n");
            }
            fclose(output);
            //printf("%c%c_%04x, %i, %li, %i\n\n", charFont[i][l], charWeight[i][l], charName[i][l], charHeight[i][l], fontWidthInfo[i][1], charLocation[i][l]);
        }
    }
};
