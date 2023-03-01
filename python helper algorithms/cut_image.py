import os
import json
import PIL.Image
import numpy as np

IMAGE_NAME = 'Grass_Block_TEX.jpg'

OUTPUT_IMAGE_SIZE_X = 16
OUTPUT_IMAGE_SIZE_Y = 16

NUMBER_ITEMS_X = 3
NUMBER_ITEMS_Y = 4

SIZE_OF_ORIGINAL_IMG_X = 1440 #pixels
SIZE_OF_ORIGINAL_IMG_Y = 1920 #pixels

SIZE_OF_PIXEL_X = SIZE_OF_ORIGINAL_IMG_X//NUMBER_ITEMS_X//OUTPUT_IMAGE_SIZE_X
SIZE_OF_PIXEL_Y = SIZE_OF_ORIGINAL_IMG_Y//NUMBER_ITEMS_Y//OUTPUT_IMAGE_SIZE_Y


total_img = np.zeros((OUTPUT_IMAGE_SIZE_X,OUTPUT_IMAGE_SIZE_Y,4),dtype=np.uint8)


img = PIL.Image.open(IMAGE_NAME)

img_np = np.asarray(img)

for i,x in enumerate(np.arange(SIZE_OF_ORIGINAL_IMG_X//NUMBER_ITEMS_X,2*SIZE_OF_ORIGINAL_IMG_X//NUMBER_ITEMS_X,SIZE_OF_PIXEL_X)):
    for j,y in enumerate(np.arange(3*SIZE_OF_ORIGINAL_IMG_Y//NUMBER_ITEMS_Y,4*SIZE_OF_ORIGINAL_IMG_Y//NUMBER_ITEMS_Y,SIZE_OF_PIXEL_Y)):
        total_img[j,i] = [*img_np[y,x],255]
                
outimg = PIL.Image.fromarray(total_img,'RGBA')
outimg.show()

outimg.save('dirt.png')

