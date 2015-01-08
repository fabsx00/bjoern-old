
from bjoern.all import BjoernSteps
import os

BJOERN_TOOLS_STEPDIR = os.path.join(os.path.dirname(__file__), 'steps')

class DBInterface:
    
    def connectToDatabase(self):
        self.j = BjoernSteps()
        self.j.addStepsDir(BJOERN_TOOLS_STEPDIR)
        self.j.connectToDatabase()
    
    def runGremlinQuery(self, query):
        return self.j.runGremlinQuery(query)
    
    def chunks(self, ids, chunkSize):
        return self.j.chunks(ids, chunkSize)
    
