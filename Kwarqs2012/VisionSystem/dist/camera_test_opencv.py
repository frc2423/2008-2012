from cv import *
from scipy import *
from matplotlib.pyplot import *
import sys
import cv
import pdb

# GLOBAL VARIABLES
NUMBER_OF_TARGETS                  = 2
FROM_AXIS                          = True
SHOW_COLOR_HISTOGRAMS              = True
SHOW_RECTANGLE_CONTOURS            = False
SHOW_HLS_IMAGE                     = False
SHOW_RGB_IMAGE                     = False
SHOW_HLS_THRESHOLDED_IMAGE         = False
SHOW_HLS_THRESHOLDED_DILATED_IMAGE = False
KERNEL_SIZE                        = 10
KERNEL_ANCHOR_OFFSET               = 5
NUMBER_ITERATIONS_DILATATION       = 1


def capture_images_from_axis(cam_address, 
								min_hls, 
								max_hls, 
								ks = KERNEL_SIZE, 
								koa = KERNEL_ANCHOR_OFFSET
								iter_dilate = NUMBER_ITERATIONS_DILATATION):
    
    capture = CaptureFromFile(cam_address)
    rgb = QueryFrame(capture)
    if SHOW_RGB_IMAGE: ShowImage("RGB Image", rgb)
	
	# Color conversion to HLS space
    hls = CreateMat(rgb.height, rgb.width, CV_8UC3)
    CvtColor(rgb, hls, CV_BGR2HLS)
	if SHOW_HLS_IMAGE: ShowImage("HLS Image", hls)
	
	# Color thresholding in the HLS color space
    hls_t = CreateMat(rgb.height, rgb.width, CV_8UC1)
    InRangeS(hls, min_hls, max_hls, hls_t)
	if SHOW_HLS_THRESHOLDED_IMAGE: ShowImage("HLS Image thresholded", hls_t)
    
	# Morphological dilatation to glue rectangle particles together 
	hls_t_d = CreateMat(rgb.height, rgb.width, CV_8UC1)
    kernel = CreateStructuringElementEx(ks, ks, kao, kao, CV_SHAPE_RECT);
    Dilate(hls_t, hls_t_d, kernel, iter_dilate)
	if SHOW_HLS_THRESHOLDED_DILATED_IMAGE: ShowImage("HLS Image thresholded and dilated", hls_t_d)
	
    return rgb, hls, hls_t, hls_t_d
	
def hls_hist(hls, rect):
	"""
	This generates 3 histograms from the 3 HLS channels with an ROI on the reflectors,
	that way we extract the color spectrum that is defined by the LED reflection.
	The ROIs are defined with 3 rectangle coordinates
	
	"""
	# loop on the channels
	for channel in range(3):
		values = []
		# loop over the rectangles
		n_rect = len(rect)
		for i in range(n_rect):
			temp_list = hls[rect[i][0]:rect[i][1], rect[i][2]:rect[i][3] , channel].ravel().tolist()
			values.append(temp_list)
		# concatenate all pixel values from each channel in one long list	
		values = reduce(lambda x,y:x+y,values)
		if SHOW_COLOR_HISTOGRAMS:
			figure(3) ; subplot(3,1,channel+1) ; hist(values, bins = 100) ; title('pixel values histogram on HLS color space, channel: ' + str(channel) )
	show()
	return 0

def find_contours(hls_t_d):
	"""
	This uses the binary thresholded and dilated image extracted from the HLS color space 
	and finds rectangle contours, then extract the 4 points polygon (rectangle in 3D perspective) 
	that represents the best that rectangle 
	"""
	
	seq = FindContours(hls_t_d, CreateMemStorage(), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE)
	external_color = RGB(255, 255, 255)
	hole_color = RGB(100, 255, 100)
	max_level = 1
	DrawContours(hls_t_d, seq, external_color, hole_color, max_level)
	for rect in range(NUMBER_OF_RECTANGLES):
		result = ApproxPoly(seq, CreateMemStorage(), CV_POLY_APPROX_DP, 10, 0)
		print result[:]
		seq = seq.h_next()
	if SHOW_RECTANGLE_CONTOURS: ShowImage("Rectangle contours", hls_t_d)
	
	# TODO: try those functions later, if the previous stuff doesn't work
	#box_2d = MinAreaRect2(seq, CreateMemStorage())
	#ConvexHull2(points, storage, orientation=CV_CLOCKWISE, return_points=0) 
	#FillPoly(img, polys, color, lineType=8, shift=0)
	#FillConvexPoly(img, pn, color, lineType=8, shift=0) ? None¶
	return 0
	
	
if __name__ == '__main__':
	
	if FROM_AXIS:
		cam_address = "http://10.24.23.11/mjpg/video.mjpg?resolution=640x480&req_fps=30&.mjpg"
	else:
		cam_address = 0
		
	# Define rectangle coordinates [y_min:y_max, x_min:x_max]
    rect = [[148,160, 186,237], [179,225, 447,454], [192,298, 541,592]]
	
	# Define min and max values to be filtered from the HLS color image
	min_hls = Scalar( 60, 120,  220, 0)
    max_hls = Scalar(140, 150, 255, 0)
    
    rgb, hls, hls_t, hls_t_d = capture_image_from_axis(min_hls, max_hls)
    hls = asarray(hls)
    hls_t = asarray(hls_t)
	
	hls_hist(hls, rect)
    find_contours(hls_t_d)
	
	
    






