from DevIL import *

ilInit()
image = ilGenImages(1)
ilBindImage(image)

ilLoadImage('devil.gif')

data = ilGetData()
for i in range(len(data)):
   if data[i] == 255:
      data[i] = 0
ilSetData(data)

ilSaveImage('test.tga')
