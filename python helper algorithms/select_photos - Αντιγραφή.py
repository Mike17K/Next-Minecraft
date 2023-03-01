import os 
import pygame
pygame.init()

background_colour = (255,255,255)
(width, height) = (300, 200)
screen = pygame.display.set_mode((width, height))
pygame.display.set_caption('Tutorial 1')

textures = os.listdir("./blocks")
add = [False for i in range(len(textures))]
 
font = pygame.font.Font('freesansbold.ttf', 14)
index = 0

running = True
while running:
    screen.fill(background_colour)
    text = font.render(f"{index+1} / {len(textures)}", True, background_colour, (1,0,0))
    textRect = text.get_rect()
    textRect.center = (textRect[2]/2,textRect[3]/2)
    screen.blit(text, textRect)

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        
    keys = pygame.key.get_pressed()

    if keys[pygame.K_LEFT]:
        index = max( index -1 ,0)
        while pygame.key.get_pressed()[pygame.K_LEFT]:
            pygame.event.get()

    if keys[pygame.K_RIGHT]:
        index = min(index+1,len(textures)-1)
        while pygame.key.get_pressed()[pygame.K_RIGHT]:
            pygame.event.get()

    if keys[pygame.K_SPACE]:
        add[index] = not add[index]
        while pygame.key.get_pressed()[pygame.K_SPACE]:
            pygame.event.get()



    pygame.display.flip()
    
for i in range(len(textures)):
    if add[i]:
        os.system(f"copy {textures[i]} C:/Users/User/Desktop/projects/cpp/minecraft-project/minecraft/textures/selected_textures")
pygame.quit()