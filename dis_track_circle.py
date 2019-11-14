#!/usr/bin/env python
import roslib
roslib.load_manifest('stereo_color_tracker')
import sys
import rospy
import cv2
import numpy as np
from std_msgs.msg import String, Header
from sensor_msgs.msg import Image , PointCloud2 , PointField ,PointCloud
from geometry_msgs.msg import PointStamped, Point
import std_msgs.msg 
from cv_bridge import CvBridge, CvBridgeError
import pickle
import matplotlib.pyplot as plt
from sklearn.preprocessing import normalize
from itertools import combinations 
from sklearn.linear_model import LinearRegression
# import IPython

class disparity_track:

  def __init__(self):
    f = open('L_proto2.pckl', 'rb')
    self.Left_Stereo_Map = pickle.load(f)
    f.close()
    f = open('R_proto2.pckl', 'rb')
    self.Right_Stereo_Map = pickle.load(f)
    f.close()
    f = open('FundamentalMat.pckl', 'rb')
    self.F = pickle.load(f)
    f.close()
    f = open('FundamentalMat_mask.pckl', 'rb')
    self.mask = pickle.load(f)
    f.close()

    
    self.left_point_pub = rospy.Publisher("left_point", PointStamped, queue_size=5)
    self.left_point_pub_min = rospy.Publisher("left_point_min", PointStamped, queue_size=5)
    self.left_point_pub_mid = rospy.Publisher("left_point_mid", PointStamped, queue_size=5)
    self.left_point_pub_max = rospy.Publisher("left_point_max", PointStamped, queue_size=5)
    self.left_point_clound = rospy.Publisher("point_clound", PointCloud, queue_size=5)

    self.pub_list = [self.left_point_pub_min,self.left_point_pub_mid,self.left_point_pub_max]
  
    self.bridge = CvBridge()
    
    self.image_sub = rospy.Subscriber("/left_cam/image_raw",Image,self.left_callback)
    self.image_sub = rospy.Subscriber("/right_cam/image_raw",Image,self.right_callback)
    
    
    self.lower_threshold = np.array([0, 0, 246])
    self.upper_threshold = np.array([255, 255, 255])
    
    self.disparity_ratio = 211.4
    self.center_x = (640.0/2.0) # half x pixels
    self.center_y = (480.0/2.0) # half y pixels
    self.Kinv = np.matrix([[861.7849547322785, 0, 320.0], 
                           [0, 848.6931340450212, 240.0],
                           [0, 0, 1]]).I # K inverse
    
    self.circles_1 = None
    self.circles_2 = None
    
    self.para1L = 200
    self.para2L = 100
    self.para1R = 200
    self.para2R = 100

    cv2.namedWindow("Control"); # Threshold Controller window
    cv2.createTrackbar("Disparity ratio", "Control", int(self.disparity_ratio*10), 5000, self.updateDisparity)
    cv2.createTrackbar('para1L','Control',166,255,self.updatepara1L)
    cv2.createTrackbar('para2L','Control',45,255,self.updatepara2L)
    cv2.createTrackbar('para1R','Control',166,255,self.updatepara1R)
    cv2.createTrackbar('para2R','Control',47,255,self.updatepara2R)
    cv2.createTrackbar('H_low','Control',0,255,self.updateLowH)
    cv2.createTrackbar('S_low','Control',0,255,self.updateLowS)
    cv2.createTrackbar('V_low','Control',255,255,self.updateLowV)
    cv2.createTrackbar('H_High','Control',180,255,self.updateHighH)
    cv2.createTrackbar('S_High','Control',255,255,self.updateHighS)
    cv2.createTrackbar('V_High','Control',255,255,self.updateHighV)


    self.last_left_image_pos = np.array([0.0, 0.0])

  def updateLowH(self, value):
    self.lower_threshold[0] = value
  def updateHighH(self, value):
    self.upper_threshold[0] = value
  def updateLowS(self, value):
    self.lower_threshold[1] = value
  def updateHighS(self, value):
    self.upper_threshold[1] = value
  def updateLowV(self, value):
    self.lower_threshold[2] = value
  def updateHighV(self, value):
    self.upper_threshold[2] = value
  def updateDisparity(self, value):
    self.disparity_ratio = float(value) * 0.1
  def updatepara1L(self, value):
    self.para1L = value
  def updatepara2L(self, value):
    self.para2L = value
  def updatepara1R(self, value):
    self.para1R = value
  def updatepara2R(self, value):
    self.para2R = value

  def left_callback(self,data):
    try:
      cv_image = self.bridge.imgmsg_to_cv2(data, "bgr8")
    except CvBridgeError, e:
      print e
    cv_image= cv2.remap(cv_image,self.Left_Stereo_Map[0],self.Left_Stereo_Map[1], cv2.INTER_LANCZOS4, cv2.BORDER_CONSTANT, 0)  
    
    hsv = cv2.cvtColor(cv_image, cv2.COLOR_BGR2HSV)
    mask = cv2.inRange(hsv, self.lower_threshold, self.upper_threshold)
    mask = cv2.morphologyEx(mask,cv2.MORPH_DILATE, np.ones((3,3),np.uint8),iterations = 3)
    
    res = cv2.bitwise_and(cv_image,cv_image, mask= mask )
    cv2.imshow("test",res)

    cv_image = res
    gray = cv2.cvtColor(cv_image, cv2.COLOR_BGR2GRAY)
    gray = cv2.medianBlur(gray, 3)
    rows = gray.shape[0]
    
    self.circles_1 = cv2.HoughCircles(gray, cv2.HOUGH_GRADIENT, 1, rows / 8,
                                param1=self.para1L, param2=self.para2L,
                                minRadius=0, maxRadius=0)
    self.points_L = []  
                             
    if self.circles_1 is not None:
      for circle in self.circles_1[0]:
        self.points_L.append(circle[0:3])
    self.points_L = np.array(self.points_L, dtype = int)
    
    #print(self.points_L)
    if len(self.points_L) != 0:
      self.line_estimation(self.points_L.T,0.8,cv_image,True)
    cv_image = self.draw(self.circles_1,cv_image)
    self.cv_image_L = cv_image
    
    cv2.imshow("detected circles Left", cv_image)
    k = cv2.waitKey(3) & 0xFF
    if k == 113 or k == 27:
      rospy.signal_shutdown("User Exit")

  def right_callback(self,data):
    try:
      cv_image = self.bridge.imgmsg_to_cv2(data, "bgr8")
    except CvBridgeError, e:
      print e
    cv_image= cv2.remap(cv_image,self.Right_Stereo_Map[0],self.Right_Stereo_Map[1], cv2.INTER_LANCZOS4, cv2.BORDER_CONSTANT, 0)
    gray = cv2.cvtColor(cv_image, cv2.COLOR_BGR2GRAY)
    gray = cv2.medianBlur(gray, 3)
    rows = gray.shape[0]
    self.circles_2 = cv2.HoughCircles(gray, cv2.HOUGH_GRADIENT, 1, rows / 8,
                                param1=self.para1R, param2=self.para2R,
                                minRadius=0, maxRadius=0)
    self.points_R = []                           
    if self.circles_2 is not None:
      for circle in self.circles_2[0]:
        self.points_R.append(circle[0:3])
    self.points_R = np.array(self.points_R, dtype = int)

    if len(self.points_R) != 0:
      self.line_estimation(self.points_R.T,0.9,cv_image,False)
    cv_image = self.draw(self.circles_2,cv_image)

    # print(self.circles_1[0,0,0:2])
    # for point in self.three_points_L :
    #   lines_inR = cv2.computeCorrespondEpilines(point[0:2].reshape((-1,2)), 2,self.F)
    #   lines_inR = lines_inR.reshape(-1,3)
    #   cv_image = self.draw_line(cv_image,lines_inR)
    cv2.imshow("detected circles Right", cv_image)

    # lines_inL = cv2.computeCorrespondEpilines(self.circles_2[0,0,0:2].reshape((-1,2)), 1,self.F)
    # lines_inL = lines_inL.reshape(-1,3)
    # self.cv_image_L = self.draw_line(self.cv_image_L,lines_inL)
    # cv2.imshow("detected circles Left_new", self.cv_image_L)
    depth_list = []
    if len(self.three_points_L) == 3 and len(self.three_points_R) == 3 :
      self.three_points_R = self.sort(self.three_points_R)
      self.three_points_L = self.sort(self.three_points_L)
      #for point_in_L in self.three_points_L:
      #dx = float(self.last_left_image_pos[0] - cx)
      #depth = self.disparity_ratio/dx
      for i in range(3):
        depth = self.caculate_distance(f=820,baseline=120,XL=self.three_points_L[i],XR=self.three_points_R[i])
        depth_list.append(depth)
        print(i,"depth ",depth)
        self.postLeftPoint(self.three_points_L[i][0],self.three_points_L[i][1],depth,self.pub_list[i])
      
      #self.postPointclound(self.three_points_L[i][0],self.three_points_L[i][1],depth,self.left_point_clound)
      self.depth_gress(self.three_points_L,np.array(depth_list),cv_image.shape,200)
        
  def depth_gress(self,xy,z,imgshape, num_sam):
    print(xy)
    print(z.T)
    regr = LinearRegression()
    regr.fit(xy, z.T)
    print( 'Solution found by scikit-learn  : ', regr.coef_ )
    print( 'Solution found by scikit-learn  : ', regr.intercept_ )
    x_draw = np.linspace(0, imgshape[1], num_sam)
    y_draw = x_draw*self.coef_ + self.intercept_
    xy_draw =  np.stack((x_draw.reshape((num_sam,1))[:,0], y_draw.T[:,0])).T
    z_draw = regr.predict(xy_draw)
    xy_draw =  np.concatenate((xy_draw, np.ones_like(x_draw).reshape(num_sam,1) ), axis = 1)
    #xy_draw =  np.concatenate((xy_draw.reshape(10,2), np.ones((10,1))), axis=1)
    worldPos = np.multiply( np.dot(  xy_draw , self.Kinv.T ) , z_draw.reshape(num_sam,1))
    #print('sss',worldPos)
    self.postPointclound(worldPos,self.left_point_clound)

  def sort(self,array):
    print(array)
    temp = []
    temp.append(array[self.min_position(array),:])
    temp.append(array[self.mid_position(array),:])
    temp.append(array[self.max_position(array),:])
    return np.array(temp)
  
  def max_position(self,array):
    array = array.T
    return np.where(array[0] == array.max(axis=1)[0])[0][0] 

  def min_position(self,array):
    array = array.T
    return np.where(array[0] == array.min(axis=1)[0])[0][0] 

  def mid_position(self,array):
    array = array.T
    k = (3 - int(np.where(array[0] == array.min(axis=1)[0])[0][0]) - int(np.where(array[0] == array.max(axis=1)[0])[0][0]))
    return k

  def draw_line(self, cv_image, lines):
    c = cv_image.shape[1]
    for line in lines:
      x0,y0 = map(int, [0, -line[2]/line[1] ])
      x1,y1 = map(int, [c, -(line[2]+line[0]*c)/line[1] ])
      cv_image = cv2.line(cv_image, (x0,y0), (x1,y1), (255,255,255),1)
    return cv_image

  def postLeftPoint(self, x, y, depth, pub):
    worldPos = self.Kinv * np.matrix([x,y,1]).T * depth
    point = PointStamped(header=Header(stamp=rospy.Time.now(),
                                       frame_id='/left_camera'),
                         point=Point(worldPos[0],worldPos[1], worldPos[2]))
    print(Point(worldPos[0],worldPos[1], worldPos[2]))
    pub.publish(point)

  def postPointclound(self, points , pub):
    #point = self.xyz_array_to_pointcloud2(worldPos,stamp=rospy.Time.now(),frame_id='/left_camera')
    #point = PointCloud2(header=Header(stamp=rospy.Time.now(),
    #                                    frame_id='/left_camera'),
    #                      point=Point(worldPos[0],worldPos[1], worldPos[2]))
    debug_pointcloud = PointCloud()
    debug_pointcloud.header = std_msgs.msg.Header()
    debug_pointcloud.header.stamp = rospy.Time.now()
    debug_pointcloud.header.frame_id = '/left_camera'

    number_of_pixels = points.shape[0]
    # create an empty list of correct size
    debug_pointcloud.points = [None] * number_of_pixels

    # fill up pointcloud with points where x value changes but y and z are all 0
    for ind,p in enumerate(points):
        print(ind)
        print(p)
        debug_pointcloud.points[ind] = Point(p[0,0],p[0,1],p[0,2])
    
    pub.publish(debug_pointcloud)

  def caculate_distance(self,f,baseline,XL,XR):
    dist = baseline*f/(XL[0]-XR[0])               
    print('----------------------------------')
    # print("Pixel: ",dist)
    dist = 6.16922869 + dist*07.55723716e-02 + dist**2*5.36279911e-05 \
    + dist**3*-3.12188045e-08 + dist**4*2.20282126e-12 
    return dist

  def draw(self,circles,img):
        if circles is not None:
            circles = np.uint16(np.around(circles))
            for i in circles[0]:
              center = (i[0], i[1])
              cv2.circle(img, center, 3, ( 255, 0, 0), 3)
              radius = i[2]
              cv2.circle(img, center, radius, (0, 0, 255), 8) 
              cv2.putText(img, str(center) , center, cv2.FONT_HERSHEY_SIMPLEX, 0.4, (255,0,0),1)
            return img
        else:
            #print(' No circle ! ')
            return img

  def line_estimation(self, X, thred, img, left):
    if len(X) > 10 : 
      return img
    comb = combinations(X.T, 3) 
    reg_list = []
    if left == True:
      self.three_points_L = np.array([])
    else: 
      self.three_points_R = np.array([])
    # Print the obtained combinations 
    for i in list(comb): 
        test = np.array(i)[:,0:2].T
        radi = np.array(i)[:,2:3].T
        #print(i) 
        cor = np.corrcoef(test)
        print(np.sum( np.abs(radi - np.mean(radi))))
        if (cor[0,1] >= thred or cor[0,1] <= -thred) and ( np.sum( np.abs(radi - np.mean(radi))) < 20 ) :
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
                img = self.draw_line(img,np.array([-reg.coef_,1,-reg.intercept_]))
                reg_list.append(reg)
          else:
              reg = LinearRegression().fit(x_t,y_t)
              img = self.draw_line(img,np.array([[-reg.coef_,1,-reg.intercept_]]))
              reg_list.append(reg)
              if left == True:
                self.three_points_L = np.array(i)[:,0:2]
                self.coef_ = reg.coef_
                self.intercept_ = reg.intercept_
              else: 
                self.three_points_R = np.array(i)[:,0:2]
    print('                     Total lines detected: ', len(reg_list))
    return img

def main(args):
  rospy.init_node('disparity_track', anonymous=True)
  ic = disparity_track()
  try:
    rospy.spin()
  except KeyboardInterrupt:
    print "Shutting down"
  cv2.destroyAllWindows()
  print "Finished."

if __name__ == '__main__':
    main(sys.argv)