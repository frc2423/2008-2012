
class SmartDashboard(object):

    instance = None
    
    @staticmethod
    def GetInstance():
        if SmartDashboard.instance is None:
            SmartDashboard.instance = SmartDashboard()
        return SmartDashboard.instance
        
    def __init__(self):
        self.data = {}
        
    def GetBoolean(self, key):
        if key in self.data:
            return self.data[key]
        return None
        
    def GetInt(self, key):
        if key in self.data:
            return self.data[key]
        return None
        
    def PutInt(self, key, value):
        self.data[key] = int(value)
        
    def PutBoolean(self, key, value):
        self.data[key] = bool(value)
        