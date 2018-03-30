# gta-place-recognition
Evaluating place recognition algorithms using GTA V

# instructions
- Open solution file "datasetgen.sln" in Visual Studio
- Retarget if needed
- Build project(F7)
- copy file "datasetgen.asi" from "bin" folder to <GTA V root directory>
- Start game. Cycle to First Person Mode using v key
- Disable map and HUD using game settings menu for best results
- When in-game run mod using F4
- Controls: 
	- F9 starts data capture process (cannot be stopped or paused)
	- image files are located in "<GTA V root directory>/dataset/"
	- numpad 0 shows the current camera 6DOF pose(x,y,z location coordinates and camera pose)
	- numpad 0 also writes the camera 6DOF pose to a file "<GTA V root directory>/coords.txt"
- Once data capture complete copy "resizing-script.py" to "<GTA V root directory>/dataset/" and run
- Image files will be resized to 640x360 pixels and saved to "<GTA V root directory>/dataset/resized" folder