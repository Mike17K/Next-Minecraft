import os 

source_ppath = os.getcwd()
print(source_ppath)

for foldername in ['ores','others','plants','wood']:    
    folder_path = os.path.join(source_ppath,f"selected_textures\\{foldername}")
    files = os.listdir(folder_path)
    for i in files:
        print(i)
        res = i.strip("- Αντιγραφή - Αντιγραφή.png")
        if res+'.png'==i:
            continue
        filename = folder_path+'\\'+res+'.png'
        os.rename(folder_path+'\\'+i,filename)