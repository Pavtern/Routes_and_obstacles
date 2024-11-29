Route Planner with Obstacle Avoidance
This project is a graphical route-planning application that visualizes routes between two points while avoiding obstacles. It uses SFML for rendering the GUI and provides basic geographic computations, such as calculating distances and checking for obstacles on a route.

Features
Interactive Point Selection:
Select a start and end point by clicking on the screen.

Dynamic Obstacle Generation:
Randomly generate obstacles on the map that the route must avoid.

Route Calculation:
Compute an initial route with configurable intermediate steps.
Automatically detect and validate routes for obstacle interference.
Rerouting:
Automatically generate an alternative route when obstacles are detected.
Real-time Visualization:
Draw the route, including intermediate steps and total distance.
Color-coded routes:
Valid routes are black.
Invalid routes are yellow.
Rerouted paths are cyan.
Textual Feedback:
Clear instructions for user interaction.
Display the total distance of the route.
How It Works
Start Point Selection:
Click anywhere on the screen to set the starting point (green).
End Point Selection:
Click again to select the endpoint (blue).
Route Generation:
The program calculates the shortest path with intermediate steps and checks for obstacles.
Obstacle Avoidance:
If obstacles interfere with the route, you can press R to reroute.
Feedback:
The app shows the current status of the route and the total distance in meters.
Technical Details
SFML:
Used for rendering and handling graphical components.
Geographic Calculations:
Includes GPS distance calculation between points.
Validates routes by checking if any lines or intermediate steps intersect with obstacles.
