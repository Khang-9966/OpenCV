# Detect the gabbages by using OpenCV 
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

## Step by Step
 Ok, let start. First, when looking down we have a image as below.

<p align="center">
<img src=" https://user-images.githubusercontent.com/46638985/51757234-7136bd80-20f5-11e9-9cd8-a0fca945f6c9.png" width="50%" height="50%" >
<br>
  
 We can se the gabbages is green and it's the effect feature for dectecting. Furthermore, The white pipes is one additional feature. The first step is median filter, The median filter is the simple and basic method for removing noise.
