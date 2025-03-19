import sys, os, math
from io import TextIOWrapper
from PIL import Image
from PIL import ImageFile

def check_args_and_create_file(args):
    # checking program args

    if (len(args) != 1):
        print('Usage: python converter.py png_filepath\n')
        return False

    # opening PNG file

    png_filepath: str = args[0]
    image: ImageFile
    try:
        image = Image.open(png_filepath)
    except:
        print('ERROR: error openning file %s\n' % png_filepath)
        return False
    
    # checking image dimensions

    pow: float = math.log2(image.width)
    poh: float = math.log2(image.height)
    if image.width < 8 or image.height < 8 or int(pow) != pow or int(poh) != poh:
        print('ERROR: image width and height must at least 8 and a power of 2. Ex: 8, 16, 32, ...\n')
        image.close()
        return False
    
    # creating output binary file

    new_filepath: str = os.path.splitext(png_filepath)[0] + '.bin'
    try:
        file: TextIOWrapper = open(new_filepath, 'wb')
    except:
        print('ERROR: error trying to create file %s\n' % new_filepath)
        image.close()
        return False

    return True, image, file


def main():
    data: tuple = check_args_and_create_file(['animated_tiles.png'])#sys.argv[1:])
    if not data[0]:
        return

    image: ImageFile = data[1]
    file: TextIOWrapper = data[2]

    pixels = image.load()

    for pos_y in range(0, image.height, 8):
        for pos_x in range(0, image.width, 8):

            for y in range(pos_y, pos_y+8):
                for x in range(pos_x, pos_x+8, 2):
                    nibbles: int = (pixels[x,y] << 4) | pixels[x+1,y]
                    file.write(nibbles.to_bytes())
                    print('%02X ' % nibbles, end='')
                print()
            print()

    file.close()
    image.close()

if __name__ == '__main__':
    main()