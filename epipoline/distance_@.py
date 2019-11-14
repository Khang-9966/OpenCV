import cv2
import numpy as np
import pickle
import matplotlib.pyplot as plt
from sklearn.preprocessing import normalize
from itertools import combinations 
from sklearn.linear_model import LinearRegression


f = open('L.pckl', 'rb')
Left_Stereo_Map = pickle.load(f)
f.close()
f = open('R.pckl', 'rb')
Right_Stereo_Map = pickle.load(f)
f.close()
f = open('FundamentalMat.pckl', 'rb')
F = pickle.load(f)
f.close()
f = open('FundamentalMat_mask.pckl', 'rb')
mask = pickle.load(f)
f.close()


def nothing(x):
    pass
# Create a black image, a window
cv2.namedWindow('image')
# create trackbars for color change
cv2.createTrackbar('para1L','image',200,255,nothing)
cv2.createTrackbar('para2L','image',115,255,nothing)
cv2.createTrackbar('para1R','image',200,255,nothing)
cv2.createTrackbar('para2R','image',115,255,nothing)

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
                cv2.circle(self.img, center, 3, ( 255, 0, 0), 3)
                # circle outline
                radius = i[2]
                cv2.circle(self.img, center, radius, (0, 0, 255), 8) 
                cv2.putText(self.img, str(center) , center, cv2.FONT_HERSHEY_SIMPLEX, 0.4, (255,0,0),1)
                return self.img
        else:
            #print(' No circle ! ')
            return self.img
    def para_update(self,param1_circle,param2_canny):
        self.para1_circle = param1_circle
        self.para2_canny = param2_canny

def caculate_distance(f,baseline,XL,XR):
    global count
    global dist_list
    if XR is not None and XL is not None : 
        if XL.shape[1] == 1 and XR.shape[1] == 1 :
            count = count + 1
            # X_center = imageshape[1]/2
            # XL =  center_circle_L[0,0,0] - X_center
            # XR =  X_center - center_circle_R[0,0,0]
            # print('XL: ', XL[0,0,0])
            # print('XR: ', XR[0,0,0])
            #print('X center: ',X_center)
            dist = baseline*f/(XL[0,0,0]-XR[0,0,0])
            dist_list.append(dist)
            if count == 10 :
                count = 0
                average_dist = (sum(dist_list) / len(dist_list))
                print('----------------------------------')
                print("Pixel: ",average_dist)
                average_dist = 6.16922869 + average_dist*07.55723716e-02 + average_dist**2*5.36279911e-05 \
                + average_dist**3*-3.12188045e-08 + average_dist**4*2.20282126e-12 
                print("Distance(cm): " ,average_dist)
                dist_list = []
                return average_dist
    else:
        return 0


def line_estimation(X):
    thred = 0.98
    comb = combinations(X.T, 3) 
    reg_list = []
    # Print the obtained combinations 
    for i in list(comb): 
        test = np.array(i).T
        #print(i) 
        cor = np.corrcoef(test)
        #print(cor)
        if cor[0,1] >= thred or cor[0,1] <= -thred :
        print('Points set: \n\r',test.T)
        print('regression ------------------------------------------------------>')
        x_t = test.T[:,0].reshape(3,1)
        y_t = test.T[:,1].reshape(3,1)
        no_new_line = True
        if len(reg_list) != 0 :
            for reg in reg_list:
            y_check = reg.predict(x_t)
            print('Check score: ',sum( abs(y_t - y_check)))
            if sum( abs(y_t - y_check))  < 300:
                no_new_line = False  
                print(' No new line detected !')
                
            if no_new_line:
            print(' Detected new line !')    
            reg = LinearRegression().fit(x_t,y_t)
            x_draw = np.linspace(0, 300, 100)
            y_draw = reg.predict(x_draw.reshape(100,1))
            plt.plot( x_draw, y_draw, color='red', linewidth=1)
            reg_list.append(reg)
        else:
            reg = LinearRegression().fit(x_t,y_t)
            x_draw = np.linspace(0, 300, 100)
            y_draw = reg.predict(x_draw.reshape(100,1))
            plt.plot( x_draw, y_draw, color='red', linewidth=1)
            reg_list.append(reg)
        print('=================================================================XX')

    print(' \n ___________________________SUMMARY____________________________  \n ')
    print('                     Total lines detected: ', len(reg_list))
    plt.scatter(x,y)
    plt.show()  # f = open('L_proto2.pckl', 'wb')
