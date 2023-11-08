from .agent import LWRobot, CleaningTank, PalmTree

def beach_portrayal(agent):
    """To return the correct portrayal of each agent type

    Args:
        agent (_type_): _description_

    Returns:
        _type_: _description_
    """
   
    if isinstance(agent,LWRobot):
        return LWrobot_portrayal(agent)
    elif isinstance(agent, CleaningTank):
        return CleaningTank_portayal(agent)
    else: 
        return PalmTree_portayal(agent)
   
def get_color(hopper_fullness):
    if hopper_fullness > 90:
        return "red"
    elif hopper_fullness > 50: 
        return "yellow"
    else: 
        return "green"

def LWrobot_portrayal(robot):

    if robot is None:
        raise AssertionError
    return {
        "Shape": "arrowHead",
        "w": 1,
        "h": 1,
        "Filled": "true",
        "Layer": 0,
        "x": robot.x,
        "y": robot.y,
        "scale": 2,
        # "r":4,
        "Color": "red" if robot.hopper_fullness > 90 else "red",
    }
    
def CleaningTank_portayal(robot):
    pass

def PalmTree_portayal(tree):
    pass