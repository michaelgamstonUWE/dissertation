from pydoc import doc
import mesa
import numpy as np
import math
from .agent import LWRobot, CleaningTank, PalmTree



class Beach(mesa.Model):
    """ Model representing a Beach needing to be cleaned"""

    def __init__(self, n_LWRobots=2, n_CleaningTank=2, n_PalmTrees=4, width=50, height=50, seed=123):
        
        self.tick = 0
        self.schedule = mesa.time.SimultaneousActivation(self)
        
        self.grid = mesa.space.MultiGrid(width, height, torus=True)
        
        #put LWRobots on the grid 
        # y_step = round(height / n_LWRobots)
        # x_mid = width / 2
        # y_mid = height / 2 
        # x_start = x_mid - math.floor(n_LWRobots / 2)
        # for n in range(n_LWRobots):
        #     #find search_coordinates 
        #     search_coordinate = []
        #     y_start = n * y_step
        #     for y in range(y_start, (y_step + y_start)): 
        #         search_coordinate.extend([(x, y) for x in range(0, width)])
                
        pos = (0, 0)
        lw_robot = LWRobot(1, self, pos, None)
        self.grid.place_agent(lw_robot, pos)
        
        # #put cleaning tanks on grid randomly 
        # for n in range(n_CleaningTank):
            
        #     while(True):
        #         pos = (np.random.randint(0, width), np.random.randint(0, height))
        #         if self.grid.is_cell_empty(pos):
        #             break
                
        #     cleaningTank = CleaningTank(n+n_LWRobots, self.grid, pos)
        #     self.grid.place_agent(cleaningTank, pos)
            
        # #place palm trees randomly
        # for n in range(n_PalmTrees):
            
        #     while(True):
        #         pos = (np.random.randint(0, width), np.random.randint(0, height))
        #         if self.grid.is_cell_empty(pos):
        #             break
                
        #     palmTree = PalmTree(n+(n_CleaningTank + n_LWRobots), self.grid, pos)
        #     self.grid.place_agent(palmTree, pos)
            

        self.running = True

    
            

    def step(self):
        
        pass