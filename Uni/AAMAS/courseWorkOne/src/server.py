# Make a world that is 50x50, on a 250x250 display.
import mesa
from src.model import Beach
from .portayel import beach_portrayal



SIZE_OF_CANVAS_IN_PIXELS_X = 500
SIZE_OF_CANVAS_IN_PIXELS_Y = 500

# TODO Add a parameter named "n_boxes" for the number of boxes to include in the model.
simulation_params = {
    "height": 50, # PLACE HOLDERS 
    "width": 50,
    # "n_robots": mesa.visualization.Slider(
    #     'number of robots',
    #     2, #default
    #     1, #min
    #     10, #max
    #     1, #step
    #     "choose how many robots to include in the simulation"
    # )
    # ,
    # "n_boxes": 
    #     mesa.visualization.Slider(
    #     'number of boxes',
    #     5, #default
    #     1, #min
    #     20, #max
    #     1, #step
    #     "choose how many boxes to include in the simulation",
        
    # )

    }
grid = mesa.visualization.CanvasGrid(beach_portrayal, 50, 50, SIZE_OF_CANVAS_IN_PIXELS_X, SIZE_OF_CANVAS_IN_PIXELS_Y)


server = mesa.visualization.ModularServer(
    Beach, [grid], "Modern Warehouse", simulation_params
)