'''
    Not quite sure the best way to implement this yet. Perhaps
    we won't, in fact. But here's a placeholder just in case.
'''


class TrackingData(object):
    x = 0
    y = 0
    angularVelocity = 0
    validData = False


class BasketTracker(object):

    def __init__(self):
        pass
        
    def GetData(self):
        return TrackingData()

 
 