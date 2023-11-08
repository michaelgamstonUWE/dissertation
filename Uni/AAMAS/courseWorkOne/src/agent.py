import mesa
from mesa.model import Model 

class LWRobot(mesa.Agent):
    
    def __init__(self, unique_id: int, model: Model, pos : tuple, search_coordinates : list) -> None:
        super().__init__(unique_id, model)
        self.x, self.y = pos 
        self.search_coordinates = search_coordinates
        self.coordinates_searched = []
        self.obstacles_encountered = []
        self.next_x, self.next_y = None, None
        self.hopper_fullness = 0 #(percent)
        
    def is_hopper_full(self) -> None:
        """ 
            Returns a boolean based on if the hopper is full 
        """
        return True if self.hopper_fullness == 100 else False
    
    def get_next_coordinate(self) -> None:
        """find next coordinate in list 
            check if we can move there, if not propose alternative course 
        """
        pass
    
    def go_to_bin(self) -> None:
        pass
    
    def pick(self) -> None:
        pass
     
    def step(self) -> None:
        pass
    
    def advance(self) -> None:
        pass
    
class CleaningTank(mesa.Agent):
    
    def __init__(self, unique_id: int, model: Model, pos) -> None:
        super().__init__(unique_id, model)
        self.x, self.y = pos 

class PalmTree(mesa.Agent):
    
    def __init__(self, unique_id: int, model: Model, pos) -> None:
        super().__init__(unique_id, model)
        self.x, self.y = pos