# CS430_AnimationRenderer
This is a raytracing 3D renderer I wrote for my Computer Graphics class in Fall 2022.
## File Format
Files consist of a \<filemame\>.scene file containing the elements of the scene. Colors are given as decimals in a range from 0 to 1, and moving objects have an endpoint and velocity that determine how fast they move and where they move to.
Valid elements include:
  - camera: defines the height and width of the box that the rays are cast towards, as well as the FPS and Duration of the animation (if included
    - Example: camera, width: 4, height:2.25, fps:24, duration:2.5
  - sphere: A sphere in center-radius form, with position representing the center of the sphere
    - example: sphere, radius: 1.5, diffuse_color: [0, 0.5, 0], specular_color: [1, 1, 1], position: [0, 0, -5], reflectivity: 0.99, endpoint: [4, 4, -5]
  - plane: A plane in point-normal form, supports all parameters of a sphere except for the radius, which is replaced with a normal vector.
    - Example: plane, normal: [0,1,0], diffuse_color: [0.9, 0.5, 0], position: [0, -2, 0], reflectivity: 0.3
  - light: A point-light object, lighting scenes. has values for radial attenuation coefficients (radial_a0 represents a constant, radial_a1 is multiplied by distance, and radial_a2 is multiplied by the square of the distance, adjusting the light's intensity according to the formula 1/(a_0 + a_1 d +a_2 d^2). it also features light attenuation by an angle from the light, with theta representing the angle around a given vector that the light is valid for, and the angular_a0 value used to calculate the angle-based attenuation, used to create spotlights. a theta of 0 represents an omnidirectional light. a color value >0 represents an excessively bright light, and doubles the values of every color's saturation on an object.
    - example: light, color: [2, 2, 2], theta: 0, radial-a2: 0.125, radial-a1: 0.125, radial-a0: 0.125, position: [0, 4, -5]

## Usage
raytrace pixel_width pixel_height input.scene output.ppm
