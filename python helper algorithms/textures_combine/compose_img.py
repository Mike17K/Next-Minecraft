import os
import json
import PIL.Image
import numpy as np

DIR_PATH = 'C:/Users/User/Desktop/projects/cpp/minecraft-project/minecraft/textures/selected_textures/python-target-make-totalmesh/'

OUTPUT_IMAGE_SIZE_X = 4096
OUTPUT_IMAGE_SIZE_Y = 4096

NUMBER_ITEMS_X = 16
NUMBER_ITEMS_Y = 16

SIZE_OF_ORIGINAL_IMG_X = 16 #pixels
SIZE_OF_ORIGINAL_IMG_Y = 16 #pixels

SIZE_OF_IMG_X = OUTPUT_IMAGE_SIZE_X//NUMBER_ITEMS_X
SIZE_OF_IMG_Y = OUTPUT_IMAGE_SIZE_Y//NUMBER_ITEMS_Y

SIZE_OF_PIXEL_X = SIZE_OF_IMG_X//SIZE_OF_ORIGINAL_IMG_X
SIZE_OF_PIXEL_Y = SIZE_OF_IMG_Y//SIZE_OF_ORIGINAL_IMG_Y


f = open('textures_order.json')
  
data = json.load(f)

total_img = np.zeros((OUTPUT_IMAGE_SIZE_X,OUTPUT_IMAGE_SIZE_Y,4),dtype=np.uint8)
print(total_img.shape)

for index_y,i in enumerate(data.values()):
    if i != '':
        for index_x,j in enumerate(i.values()):
            #print(j,end=' ')
            if j != '':
                img = PIL.Image.open(DIR_PATH+j)
                img_np = np.asarray(img)
                if img_np.shape[2] == 4:
                    #this is the correct img shape
                    print(j)
                    for u in range(SIZE_OF_IMG_X):
                        for v in range(SIZE_OF_IMG_Y):
                            tmp = []
                            for d in range(4):
                                tmp.append(img_np[u//SIZE_OF_PIXEL_X,v//SIZE_OF_PIXEL_Y,d])
                            total_img[index_y*SIZE_OF_IMG_Y+u,index_x*SIZE_OF_IMG_X+v] = tmp
                elif img_np.shape[2] == 2:
                    #handle grayscale imgs
                    for u in range(SIZE_OF_IMG_X):
                        for v in range(SIZE_OF_IMG_Y):
                            total_img[index_y*SIZE_OF_IMG_Y+u,index_x*SIZE_OF_IMG_X+v] = [img_np[u//SIZE_OF_PIXEL_X,v//SIZE_OF_PIXEL_Y,0],img_np[u//SIZE_OF_PIXEL_X,v//SIZE_OF_PIXEL_Y,0],img_np[u//SIZE_OF_PIXEL_X,v//SIZE_OF_PIXEL_Y,0],img_np[u//SIZE_OF_PIXEL_X,v//SIZE_OF_PIXEL_Y,1]]
                elif img_np.shape[2] == 3:
                    #handle grayscale imgs
                    for u in range(SIZE_OF_IMG_X):
                        for v in range(SIZE_OF_IMG_Y):
                            total_img[index_y*SIZE_OF_IMG_Y+u,index_x*SIZE_OF_IMG_X+v] = [img_np[u//SIZE_OF_PIXEL_X,v//SIZE_OF_PIXEL_Y,0],img_np[u//SIZE_OF_PIXEL_X,v//SIZE_OF_PIXEL_Y,1],img_np[u//SIZE_OF_PIXEL_X,v//SIZE_OF_PIXEL_Y,2],255]
                else:
                    print(f"Error with img: {j} shape {img_np.shape}")
                    continue

total_img = total_img[:,:,:3]
print(total_img.shape)
outimg = PIL.Image.fromarray(total_img,'RGB')
outimg.show()

outimg.save('outimg.bmp')

