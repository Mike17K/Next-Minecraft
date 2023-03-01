import os
import json
import PIL.Image
import numpy as np

f = open('textures_order.json')
  
data = json.load(f)

names_cords = dict()
for i,dictionary in data.items():
    for j,value in dictionary.items():
        names_cords[value] = [i,j]

with open("reverce_textures_names.json","w") as f:
    json.dump(names_cords,f)