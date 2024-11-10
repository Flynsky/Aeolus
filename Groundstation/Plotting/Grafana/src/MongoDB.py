import pymongo
import time
import math

localpath = "mongodb://localhost:27017/"
atlaspath = "mongodb+srv://admin:admin@cluster0.hrga0.mongodb.net/"
PATH = atlaspath

class Database_MongoDB:
    def __init__(self):
        myclient = pymongo.MongoClient(PATH)
        self.__db = myclient["Database_Drone"]
        self.imu_col = self.__db["IMU_Data"]
    
    def add(self,value):
        id = self.imu_col.insert_one(value)
        
if __name__=='__main__':
    db = Database_MongoDB()
    starttime = time.time_ns()
    while True:
        sinevalue = math.sin((time.time_ns()-starttime)/1000/1000/1000/10)
        print(sinevalue)
        data = { "sine":sinevalue,"axis_x": 0, "axis_y": 2 }
        db.add(data)
        
        time.sleep(0.25)