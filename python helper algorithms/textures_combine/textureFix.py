import os 
import PIL.Image
import numpy as np

FILE_PATH = "C:\\Users\\User\\Desktop\\projects\\cpp\\minecraft-project\\minecraft\\textures\\selected_textures\\important\\"
FILE_NAME = "grass_block_top.png"

TOTAL_PATH = FILE_PATH + FILE_NAME

total_img = np.zeros((16,16,4),dtype=np.uint8)

img = PIL.Image.open(TOTAL_PATH)
img_np = np.asarray(img)

print(img_np.shape)

color_mult = [0.5,0.8,0.5]

MAX = 0
for y in range(len(img_np)):
    for x in range(len(img_np[0])):
        tmp = np.array([img_np[y,x][0]*color_mult[0],img_np[y,x][0]*color_mult[1],img_np[y,x][0]*color_mult[2],img_np[y,x][1]])
        
        MAX = max(MAX,max(tmp[0:3]))
        
#if total_img.max()>255: 
print(MAX)

for y in range(len(img_np)):
    for x in range(len(img_np[0])):
        tmp = np.array([img_np[y,x][0]*color_mult[0],img_np[y,x][0]*color_mult[1],img_np[y,x][0]*color_mult[2],img_np[y,x][1]])
        
        total_img[y,x] = [tmp[0]/MAX*255,tmp[1]/MAX*255,tmp[2]/MAX*255,tmp[3]]


outimg = PIL.Image.fromarray(total_img,'RGBA')
outimg.show()
outimg.save(FILE_NAME)