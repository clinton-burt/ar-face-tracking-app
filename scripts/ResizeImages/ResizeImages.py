import sys
import os
from PIL import Image
from PIL import ImageDraw
import msvcrt

input = './input/'
output = './output/'
sizeFactor = 0.1

def resizeImages():
  folderList = os.listdir(input);
  for name in folderList :
    fullName = input + name
    if os.path.isfile(fullName) and os.path.splitext(fullName)[1] == '.png' :
      img = Image.open(fullName)
      img = img.convert('RGBA')
      img = img.resize((int(img.size[0] * sizeFactor), int(img.size[1] * sizeFactor)), Image.ANTIALIAS)
      print(fullName + '   ' + str(img.size[0]) + 'x' + str(img.size[1]))
      img.save(output + name)

resizeImages()
print('Done. Press any key...')
msvcrt.getch()