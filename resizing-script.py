import os, sys
import cv2
import numpy as np
from PIL import Image

size = 640, 480

# most from PIL documentation
directory = "resized"
try:
    os.stat(directory)
except:
    os.mkdir(directory)
    
for infile in os.listdir(os.getcwd()):
    outfile = os.getcwd()+"\\resized\\"+os.path.splitext(infile)[0] + ".png"
    if infile != outfile:
        try:
            im = Image.open(os.getcwd()+"\\"+infile)
            im.thumbnail(size, Image.ANTIALIAS)
            img = np.array(im)
            
            img = img.astype(np.uint8)
            cv2.imwrite(outfile,cv2.cvtColor(np.array(img),cv2.COLOR_BGR2GRAY))
            print(infile+" resized")
        except IOError:
            print ("cannot create thumbnail for {}".format(infile))
