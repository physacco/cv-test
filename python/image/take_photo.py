import sys
import cv2
 
camera = cv2.VideoCapture(0)
 
def get_image():
    ok, img = camera.read()
    if not ok:
        sys.stderr.write('Cannot get image from the camera\n')
        exit(1)
    return img
 
# Drop the first N frames because they're usually awful.
for i in range(32):
    get_image()

cimg = get_image()
camera.release()

cv2.imwrite("cimg.png", cimg, [cv2.IMWRITE_PNG_COMPRESSION, 9])
cv2.imwrite("cimg.jpg", cimg, [cv2.IMWRITE_JPEG_QUALITY, 50])
cv2.imwrite("cimg.webp", cimg, [cv2.IMWRITE_WEBP_QUALITY, 50])

gimg = cv2.cvtColor(cimg, cv2.COLOR_BGR2GRAY)

cv2.imwrite("gimg.png", gimg, [cv2.IMWRITE_PNG_COMPRESSION, 9])
cv2.imwrite("gimg.jpg", gimg, [cv2.IMWRITE_JPEG_QUALITY, 50])
cv2.imwrite("gimg.webp", gimg, [cv2.IMWRITE_WEBP_QUALITY, 50])
