# Extract Nokia DCT3 Fonts

This is a simple program that takes a PPM file from a Wintesla flash package or a full Dejan flash file (.fls) of a DCT3 generation Nokia phone and exports .pbm bitmaps for each character in each font present in the firmware.

Place either the PPM or flash file in the same directory as the code, rename it to `input`, compile code with `gcc main.c`, and run with `./a.out`.

The code has been tested under Debian with most locales of 3310, 3330, 3410, and 8250 firmwares with no apparent issues. It should work with all DCT3 firmwares, and may also work with a decrypted DCT4 PPM.
