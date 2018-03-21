from PIL import Image
from os.path import basename, join, dirname
import glob

images_to_convert = glob.glob("*.jpg")
for image_fname in images_to_convert:
    image = Image.open(image_fname)
    file_basename = basename(image_fname)
    file_basename = file_basename.split('.')[0] + ".ppm"
    image.save(join(dirname(image_fname), file_basename))

