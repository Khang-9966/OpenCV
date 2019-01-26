# Detecting the gabbages by using OpenCV 
## Overall
The smart farm is the hot topic in VietNam over recent year. In this project we try to use the computer vision and arm robot in order to harvest the vegetables at one smart farm. The project is deivided to part. One is CV and one is robot.
<p align="center">
<img src="https://user-images.githubusercontent.com/46638985/51755445-464a6a80-20f1-11e9-946a-91dd81b03c62.jpg" width="50%" height="50%" >
<br>
  Figure 1. <b>One Soilless Growing Farm in Viet Nam</b> – All systems are controlled automationly 
</p>

This demon will so you how to use the OpenCV to detect the center of the gabbages. Because the camera is set up look down and we want a fast implement, we won't use the deep learning in this task, use the traditional image processing algorithm for all. 

<p align="center">
<img src="https://user-images.githubusercontent.com/46638985/51756008-970e9300-20f2-11e9-8ad7-49969de9a3d4.png" width="50%" height="50%" >
<br>
  Figure 2. <b>The robot and camera system </b> – This is the first design and  
</p>
<p align="center">
<img src="https://user-images.githubusercontent.com/46638985/51789382-e61ffb00-21ba-11e9-81f0-86289d99174b.png" width="50%" height="50%" >
<br>
  Figure 3. <b>The result of all processing </b> – We want to know where are the centers of tha gabbages.
</p>


## Step by Step
 Ok, let start. First, when looking down we have a image as below.
 
</p>
<p align="center">
<img src="https://user-images.githubusercontent.com/46638985/51789468-b291a080-21bb-11e9-98d5-1d2e2e14c53d.png" width="25%" height="25%" >
<br>
  Figure 4. <b>The raw image </b> – The image we get it when looking down.
</p>
 
 We can se the gabbages is green and it's the effect feature for dectecting. Furthermore, The white pipes is one additional feature. The first step is median filter, The median filter is the simple and basic method for removing noise.
``` shell
medianBlur(img, imgMedian, 7);
```
</p>
<p align="center">
<img src="https://user-images.githubusercontent.com/46638985/51789614-12d51200-21bd-11e9-9eb5-83c2c836e266.png" width="50%" height="50%" >
<br>
  Figure 5. <b>The image after median filter processing </b>
</p>
After filting, we convert image from RGB to HSV color space, use the mornal threshold in HSV to make one mask includes all green pixeles. 

``` shell
cvtColor(imgMedian, imgHSV, COLOR_BGR2HSV);
inRange(imgHSV, cv::Scalar(35, 60, 40, 0), cv::Scalar(80, 255, 255, 0), imgTh);
inRange(imgHSV, cv::Scalar(0, 0, 200, 0), cv::Scalar(180, 50, 255, 0), imgThW);	
erode(imgTh, imgTh, Mat(), Point(0, 0), 2, 1, 1);
imgAfMask = Mat::zeros(img.size(), CV_8UC3);
img.copyTo(imgAfMask, imgTh);
```
</p>
<p align="center">
<img src="https://user-images.githubusercontent.com/46638985/51789883-7a409100-21c0-11e9-8410-4d7be789dfed.png" width="50%" height="50%" >
<br>
  Figure 6. <b>The mask after using threshold algrorithm </b>
</p>

Because the gabbages touch eac other, It's make very difficult to perform segmantation by one threshold algorithm. So, we use the watershed algorithm. You can enter the links for more detailed tutorial : </p>
https://docs.opencv.org/3.3.1/d3/db4/tutorial_py_watershed.html </p>
https://docs.opencv.org/3.4/d2/dbd/tutorial_distance_transform.html </p>



