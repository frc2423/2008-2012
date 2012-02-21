'''
    Not quite sure the best way to implement this yet. Perhaps
    we won't, in fact. But here's a placeholder just in case.
'''


class TrackingData(object):
   
   frame_number = 0
   
   valid_frames = 0
   
   target_data_valid = False
   
   x = 0
   y = 0
   
   distance = 0.0
   
   sonar_distance = 0.0


class BasketTracker(object):

    def __init__(self):
        pass
        
    def GetData(self):
        return TrackingData()

 
 