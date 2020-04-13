import pygame
from OpenGL.GL import *
from OpenGL.GLU import *
import DevIL

pygame.init()
screen = pygame.display.set_mode((800, 600), pygame.OPENGL | pygame.DOUBLEBUF)
pygame.display.set_caption('Devil Test')

glDisable(GL_DEPTH_TEST)
glMatrixMode(GL_PROJECTION)
glLoadIdentity()
gluOrtho2D(0, 800, 0, 600)
glMatrixMode(GL_MODELVIEW)
glLoadIdentity()
glEnable(GL_TEXTURE_2D)

DevIL.ilInit()
DevIL.ilutRenderer(DevIL.ILUT_OPENGL)
texture = DevIL.ilutGLLoadImage('devil.gif')
glEnable(GL_TEXTURE_2D)
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
glBindTexture(GL_TEXTURE_2D, texture)

while 1:
    event = pygame.event.poll()
    if event.type == pygame.QUIT:
        break

    glClear(GL_COLOR_BUFFER_BIT)

    glBegin(GL_QUADS)
    glTexCoord2f(0, 0); glVertex2f(-400, -400); 
    glTexCoord2f(5, 0); glVertex2f(1200, -400)
    glTexCoord2f(5, 5); glVertex2f(1200, 1000)
    glTexCoord2f(0, 5); glVertex2f(-400, 1000)
    glEnd()

    glTranslatef(400, 300, 0)
    glRotatef(0.5, 0, 0, 1)
    glTranslatef(-400, -300, 0)    

    pygame.display.flip()
