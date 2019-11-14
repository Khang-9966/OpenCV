import numpy as np
import cv2
from openpyxl import Workbook # Used for writing data into an Excel file
from sklearn.preprocessing import normalize
import pickle
import matplotlib.pyplot as plt

wb=Workbook()
ws=wb.active  

#*************************************************
#***** Parameters for Distortion Calibration *****
#*************************************************

# Termination criteria
criteria =(cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
criteria_stereo= (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)

# Prepare object points
objp = np.zeros((9*6,3), np.float32)
objp[:,:2] = np.mgrid[0:9,0:6].T.reshape(-1,2)

# Arrays to store object points and image points from all images
objpoints= []   # 3d points in real world space
imgpointsR= []   # 2d points in image plane
imgpointsL= []
num_calib = 2
# Start calibration from the camera
print('Starting calibration for the 2 cameras... ')
# Call all saved images
for i in range(0,num_calib):   # Put the amount of pictures you have taken for the calibration inbetween range(0,?) wenn starting from the image number 0
    t= str(i)
    print('Calibrating image: '+t)
    ChessImaR= cv2.imread('./calibdata/chessboard-R'+t+'.png',0)    # Right side
    ChessImaL= cv2.imread('./calibdata/chessboard-L'+t+'.png',0)    # Left side
    retR, cornersR = cv2.findChessboardCorners(ChessImaR,
                                               (9,6),None)  # Define the number of chees corners we are looking for
    retL, cornersL = cv2.findChessboardCorners(ChessImaL,
                                               (9,6),None)  # Left side
    if (True == retR) & (True == retL):
        objpoints.append(objp)
        cv2.cornerSubPix(ChessImaR,cornersR,(11,11),(-1,-1),criteria)
        cv2.cornerSubPix(ChessImaL,cornersL,(11,11),(-1,-1),criteria)
        imgpointsR.append(cornersR)
        imgpointsL.append(cornersL)
    
        
print('############################')

pts1 = np.int32(np.array(imgpointsL).reshape(54*(num_calib-1),2))
pts2 = np.int32(np.array(imgpointsR).reshape(54*(num_calib-1),2))
F, mask = cv2.findFundamentalMat(pts1,pts2,cv2.FM_LMEDS)
print(F)
f = open('FundamentalMat.pckl', 'wb')
pickle.dump(F, f ,protocol=2)
f.close()
f = open('FundamentalMat_mask.pckl', 'wb')
pickle.dump(mask, f ,protocol=2)
f.close()
# We select only inlier points
pts1 = pts1[mask.ravel()==1]
pts2 = pts2[mask.ravel()==1]

def drawlines(img1,img2,lines,pts1,pts2):
    ''' img1 - image on which we draw the epilines for the points in img2
        lines - corresponding epilines '''
    r,c = img1.shape
    img1 = cv2.cvtColor(img1,cv2.COLOR_GRAY2BGR)
    img2 = cv2.cvtColor(img2,cv2.COLOR_GRAY2BGR)
    for r,pt1,pt2 in zip(lines,pts1,pts2):
        color = tuple(np.random.randint(0,255,3).tolist())
        x0,y0 = map(int, [0, -r[2]/r[1] ])
        print(x0,y0)
        x1,y1 = map(int, [c, -(r[2]+r[0]*c)/r[1] ])
    
        img1 = cv2.line(img1, (x0,y0), (x1,y1), color,1)
        img1 = cv2.circle(img1,tuple(pt1),5,color,-1)
        img2 = cv2.circle(img2,tuple(pt2),5,color,-1)
    return img1,img2
# Find epilines corresponding to points in right image (second image) and
# drawing its lines on left image
img2= ChessImaR.copy()    # Right side
img1= ChessImaL.copy()    # Left side
#print(cornersR.reshape((54,2))[0])
#test_point = np.array([np.array([ 300, 110 ])])
#lines1 = cv2.computeCorrespondEpilines(test_point , 2,F)

#cv2.imshow('ss',cv2.circle(img2,tuple(test_point[0]),5,(255,255,0),-1))
#lines1 = lines1.reshape(-1,3)
#print(lines1)
#img5,img6 = drawlines(img1,img2,lines1,pts1,pts2)
# Find epilines corresponding to points in left image (first image) and
# drawing its lines on right image
#lines2 = cv2.computeCorrespondEpilines(cornersL.reshape((54,2)), 1,F)
#lines2 = lines2.reshape(-1,3)
#img3,img4 = drawlines(img2,img1,lines2,pts2,pts1)
#plt.subplot(121),plt.imshow(img5)
#plt.subplot(122),plt.imshow(img3)
#plt.show()