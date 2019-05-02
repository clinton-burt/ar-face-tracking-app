import sys
import os
from PIL import Image
from PIL import ImageDraw
import msvcrt

# Configurations you need to set. Only use borders if you ever do filtering on your sprites. And then you should
# probably set it to transparent or duplicate the edges depending on your use case. Using solid red now to
# make it clear they're there.

folder = './input/'
atlasBaseName = './output/output'

GL_MAX_TEXTURE_SIZE = 16384

frameW = 0
frameH = 0
atlasW = 0
atlasH = 0
rowSize = 0
rowCount = 0

images = []

def writeAtlas(images, atlasW, atlasH):
  atlasImg = Image.new('RGBA', [atlasW, atlasH])
  xShift = 0
  yShift = 0
  for i in images:
    atlasImg.paste(i, [xShift, yShift, xShift + frameW, yShift + frameH])
    xShift += frameW
    if xShift >= atlasW:
      xShift = 0
      yShift += frameH
  atlasImg.save(atlasBaseName + '.png')
  atlasImg = None

def writeJSON(images):
  json = open(atlasBaseName + '.json', 'w')
  json.write('{\n  "count" : ' + str(len(images)) + ',\n  "imagesSize" : [' + str(frameW) + ', ' + str(frameH) + '],\n  '  + '"images" : {\n');
  xShift = 0
  yShift = 0
  index = 0
  for i in images:
    json.write('    "' + str(index) + '" : {\n')
    json.write('      "shift": [' + str(xShift) +', ' + str(yShift) + ']\n    }')
    index += 1
    xShift += frameW
    if xShift >= atlasW:
      xShift = 0
      yShift += frameH
    if index < len(images):
      json.write(',')
    json.write('\n')
  json.write('  }\n}')
  json.close()

def loadFolder(folder):
  index = 0
  fullName = folder + str(index) + '.png'
  extension = os.path.splitext(fullName)[1]
  while os.path.isfile(fullName) and extension == '.png' :
    img = Image.open(fullName)
    img = img.convert('RGBA')
    #img = img.resize((int(img.size[0] * sizeFactor), int(img.size[1] * sizeFactor)), Image.ANTIALIAS)
    print(fullName + '   ' + str(img.size[0]) + 'x' + str(img.size[1]))
    images.append(img)
    index += 1
    fullName = folder + str(index) + '.png'
  global frameW
  global frameH
  frameW = images[0].size[0]
  frameH = images[0].size[1]

def calcAtlasSize(images):
  global rowSize
  global rowCount
  global atlasW
  global atlasH
  totalCount = len(images)
  rowSize = len(images)
  rowCount = 1
  atlasW = frameW * rowSize
  atlasH = frameH * rowCount
  while atlasW > atlasH :
    print(str(atlasW) + 'x' + str(atlasH) + ' (' + str(rowCount) + 'x' + str(rowSize) + ' frames)')
    rowCount += 1
    rowSize = totalCount // rowCount + (1 if totalCount // rowCount != 0 else 0)
    atlasW = frameW * rowSize
    atlasH = frameH * rowCount
  rowCount -= 1
  rowSize = totalCount // rowCount + (1 if totalCount // rowCount != 0 else 0)
  atlasW = frameW * rowSize
  atlasH = frameH * rowCount
  if atlasW > GL_MAX_TEXTURE_SIZE or atlasH > GL_MAX_TEXTURE_SIZE:
    print('Error: GL_MAX_TEXTURE_SIZE limit violated')
    print('Press any key...')
    msvcrt.getch()

print('Scaning...')
loadFolder(folder)
print('Frame size: ' + str(frameW) + 'x' + str(frameH))
print('Computing...')
calcAtlasSize(images)
print('Atlas size: ' + str(atlasW) + 'x' + str(atlasH) + ' (' + str(rowCount) + 'x' + str(rowSize) + ' frames)')
print('Outputing... Dont close the window')
writeAtlas(images, atlasW, atlasH)
writeJSON(images)
print('Done. Press any key...')
msvcrt.getch()