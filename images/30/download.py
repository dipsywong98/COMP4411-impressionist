from requests import get
from PIL import Image
import os

imageSize = 30
numberOfImages = 5000
urlFormat = "https://picsum.photos/%d?image=%d"

def download(url, fileName):
    with open(fileName, "wb") as file:
        print("Downloading: %s" % url)
        r = get(url)
        file.write(r.content)

for i in range(1, numberOfImages):
    jpgName = "%d.jpg" % i
    bmpName = "%d.bmp" % i
    
    if os.path.isfile(bmpName): continue

    download(urlFormat % (imageSize, i), jpgName)

    if os.path.isfile(jpgName):
        try:
            Image.open(jpgName).save(bmpName)
        except:
            pass
        finally:
            if os.path.isfile(jpgName):
                os.remove(jpgName)

