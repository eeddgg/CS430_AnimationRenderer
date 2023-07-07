#include "PPM.h"
#include "v3math.h"
#define _x [0]
#define _y [1]
#define _z [2]
#define PI_a 3.14159265358979323846f 

typedef struct color{
    float red;
    float green;
    float blue;
} color;


typedef enum scene_type{Sphere=0, Plane=1} scene_type;



typedef struct scene_item{
    scene_type type;
    color item_color;
    color diffuse_color;
    color specular_color;
    color ambient_color;
    vector position;
    vector start_position;
    float radius;
    float reflectivity;
    vector normal;
    vector endpoint;
    vector motion_direction;
    float distance_moved;
    float motion_velocity;
    bool isAnimated;
} scene_item;

typedef enum light_type{spot=0, point=1, parallel=3} light_type;

typedef struct light_items{
    vector position;
    color light_color;
    float radial_a0;
    float radial_a1;
    float radial_a2;
    float theta; // spot lights only
    float angular_a0; //spot lights only 
    vector light_direction; //spot lights only
    light_type type;
}light;

typedef struct scene{
    scene_item items[128];
    light lights[128];
    size_t scene_size;
    size_t light_size;
}scene;

typedef struct camera{
    vector position;
    float width;
    float height;
    int fps;
    float duration;
}camera;