# pickle.dump(Left_Stereo_Map, f ,protocol=2)
# f.close()  
    


dist_list = []
count = 0
CamR= cv2.VideoCapture(2)   # Wenn 0 then Right Cam and wenn 2 Left Cam
CamL= cv2.VideoCapture(1)
retR, frameR= CamR.read()
retL, frameL= CamL.read()
imgL_ob = circle_detect(frameL,200,115)
imgR_ob = circle_detect(frameR,200,115)
dist_temp = None
Kinv = np.matrix([[861.7849547322785, 0, 320.0], 
                           [0, 848.6931340450212, 240.0],
                           [0, 0, 1]]).I # K inverse
worldPosL = None
worldPosR = None
while True:
    retR, frameR= CamR.read()
    retL, frameL= CamL.read()
    Left_nice= cv2.remap(frameL,Left_Stereo_Map[0],Left_Stereo_Map[1], cv2.INTER_LANCZOS4, cv2.BORDER_CONSTANT, 0)  # Rectify the image using the kalibration parameters founds during the initialisation
    Right_nice= cv2.remap(frameR,Right_Stereo_Map[0],Right_Stereo_Map[1], cv2.INTER_LANCZOS4, cv2.BORDER_CONSTANT, 0)
    imgL_ob.update(Left_nice)
    imgR_ob.update(Right_nice)
    para1L = cv2.getTrackbarPos('para1L','image')
    para2L = cv2.getTrackbarPos('para2L','image')
    para1R = cv2.getTrackbarPos('para1R','image')
    para2R = cv2.getTrackbarPos('para2R','image')
    imgL_ob.para_update(para1L,para2L)
    imgR_ob.para_update(para1R,para2R)
    #imgL = cv2.imread('left_3.jpg')  #queryimage # left image
    #imgR = cv2.imread('right_3.jpg') #trainimage # right image
    # imgL_ob = circle_detect(frameL,10,80)
    result_L = imgL_ob.draw()
    # imgR_ob = circle_detect(frameR,10,80)
    result_R = imgR_ob.draw()
    
    dist = caculate_distance(820,120,imgL_ob.circles,imgR_ob.circles)
    if dist is not None:
        dist_temp = round(dist,2)    
    #print('dist= ', dist)
    if result_L is not None and result_R is not None :
        if imgL_ob.circles is not None and imgR_ob.circles is not None and dist_temp != 0 and dist_temp is not None:
            worldPosL = Kinv * np.matrix([imgL_ob.circles[0,0,0],imgL_ob.circles[0,0,1],1]).T * dist_temp
            worldPosR = Kinv * np.matrix([imgR_ob.circles[0,0,0],imgR_ob.circles[0,0,1],1]).T * dist_temp
            cv2.putText(result_L, str(worldPosL) +" cm", (imgL_ob.circles[0,0,0]-30,imgL_ob.circles[0,0,1]+15), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0,0,255),1)
            cv2.putText(result_R, str(worldPosR) +" cm", (imgR_ob.circles[0,0,0]-30,imgR_ob.circles[0,0,1]+15), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0,0,255),1)
        cv2.imshow("detected circles Left", result_L)
        cv2.imshow("detected circles Right", result_R)
    if cv2.waitKey(1) & 0xFF == ord(' '):
        break

imgL_ob.circles