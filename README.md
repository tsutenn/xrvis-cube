# xrvis-cube

**xrvis-cube** is a Cubic tracker program approached by OpenCV, WebSocket and Qt. 

---

## Marker and Cube:

For each face on the cube: 

|---------------cube-length----------------|

|--margin--|--marker-length--|--margin--|

The program compare the markers on the cube to track the cube and its position and rotation.

### Variables:

`Size`: the dimension (n*n) of the marker.

`Length`: marker length (mm).

`margin`: the mrgin of the marker in a cube face.

## Base Cube and Coordinates:

The **Base Cube** determines the coordinate system of the system, meaning that the coordinates of other detected cubes are transformed into the local coordinate system of the base cube.

The positive direction of the marker where `marker_id`==0 on the "Base Cube" determines the positive direction of the coordinate system's x-axis.

### Variables:

`Base Position`: The offset of the position, which refers to the position of the base cube in client coordinate system.
This variable is only used for network transmission