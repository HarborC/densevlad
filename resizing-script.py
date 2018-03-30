import os, sys
from PIL import Image

size = 640, 480

# most from PIL documentation
directory = "dataset\\resized"
try:
    os.stat(directory)
except:
    os.mkdir(directory)
    
for infile in os.listdir(os.getcwd()+"\\dataset"):
    outfile = os.getcwd()+"\\dataset\\resized\\"+os.path.splitext(infile)[0] + ".png"
    if infile != outfile:
        try:
            im = Image.open(os.getcwd()+"\\dataset\\"+infile)
            im.thumbnail(size, Image.ANTIALIAS)
            im.save(outfile, "png")
            print(infile+" resized")
        except IOError:
            print ("cannot create thumbnail for {}".format(infile))
