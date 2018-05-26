# Q-learning
Using Q-learning based reinforcement learning technique for predicting shortest path in a quantized grid 

This program can be used to predict shortest path in a finite quantized grid. The movable areas are highlighted with "green" colors while the restricted areas are highlighted with "black" color.    
The starting point is highlighted with "yellow" color, and the destination point is highlighted with "red color". Current position of the RL agent is denoted by "blue" color, while the predicted path is drawn  with "navy blue" color on right hand clip.    

The RL agent is allowed to take random moves and explore the movable areas in order to fill the Q-Matrix. Once the RL agent reaches destination point, reward is provides based on R-Matrix. Then, the RL agent re-initiates it's search from a new random point where it again look for random paths to reach the destination point. After some iterations, the predicted path(right hand side) seems to converge and the shorted possible path is highlighted.     
(Note: All the diagonal moves and straight moves are equal weightage)    

# Compile    
g++ qlearn.cpp -o main `pkg-config --cflags --libs opencv` -std=c++11     

# Usage   
./main arenaWidth arenaHeight X_start Y_start X_dest Y_dest    
(where, `arenaWidth` is the width of initial grid, `arenaHeight` is the height of initial grid, `X_start`&`Y_start` is the starting point, `X_dest`&`Y_dest` is the destination point. Make sure that both the points lie in the movable(green) area of the arena).    

# Example    
./main 20 20 5 5 17 3    
(Here, (20,20) is the grid size, (5,5) is the starting point, (17,3) is the destination point).    

# Result in action    
Left side is the RL agent moving randomly to search for the destination, while right side is the predicted path using updated Q-matrix    
![Alt Text](https://raw.githubusercontent.com/2vin/Q-learning/master/result/qlearn.gif)
