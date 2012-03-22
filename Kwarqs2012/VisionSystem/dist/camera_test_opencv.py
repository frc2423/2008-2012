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
SHOW_RECTANGLE_CONTOURS            = True
SHOW_HLS_IMAGE                     = True
SHOW_RGB_IMAGE                     = True
SHOW_HLS_THRESHOLDED_IMAGE         = True
SHOW_HLS_THRESHOLDED_DILATED_IMAGE = True
KERNEL_SIZE                        = 10
KERNEL_ANCHOR_OFFSET               = 5
NUMBER_ITERATIONS_DILATATION       = 1
IMAGE_WIDTH                        = 640
IMAGE_HEIGHT                       = 480
AXIS_CAMERA_VIEW_ANGLE             = pi*38.33/180.0 # in degrees
AXIS_FOCAL_LENGTH                  = 0.0044 # in meters
TARGET_WIDTH                       = 24 
TARGET_HEIGHT                      = 18
#TARGET_RELATIVE_ELEVATION_TO_ROBOT = 
CAMERA_MOUNT_HEIGHT                = 35 # TODO (not sure) height of the camera on the robot
DISTANCE_HOOP_FROM_TARGET_X        = 6 # from center of hoop to target panel
DISTANCE_HOOP_FROM_TARGET_Z        = 2 
DISTANCE_CAMERA_TO_SHOOTER         = 10# TODO don't know

