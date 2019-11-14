import cv2
import numpy as np
class circle_detect:
    def __init__(self,img,param1_circle,param2_canny):
        self.img = img
        self.para1_circle = param1_circle
        self.para2_canny = param2_canny
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        gray = cv2.medianBlur(gray, 3)
        rows = gray.shape[0]
        self.circles = cv2.HoughCircles(gray, cv2.HOUGH_GRADIENT, 1, rows / 8,
                                param1=self.para1_circle, param2=self.para2_canny,
                                minRadius=0, maxRadius=0)
    def update(self,img):
        self.img = img
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        gray = cv2.medianBlur(gray, 3)
        rows = gray.shape[0]
        self.circles = cv2.HoughCircles(gray, cv2.HOUGH_GRADIENT, 1, rows / 8,
                                param1=self.para1_circle, param2=self.para2_canny,
                                minRadius=0, maxRadius=0)
    def draw(self):
        if self.circles is not None:
            self.circles = np.uint16(np.around(self.circles))
            for i in self.circles[0, :]:
                center = (i[0], i[1])
                # circle center
                cv2.circle(self.img, center, 1, (0, 100, 100), 3)
                # circle outline
                radius = i[2]
                cv2.circle(self.img, center, radius, (0, 0, 255), 8) 
                cv2.putText(self.img, str(center) , center, cv2.FONT_HERSHEY_SIMPLEX, 0.4, (255,0,0),1)
                return self.img
        else:
            #print(' No circle ! ')
            return self.img

def caculate_distance(f,baseline,center_circle_L,center_circle_R,imageshape):
    if center_circle_R is not None and center_circle_L is not None : 
        if center_circle_R.shape[1] == 1 and center_circle_L.shape[1] == 1 :
            X_center = imageshape[1]/2
            XL =  center_circle_L[0,0,0] - X_center
            XR =  X_center - center_circle_R[0,0,0]
            print('XL: ', XL )
            print('XR: ', XR)
            print('X center: ',X_center)
            return baseline*f/(XR-XL)
    else:
        return 0





imgL = cv2.imread('left_2.jpg')  #queryimage # left image
imgR = cv2.imread('right_2.jpg') #trainimage # right image
imgL_ob = circle_detect(imgL,30,80)
result_L = imgL_ob.draw()
imgR_ob = circle_detect(imgR,30,80)
result_R = imgR_ob.draw()

dist = caculate_distance(214,120,imgL_ob.circles,imgR_ob.circles,imgL.shape)

print('dist', dist)
if result_L is not None and result_R is not None:
    cv2.imshow("detected circles Left", result_L)
    cv2.imshow("detected circles Right", result_R)
cv2.waitKey(0)

