To add a texture to the minecraft project

1. put the image .png file (16*16 pixels rgb or rgba or grayscale) to folder:
    'C:/Users/User/Desktop/projects/cpp/minecraft-project/minecraft/textures/selected_textures/python-target-make-totalmesh/'

2. put the name of the image.png to the textures_order.json file in the place u want to have it in this directory:
    'C:\Users\User\Desktop\projects\cpp\minecraft-project\python helper algorithms\textures_combine'

3. run the compose_img.py file from the above directory, this will generate the new output.bmp texture file 
    to use from cpp

4. to have access fast to the items inside run the reverce_textures_json.py file, that will generate the
    reverce_textures_names.json that contains the json data of the location of each image in the output.bmp (in index not in pixels, to have it in pixels u need to mult the index by 16)

5. online convert the bmp to jpg and then back to bmp (the header is wrong directry with python)