def capture_image_from_axis(cam_address, 
                                min_hls, 
                                max_hls, 
                                ks = KERNEL_SIZE, 
                                kao = KERNEL_ANCHOR_OFFSET,
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
    
#-----------------------------------------------------------------------------#   
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

#-----------------------------------------------------------------------------#
def find_contours(hls_t_d):
    """
    This uses the binary thresholded and dilated image extracted from the HLS color space 
    and finds rectangle contours, then extract the 4 points polygon (rectangle in 3D perspective) 
    that represents the best that rectangle 
    """
    rectangle_corners = []
    seq = FindContours(hls_t_d, CreateMemStorage(), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE)
    external_color = RGB(255, 255, 255)
    hole_color = RGB(100, 255, 100)
    max_level = 1
    DrawContours(hls_t_d, seq, external_color, hole_color, max_level)
    for rect in range(NUMBER_OF_TARGETS):
        result = ApproxPoly(seq, CreateMemStorage(), CV_POLY_APPROX_DP, 10, 0)
        print result[:]
        rectangle_corners.append(result[:])
        seq = seq.h_next()
    if SHOW_RECTANGLE_CONTOURS: figure() ; imshow(asarray(hls_t_d)) ; title('Rectangle Contours') ; show()
    
    #ShowImage("Rectangle contours", hls_t_d)
    
    # TODO: try those functions later, if the previous stuff doesn't work
    #box_2d = MinAreaRect2(seq, CreateMemStorage())
    #ConvexHull2(points, storage, orientation=CV_CLOCKWISE, return_points=0) 
    #FillPoly(img, polys, color, lineType=8, shift=0)
    #FillConvexPoly(img, pn, color, lineType=8, shift=0) ?
    return rectangle_corners

#-----------------------------------------------------------------------------#
def choose_biggest_target(rc):
    """
    This is looking for the closest rectangle assuming that the largest perimeter 
    corresponds to the closest rectangle
    """
    n_rect_found = len(rc)
    perimeters = []
    
    for rect in range(n_rect_found):
        perimeters.append(sqrt(((array(rc[rect][0]) - array(rc[rect][1]))**2).sum()) + \
        sqrt(((array(rc[rect][1]) - array(rc[rect][2]))**2).sum()) + \
        sqrt(((array(rc[rect][2]) - array(rc[rect][3]))**2).sum()) + \
        sqrt(((array(rc[rect][3]) - array(rc[rect][0]))**2).sum()))
    perimeters = array(perimeters)
    closest_rectangle = perimeters.argmax()
    corners_closest_rectangle = rc[closest_rectangle]
    
    return corners_closest_rectangle        

#-----------------------------------------------------------------------------#
def find_rect_center(corners):
    """
    Find the center coordinates of the rectangle defined by the corners
    #TODO make it use the intersecton function instead
    """
    x_a = double(corners[0][0])
    x_b = double(corners[1][0])
    x_c = double(corners[2][0])
    x_d = double(corners[3][0])
    y_a = double(corners[0][1])
    y_b = double(corners[1][1])
    y_c = double(corners[2][1])
    y_d = double(corners[3][1])
    
    a = (y_b - y_d)/(x_b - x_d)
    b = y_b - x_b * (y_b - y_d) / (x_b - x_d)
    c = (y_a - y_c)/(x_a - x_c)
    d = y_a - x_a * (y_a - y_c) / (x_a - x_c)
    
    x_o = (b - d) / (c - a)
    y_o = a * (b-d)/(c-a) + b
    center = [x_o, y_o]
    
    return center

#-----------------------------------------------------------------------------#
def get_target_dimensions(corners, center):
    """
    targets look like polygons in perspective geometry, this will find the height and witdh 
    of a target based on the intersection of their "lines"...
    """
    A = corners[0]
    B = corners[1]
    C = corners[2]
    D = corners[3]
    
    N = intersection_coordinates_of_two_lines(A, D, B, C)
    if isnan(N).any():
        height = norm_vector(A, D)
    else:
        Q = intersection_coordinates_of_two_lines(N, center, A, B)
        S = intersection_coordinates_of_two_lines(N, center, D, C)
        height = norm_vector(Q, S)
    
    M = intersection_coordinates_of_two_lines(A, B, D, C)
    if isnan(M).any():
        width = norm_vector(A, B)
    else:
        P = intersection_coordinates_of_two_lines(M, center, A, D)
        R = intersection_coordinates_of_two_lines(M, center, B, C)        
        width = norm_vector(P, R)
       
    return width, height
    
#-----------------------------------------------------------------------------#        
def norm_vector(A, B):
    norm_AB = sqrt(((array(A) - array(B))**2).sum())

    return norm_AB
    
#-----------------------------------------------------------------------------#    
def get_exact_distance_and_angle_to_hoop(corners,
                                        z,
                                        f   = AXIS_FOCAL_LENGTH,
                                        h_0 = TARGET_HEIGHT,
                                        w_0 = TARGET_WIDTH,
                                        h_r = CAMERA_MOUNT_HEIGHT,
                                        l_x = DISTANCE_HOOP_FROM_TARGET_X,
                                        l_z = DISTANCE_HOOP_FROM_TARGET_Z,
                                        a   = DISTANCE_CAMERA_TO_SHOOTER):

    w_im, h_im = get_target_dimensions(corners)
    alpha =  arcsin(2 * z * h_im / (f * h_0)) / 2.0
    d_h = z / tan(alpha)
    d = z / sin(alpha)
    beta = arccos(d * w_im / (f * w_0))
    beta_h = arcsin ( sin(beta) / cos(alpha))
    x = d_h * cos(beta_h)
    y = d_h * sin(beta_h)
    
    # those coordinates are in the target's referential (center target being at 
    # coordinates [0, 0, z+z_h+h_0/2])
    camera_relative_coordinates  = [x, -y, h_r]
    shooter_relative_coordinates = [x - a * cos(beta_h), y + a * sin(beta_h), h_r]
    hoop_relative_coordinates    = [l_x, 0 , z + z_h - h_0/2.0 - l_z]
    distance_shooter_to_hoop     = sqrt(((array(shooter_relative_coordinates) - array(hoop_relative_coordinates))**2).sum())
    R = shooter_relative_coordinates
    H = hoop_relative_coordinates
    T = [0, 0, z + z_h - h_0/2.0]
    HR_sq = (array(H - R)**2).sum()
    RT_sq = (array(R - T)**2).sum()
    HT_sq = (array(R - T)**2).sum()
    HR    = sqrt(HR_sq)
    RT    = sqrt(RT_sq)
    # cosyne formula
    theta_from_target_to_hoop = arcos((HR_sq + RT_sq - HT_sq)/(2 * HR * RT))

    # TODO: the whole angle susan now
    # basic_susan = 
    # angle_susan = theta_from_target_to_hoop + 
    
    return distance_shooter_to_hoop, theta_from_target_to_hoop
    
#-----------------------------------------------------------------------------#
def get_simple_distance_and_angle_to_hoop(corners, center):
    """
    depending on the rectangle coordinates, find the basket's coordinates and the angle to 
    wich the lazy susan should rotate to 
    """
    # simple version (turning to face the center of the target)
    angle_susan = (IMAGE_WIDTH/2.0 - center[0]) * AXIS_CAMERA_VIEW_ANGLE / double(IMAGE_WIDTH)
    rect_width  = norm_vector(corners[2], corners[3])
    distance    = (IMAGE_WIDTH * 22) / (2 * rect_width * tan(AXIS_CAMERA_VIEW_ANGLE/2.0))
    
    return distance, angle_susan

#-----------------------------------------------------------------------------#
def label_rectangles(rectangle_corners):
    #TODO
    labels = []


    return labels
    
#-----------------------------------------------------------------------------#    
def intersection_coordinates_of_two_lines(A, B, C, D):
    """
    This looks for the coordinates of the intersection point between the AB and CD lines
    """
    x_a = A[0] ; y_a = A[1]
    x_b = B[0] ; y_b = B[1]
    x_c = C[0] ; y_c = C[1]
    x_d = D[0] ; y_d = D[1]
    
    # assuming y = a_AB * x + b_AB is the AB equation and y = a_CD * x + b_CD for CD
    # TODO MAKE SURE ALL THE DIVISION ARE NOT ON 0
    if (x_b -x_a == 0) and (x_d - x_c ==0):
        # both lines are vertical
        intersection_coordinates = NaN
    elif (x_b - x_a == 0) and (x_d - x_c != 0):
        # AB is vertical and CD is not
        x_m = x_a
        y_m = a_CD * x_a + b_CD
        intersection_coordinates = [x_m, y_m]
    elif (x_b - x_a != 0) and (x_d - x_c == 0):
        # CD is vertical and AB is not
        x_m = x_c
        y_m = a_AB * x_c + b_AB
        intersection_coordinates = [x_m, y_m]
    else: # all other possibilities    
        a_AB = (y_b - y_a) / (x_b - x_a)
        b_AB = y_a - x_a * a_AB
        a_CD = (y_d - y_c) / (x_d - x_c)
        b_CD = y_c - x_c * a_CD
        
        if a_AB == a_CD:
            # parrallel lines
            intersection_coordinates = NaN
        else:
            x_m = (b_CD - b_AB) / (a_AB - a_CD)
            y_m = a_AB * x_m + b_AB    
            intersection_coordinates = [x_m, y_m]

    return intersection_coordinates    

#-----------------------------------------------------------------------------#   
if __name__ == '__main__':
    
    if FROM_AXIS:
        cam_address = "http://10.24.23.11/mjpg/video.mjpg?resolution=640x480&req_fps=30&.mjpg"
    else:
        cam_address = 0
        
    # Define rectangle coordinates [y_min:y_max, x_min:x_max]
    #rect = [[148,160, 186,237], [179,225, 447,454], [192,298, 541,592]]
    rect = [[56,64, 150,213], [34,45, 395,467], [144,153, 412,478]]
    
    
    # Define min and max values to be filtered from the HLS color image
    min_hls = Scalar( 60, 120,  220, 0)
    max_hls = Scalar(140, 150, 255, 0)
    
    rgb, hls, hls_t, hls_t_d = capture_image_from_axis(cam_address, min_hls, max_hls)
    hls = asarray(hls)
    hls_t = asarray(hls_t)
    
    hls_hist(hls, rect)
    rectangle_corners     = find_contours(hls_t_d)
    #rect_labels          = label_rectangles(rectangle_corners)
    corners               = choose_biggest_target(rectangle_corners)
    center                = find_rect_center(corners)
    distance, angle_susan = get_simple_distance_and_angle_to_hoop(corners, center)
    #distance, angle_susan = get_exact_distance_and_angle_to_hoop(corners)
    
    print "distance in inches: " + str(distance)
    print "angle_susan in degrees: " + str(180 * angle_susan / pi)
    