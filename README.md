# Extract Nokia DCT3 Fonts

This is a simple program that takes a PPM file from a Wintesla flash package or a full Dejan flash file (.fls) of a DCT3 generation Nokia phone and exports .pbm bitmaps for each character in each font present in the firmware.

Place either the PPM or flash file in the same directory as the code, rename it to `input`, compile code with `gcc main.c`, and run with `./a.out`. The resulting bitmaps will be placed in folders named after their originating fonts, eg. `smallbold`.

This code should work with all DCT3 firmwares, and may also work with a decrypted DCT4 PPM.
So far, the following firmwares have been personally tested:

- 3310 (NHM-5)
	- v4.18, langpack B
	- v4.19, langpack C
	- v4.45, langpack J
	- v5.87, langpack B
	- v5.79, langpack N
	- v6.07, langpack C
	- v6.33, all langpacks
- 3330 (NHM-6)
	- v4.30, langpack B
	- v4.50, all langpacks
- 3350 (NHM-9)
	- v5.30, all langpacks
- 3410 (NHM-2)
	- v5.42, all langpacks
- 6210 (NPE-3)
	- v5.36, all langpacks
- 6250 (NHM-3)
	- v5.00, all langpacks
- 8250 (NSM-3D)
	- v5.02, langpack K
	- v6.04, all langpacks
