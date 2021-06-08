# gimp-plugins
Gimp C plugins
- speckle.c: spread speckles with the current brush/size/color in the current drawable. The size and brightness variation can be randomly varied. Useful for simulating starfields, snow etc.

Compile this with gimptool-2.0 as follows:

gimptool-2.0 --install speckle.c
or
sudo gimptool-2.0 --install-admin speckle.c

If gimptool-2.0 is missing, install it with:

sudo apt install libgimp2.0-dev

Note: this can add >180 MBytes to your installation, 'cause headers and compile tools will be installed if not yet present!
