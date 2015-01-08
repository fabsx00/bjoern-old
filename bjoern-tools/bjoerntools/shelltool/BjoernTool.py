
from bjoerntools.shelltool.PipeTool import PipeTool
from bjoerntools.DBInterface import DBInterface

class BjoernTool(PipeTool):
    
    def __init__(self, DESCRIPTION):
        PipeTool.__init__(self, DESCRIPTION)
    
    # @Override
    def streamStart(self):
        self.dbInterface = DBInterface()
        self.dbInterface.connectToDatabase()
    
    def _runGremlinQuery(self, query):
        return self.dbInterface.runGremlinQuery(query)
    
