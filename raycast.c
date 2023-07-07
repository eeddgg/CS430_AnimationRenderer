#include "raycast.h"
#include <math.h>
size_t elements =0;
scene working_scene;
camera current_camera;
bool verbose=false;
vector ZERO_VECTOR = {0.0f, 0.0f, 0.0f};
size_t MAX_DEPTH =6;
float animation_t=0.0;




//refactored: returns the t, and scale the ray direction vector by t to get the distance between camera and sphere
float sphere_intersect(scene_item sphere, vector v, vector head){
    vector norm_v;
    v3_normalize(norm_v, v);
    float t;
    float t_2;


    double a = pow(v[0], 2)+ pow(v[1], 2) + pow(v[2],2);
    double b = 2*((v[0])*(head[0]-sphere.position[0])+ v[1]*(head[1]-sphere.position[1])+ v[2]*(head[2]-sphere.position[2]));
    double c = pow((head[0]-sphere.position[0]),2)+pow((head[1]-sphere.position[1]),2)+pow((head[2]-sphere.position[2]),2)-pow(sphere.radius, 2);
    double delta = pow(b,2)-4*a*c;


    if(fpclassify(delta)==FP_SUBNORMAL&&delta>1e-6){
        return (float) INFINITY;
    }
    else if(fabs(delta-0)<1e-6){
        t = -b/(2*a);
        return t;
    }
    else{
        t=(-b+sqrt(delta))/(2*a);
        t_2=(-b-sqrt(delta))/(2*a);

        // t is closest and in front of the camera
        if(t<=t_2 && t>0){
            return t;
        }
        // t_2 is the closest intersection in front of the camera
        else if(t_2>0){
            return t_2;
        }

        //no intersection
        else{
            return (float) INFINITY;
        }
    }
}

bool isOrthogonal(vector in){
    vector inn;
    v3_normalize(inn, in);
    return v3_equals(inn, (vector){0,0,1}, 1e-6) || v3_equals(inn, (vector){0,0,-1}, 1e-6)||v3_equals(inn, (vector){0,-1,0}, 1e-6) || v3_equals(inn, (vector){0,1,0}, 1e-6)||v3_equals(inn, (vector){-1,0,0}, 1e-6) || v3_equals(inn, (vector){1,0,0}, 1e-6);
}


// returns the t value that the direction vector is scaled by to get the distance
float plane_intersect(scene_item plane, vector direction, vector head){
    if(!isOrthogonal(plane.normal)){
        v3_scale(plane.normal, -1.0f);}
    vector delta;


    vector unit_normal={0,0,0};
    v3_normalize(unit_normal, plane.normal);

    
    v3_subtract(delta, plane.position, head);
    if(fabs(v3_dot_product(direction, plane.normal))<1e-6){
        return INFINITY;
    }
    float t = ((v3_dot_product(unit_normal, delta)))/v3_dot_product(unit_normal, direction);
    if(!isOrthogonal(plane.normal)){
        v3_scale(plane.normal, -1.0f);}
    return t;
}

void move_sphere(float t, camera c, scene_item * sphere){
    vector newPosition;
    vector scaled;
    scaled[0]=sphere->motion_direction[0]; scaled[1]=sphere->motion_direction[1]; scaled[2]=sphere->motion_direction[2];
    if(sphere->isAnimated){
        float scale_factor=t*((sphere->motion_velocity*sphere->distance_moved)/(c.duration*c.fps));
        v3_scale(scaled, scale_factor);
        if(sphere->distance_moved-scale_factor>1e-6){
            v3_add(sphere->position, sphere->position, scaled);
        }
    }
    
}

void parse_scene(FILE * fileptr){
    working_scene.light_size=0;
    working_scene.scene_size=0;
    char temp_str[255];
    char * camera_start;
    char * sphere_start;
    char * plane_start;
    char * light_start;
    char * sphere_position;
    while(!feof(fileptr) & (working_scene.scene_size<128)){
        camera_start=NULL;
        plane_start=NULL;
        sphere_start=NULL;
        light_start=NULL;
        fgets(temp_str, 255, fileptr);
        plane_start=strstr(temp_str, "plane");
        camera_start=strstr(temp_str, "camera");
        sphere_start=strstr(temp_str, "sphere");
        light_start=strstr(temp_str, "light");
        
        

        if(camera_start){
            current_camera.position[0]=0.0f;
            current_camera.position[1]=0.0f;
            current_camera.position[2]=0.0f;
            char* camera_width=strstr(temp_str, "width");
            char* camera_height=strstr(temp_str, "height");
            char* camera_fps=strstr(temp_str, "fps");
            char* camera_duration=strstr(temp_str, "duration");
            if(camera_width){
                sscanf(camera_width, "width: %f", &current_camera.width);
            }
            else{
                current_camera.height=0.0f;
            }
            if(camera_height){
                sscanf(camera_height, "height: %f", &current_camera.height);
            }
            else{
                current_camera.height=0.0f;
            }
            if(camera_fps){
                sscanf(camera_fps, "fps: %d", &current_camera.fps);
            }
            else{
                current_camera.fps=1.0f;
            }
            if(camera_duration){
                sscanf(camera_duration, "duration: %f", &current_camera.duration);
            }
            else{
                current_camera.duration=1.0f;
            }
        }

        else if(light_start){
            char* radial_a0=strstr(light_start, "radial-a0");
            char* radial_a1=strstr(light_start, "radial-a1");
            char* radial_a2=strstr(light_start, "radial-a2");
            char* theta=strstr(light_start, "theta");
            char* angular_a0=strstr(light_start, "angular-a0");
            char* direction=strstr(light_start, "direction");
            char* position=strstr(light_start, "position");
            char* color = strstr(light_start, "color");

            light item;

            if(position){
                sscanf(position, "position: [%f, %f, %f]", item.position, item.position+1, item.position+2);

            }
            else{
                item.position[0]=0.0f;
                item.position[1]=0.0f;
                item.position[2]=0.0f;
            }

            if(color){
                sscanf(color, "color: [%f, %f, %f]", &item.light_color.red, &item.light_color.green, &item.light_color.blue);
            }

            if(direction)
            {
                sscanf(direction, "direction: [%f, %f, %f]", &item.light_direction[0], &item.light_direction[1], &item.light_direction[2]);
                item.type = spot;
            }
            else{
                item.type=point;
            }

            if(radial_a0){
                sscanf(radial_a0, "radial-a0: %f", &item.radial_a0);
            }
            else{
                item.radial_a0=0.0f;
            }

            if(radial_a1){
                sscanf(radial_a1, "radial-a1: %f", &item.radial_a1);
            }
            else{
                item.radial_a1=0.0f;
            }

            if(radial_a2){
                sscanf(radial_a2, "radial-a2: %f", &item.radial_a2);
            }
            else{
                item.radial_a2=0.0f;
            }

            if(theta){
                sscanf(theta, "theta: %f", &item.theta);
                //convert to radians
                item.theta= item.theta * (PI_a/180.0);
            }

            if(angular_a0){
                sscanf(angular_a0, "angular-a0: %f", &item.angular_a0);
            }
            working_scene.lights[working_scene.light_size]=item;
            working_scene.light_size+=1;            
        }

        else if(sphere_start)
        {
            char* sphere_diffuse_color=strstr(temp_str, "diffuse_color");
            char* sphere_specular_color=strstr(temp_str, "specular_color");
            sphere_position=strstr(temp_str, "position");
            char* sphere_radius=strstr(temp_str, "radius");
            char* reflectivity = strstr(temp_str, "reflectivity");
            char* animated=strstr(temp_str, "animated");
            char* end_position=strstr(temp_str, "endpoint");
            char* velocity=strstr(temp_str, "velocity");
            scene_item item;

            char* color = strstr(temp_str, " color");
            if (color==NULL){
                color=strstr(temp_str, ",color");
            }

            if(color){
                sscanf(color, "color: [%f, %f, %f]", &item.ambient_color.red, &item.ambient_color.green, &item.ambient_color.blue);
            }
            else{
                item.ambient_color.red=0.0f;
                item.ambient_color.green=0.0f;
                item.ambient_color.blue=0.0f;
            }

            if(sphere_diffuse_color){
                sscanf(sphere_diffuse_color, "diffuse_color: [%f, %f, %f]", &item.diffuse_color.red, &item.diffuse_color.green, &item.diffuse_color.blue);
            }
            else
            {
                item.diffuse_color.red=0.0f; 
                item.diffuse_color.green=0.0f; 
                item.diffuse_color.blue=0.0f;
            }
            if(sphere_specular_color){
                sscanf(sphere_specular_color, "specular_color: [%f, %f, %f]", &item.specular_color.red, &item.specular_color.green, &item.specular_color.blue);
            }
            else
            {
                item.specular_color.red=0.0f; 
                item.specular_color.green=0.0f; 
                item.specular_color.blue=0.0f;
            }

            if(sphere_position!=NULL){
                sscanf(sphere_position, "position: [%f, %f, %f]", &item.position[0], &item.position[1],&item.position[2]);
                sscanf(sphere_position, "position: [%f, %f, %f]", &item.start_position[0], &item.start_position[1], &item.start_position[2]);
            }
            else
            {
                item.position[0]=0.0f;
                item.position[1]=0.0f;
                item.position[2]=0.0f;
            }

            if(end_position){
                item.isAnimated=true;
                sscanf(end_position, "endpoint: [%f, %f, %f]", &item.endpoint[0], &item.endpoint[1],&item.endpoint[2]);
                vector temp;
                v3_from_points(temp, item.start_position, item.endpoint);
                v3_normalize(item.motion_direction, temp);
                item.distance_moved=v3_length(temp);
            }
            else{
                item.isAnimated=false;
            }

            if(velocity){
                sscanf(velocity, "velocity: %f", &item.motion_velocity);
            }
            else{
                item.motion_velocity=1.0f;
            }

            if(sphere_radius!=NULL){
                sscanf(sphere_radius, "radius: %f", &item.radius);
            }
            else{
                item.radius=0.0f;
            }

            if(reflectivity!=NULL){
                sscanf(reflectivity, "reflectivity: %f", &item.reflectivity);
            }
            else{
                item.reflectivity=0.0f;
            }

            item.type=Sphere;
            working_scene.items[working_scene.scene_size]=item;            
            working_scene.scene_size++;
        }
        else if(plane_start){
            scene_item item;
            char * plane_normal=strstr(temp_str, "normal");
            char * plane_specular_color=strstr(temp_str, "specular_color");
            char * plane_diffuse_color=strstr(temp_str, "diffuse_color");
            char * plane_position=strstr(temp_str, "position");
            char* reflectivity = strstr(temp_str, "reflectivity");

            char* color = strstr(temp_str, " color");
            if (color==NULL){
                color=strstr(temp_str, ",color");
            }

            if(color!=NULL){
                sscanf(color, "color: [%f, %f, %f]", &item.ambient_color.red, &item.ambient_color.green, &item.ambient_color.blue);
            }
            else{
                item.ambient_color.red=0.0f;
                item.ambient_color.green=0.0f;
                item.ambient_color.blue=0.0f;
            }

            if(plane_specular_color!=NULL){
                sscanf(plane_specular_color, "color: [%f, %f, %f]", &item.specular_color.red, &item.specular_color.green, &item.specular_color.blue);
                
            }
            else{
                item.specular_color.red=0.0f; 
                item.specular_color.green=0.0f; 
                item.specular_color.blue=0.0f;
            }

            if(plane_diffuse_color!=NULL){
                sscanf(plane_diffuse_color, "diffuse_color: [%f, %f, %f]", &item.diffuse_color.red, &item.diffuse_color.green, &item.diffuse_color.blue);
                
            }
            else{
                item.diffuse_color.red=0.0f; 
                item.diffuse_color.green=0.0f; 
                item.diffuse_color.blue=0.0f;
            }

            if(plane_position){
                sscanf(plane_position, "position: [%f, %f, %f]", &item.position[0], &item.position[1],&item.position[2]);
            }
            else
            {
                item.position[0]=0.0f;
                item.position[1]=0.0f;
                item.position[2]=0.0f;
            }

            if(plane_normal){
                sscanf(plane_normal, "normal: [%f, %f, %f]", &item.normal[0], &item.normal[1], &item.normal[2]);
                
            }
            else
            {
                item.normal[0]=0.0f;
                item.normal[1]=0.0f;
                item.normal[2]=0.0f;
            }
            
            if(reflectivity!=NULL){
                sscanf(reflectivity, "reflectivity: %f", &item.reflectivity);
            }
            else{
                item.reflectivity=0.0f;
            }

            item.type=Plane;
            working_scene.items[working_scene.scene_size]=item;            
            working_scene.scene_size++;
        }
    }
}

pixel color_to_pixel(color color_val){
    pixel out;
    if(color_val.red>1.0f){
        color_val.red=1.0f;
    }
    if(color_val.green>1.0f){
        color_val.green=1.0f;
    }
    if(color_val.blue>1.0f){
        color_val.blue=1.0f;
    }
    out.red=(uint8_t)floorf(color_val.red * 255);
    out.green=(uint8_t)floorf(color_val.green *255);
    out.blue=(uint8_t)floorf(color_val.blue * 255);
    return out;
}

long long min_index(float* array, size_t array_size){
    size_t index=0;
    float min_item=INFINITY;
    size_t minIndex=-1;
    for(index=0; index<array_size; index++){
        if(array[index]<min_item&&array[index]>0){
            minIndex=index;
            min_item= array[index];
        }
    }
    return minIndex;
}


bool shadow(light light_in, vector head, vector hitPoint, size_t objIndex)
{
 vector shadowVector;
 vector normalized_shadow_vector;
 
 v3_from_points(shadowVector, hitPoint, light_in.position);
 v3_normalize(normalized_shadow_vector, shadowVector);
 float shadow_t = v3_length(shadowVector);
 
 size_t scene_items;
 float scene_t[128];

   {
    for(scene_items=0; scene_items<working_scene.scene_size; scene_items++){
        if(working_scene.items[scene_items].type==Sphere){
            scene_t[scene_items]=sphere_intersect(working_scene.items[scene_items],normalized_shadow_vector, hitPoint);
        }
        if(working_scene.items[scene_items].type==Plane){
            scene_t[scene_items]=plane_intersect(working_scene.items[scene_items], hitPoint, normalized_shadow_vector);
        }
        if (scene_t[scene_items]<shadow_t && fabsf(scene_t[scene_items]-shadow_t)>1e-4 && scene_t[scene_items]>0.0f&&scene_items!=objIndex){
            if(verbose){
            printf("item_t: %f, light_t: %f, item_number: %zu ", shadow_t, scene_t[scene_items], scene_items);}
                return true;
        }

    }
   }
  return false;   
}

color diffuseLight(light LightVector, scene_item surface, vector location, size_t obj_index)
{
 vector L;
 vector N;
 vector n;
 vector l;

 color output;
 if(surface.type == Plane){
    v3_normalize(N, surface.normal);
 }
 else{
    v3_from_points(n, surface.position, location);
    v3_normalize(N, n);
    
 }
 v3_from_points(l, location, LightVector.position);
 v3_normalize(L, l);

 bool shade= shadow(LightVector, current_camera.position, location,obj_index);
if (verbose){
printf("shadow=%d ",  shade);}
//check if dot product less than or equal to 0 and if the light source actually hits the point
if ((v3_dot_product(N,L) > -0.0f &&!shade) )
  {
// calculate diffuse value
    output.red=v3_dot_product(N,L)*LightVector.light_color.red;
    output.green=v3_dot_product(N,L)*LightVector.light_color.green;
    output.blue=v3_dot_product(N,L)*LightVector.light_color.blue;
 }
else{
    output = (color) {
              .red=0.0f, 
              .green=0.0f,
              .blue=0.0f
             };
}
return output;
 
}

color specLight(light Light, camera Cam, scene_item item, vector hitPoint, size_t obj_index)
{
vector reflected;
float specValue;
vector L;
vector l;
vector N;
vector View;
vector V;
vector R;
float specExponent=20.0f;

color intensity={0.0f, 0.0f, 0.0f};

v3_from_points(View, Cam.position, hitPoint);

v3_normalize(V, View);

if(item.type==Plane){
    v3_normalize(N, item.normal);
}
else{
    v3_from_points(N, item.position, hitPoint);
    //v3_add(N, N, hitPoint);
    v3_normalize(N, N);
}



v3_from_points(l, hitPoint, Light.position);

v3_normalize(L,l);

v3_reflect(reflected, L, N);
v3_normalize(R,reflected);
if(verbose){
printf("V.R=%.2f ", v3_dot_product(V, R));}
bool shade=shadow(Light, Cam.position, hitPoint, obj_index);
if (shade&&verbose){printf(" No Specular, item in shadow, ");}
if(v3_dot_product(V, R) > -0.0f && v3_dot_product(N, L) > 0&&!shade)
 {
  specValue = powf(v3_dot_product(V, R), specExponent);
  if(verbose){
    printf("spec= %f", specValue);
  }
  intensity.red=Light.light_color.red*specValue;
  intensity.green=Light.light_color.green*specValue;
  intensity.blue=Light.light_color.blue*specValue;
 }

 return intensity;
}

// I believe you would need the vectors to show the angle allowed as parameters
// to allow for changing of how the spot light drops off
// need to make work as a derivative
float angAtten(light Light, vector hitPoint, scene_item object )
{
    if(Light.type!=spot){
        return 1;
    }
    vector norm_direction;
    vector light_hitPoint;
    v3_from_points(light_hitPoint, hitPoint, Light.position);
    v3_normalize(light_hitPoint, light_hitPoint);
    

    v3_normalize(norm_direction, Light.light_direction);

    float cosAlpha = v3_angle_quick(norm_direction, light_hitPoint);
    if(cosAlpha>cos(Light.theta/2)){
        return 0;
    }
    else{
        return powf(v3_dot_product(norm_direction, light_hitPoint), Light.angular_a0);
    }
}

float radialAtten(vector camera_hitpoint, light light_in)
{
 float radValue;
 float distance;
 float distanceSquared;
 vector L;
 v3_from_points(L, camera_hitpoint, light_in.position);
 
 distance = v3_length(L);
 
 distanceSquared = distance * distance;

 radValue = (1/( light_in.radial_a0 + (light_in.radial_a1 * distance) + (light_in.radial_a2 * distanceSquared)));

 return radValue;
}

color colorScale(color base, color intensity){
    color out;
    out.red = base.red * intensity.red;
    out.green =base.green * intensity.green;
    out.blue = base.blue * intensity.blue;
    return out;
}

color colorShift(color in, float i){
    color out;
    out.red=in.red*i;
    out.green=in.green*i;
    out.blue=in.blue*i;
    return out;
}

color colorSum(color a, color b){
    color out;
    out.red=a.red+b.red;
    out.green=a.green+b.green;
    out.blue=a.blue+b.blue;
    return out;
}

color colorShoot(vector normalVector, scene scene, vector head, size_t depth, long long int last_hit)
{
    color fallback = {0.0f,0.0f,0.0f};

    if (verbose == true)
    {
    printf("shooting along [%f, %f, %f]\n", normalVector[0],normalVector[1],normalVector[2]);
    }
    int index = 0;
    float intersectValue;
    float t_vals[128];
    long long minIndex;
    vector bounce;
    vector itemNormal;
    color illuminated={0.0f, 0.0f, 0.0f};
    color spec_i;
    color diff_i;
    color spec;
    color diff;
    color sum;
    color scaled;
    color reflected;
    float atten;
    float angatt;
    float rad_atten;

    while(index < scene.scene_size){
    // find color of a plane
    if(scene.items[index].type == Plane){
            intersectValue = plane_intersect(scene.items[index], normalVector, head);
            if(verbose){
            printf("\tindex:%d type: plane, t=%f, Position=[%f,%f,%f], diffuse_Color=[%f,%f,%f]\n", index, intersectValue, scene.items[index].position[0], 
                scene.items[index].position[1], scene.items[index].position[2], scene.items[index].diffuse_color.red, scene.items[index].diffuse_color.green, scene.items[index].diffuse_color.blue);}
            t_vals[index]=intersectValue;
    }
    // find color of a sphere
        else if(scene.items[index].type == Sphere){
            intersectValue = sphere_intersect(scene.items[index], normalVector, head);
            if (verbose){
            printf("\tindex:%d type: sphere, t=%f, radius=%f, Position=[%f,%f,%f]\n", index,
                intersectValue,scene.items[index].radius,scene.items[index].position[0], 
                scene.items[index].position[1], scene.items[index].position[2]);}
            t_vals[index]=intersectValue;
            
        }
        index++;
    }
  
    minIndex=min_index(t_vals, scene.scene_size);

    if(minIndex>-1&&minIndex==last_hit){
        t_vals[minIndex]=-1.0f;
        minIndex=min_index(t_vals, scene.scene_size);
    }
  
  
    if(minIndex>-1 && depth<=MAX_DEPTH){
        intersectValue=t_vals[minIndex];
        float * hitPoint;
            hitPoint= (vector) {normalVector[0], normalVector[1], normalVector[2]};
        v3_normalize(hitPoint, hitPoint);
        v3_scale(hitPoint, intersectValue);

        if(scene.items[minIndex].type==Sphere){
            v3_from_points(itemNormal, hitPoint, scene.items[index].position);
            v3_normalize(itemNormal, itemNormal);
            v3_reflect(bounce, hitPoint, itemNormal);
            v3_scale(bounce, -1.0f);
            
        }
        else if(scene.items[minIndex].type==Plane){
            v3_from_points(itemNormal, ZERO_VECTOR, scene.items[minIndex].normal);
            v3_normalize(itemNormal, itemNormal);
            v3_reflect(bounce, hitPoint, itemNormal);
        }
        
        v3_normalize(bounce, bounce);
        

        for(index=0; index<scene.light_size; index++){
            if(verbose){
            printf("light_color: [%f,%f,%f], ", scene.lights[index].light_color.red, scene.lights[index].light_color.green, scene.lights[index].light_color.blue);
            }
            diff_i=diffuseLight(scene.lights[index], scene.items[minIndex], hitPoint, minIndex);
            spec_i=specLight(scene.lights[index], current_camera, scene.items[minIndex], hitPoint, minIndex);
            if(verbose){
                printf("diffuse_i: [%f,%f,%f]; spec_i: [%f,%f,%f]\n", diff_i.red, diff_i.green, diff_i.blue, spec_i.red, spec_i.green, spec_i.blue);
            }
            diff=colorScale(scene.items[minIndex].diffuse_color, diff_i);
            spec= colorScale(scene.items[minIndex].specular_color, spec_i);
            
            angatt=angAtten(scene.lights[index], hitPoint, scene.items[minIndex]);

            rad_atten=radialAtten(hitPoint, scene.lights[index]);
            if(verbose){
                printf("ang:%f, rad:%f item: ", angatt, rad_atten);
            }
            atten= rad_atten*angatt;
            sum= colorSum(diff, spec);
            scaled= colorShift(sum, atten);
            illuminated= colorSum(illuminated, scaled);

            if(verbose){
                printf("%lld\n", minIndex);
            }
        }

        reflected=colorShift(colorShoot(bounce, scene, hitPoint, depth+1, minIndex),scene.items[minIndex].reflectivity);


        illuminated=colorSum(illuminated, reflected);
    
    
        
        return illuminated;
    }
    else{

        return fallback;
    }
}



int main(int argc, char ** argv){
    char filename[88];
    if(argc<5){
        printf("ERROR: insufficient arguments\n raytrace <width> <height> <scene> <output>\n");
        return 1;
    }
    int image_width= atoi(argv[1]);
    int image_height=atoi(argv[2]);
    FILE * scene_file=fopen(argv[3], "r+");
    header out_header=make_header(3, 255, image_width, image_height);
    parse_scene(scene_file);
    fclose(scene_file);
    for(int frame=0; frame<(int)(current_camera.duration*current_camera.fps); frame++){
        // moves the spheres ahead of the next render
        for(int itemIndex=0; itemIndex<=working_scene.scene_size; itemIndex++){
            if(working_scene.items[itemIndex].type==Sphere){
                move_sphere((frame/(current_camera.duration*current_camera.fps)), current_camera, &working_scene.items[itemIndex]);
            }
        }        
        
        snprintf(filename, 88, "%s_frame%08d.ppm", argv[4], frame);
        pixel * pixmap=make_pixmap(out_header);
        image out;
        out.metadata=out_header;
        out.pixmap=pixmap;
        vector v, norm_v;
        vector head;
        // gotta figure out how to find the head value
        // no you don't it's the direction of the ray + the tail
        head[0] = current_camera.position[0];
        head[1] = current_camera.position[1];
        head[2] = current_camera.position[2];
        v[2]=-1;
        vector center={0,0,-1};
        float pixel_height= current_camera.height/image_height;
        float pixel_width=current_camera.width/image_width;

        size_t image_index=(image_width*image_height)-1;
        for (size_t i = 0; i < image_height; i++)
        {
            v[1]=center[1]-current_camera.height/2 + pixel_height * (i+0.5);
            for (size_t j = image_width; j >0; j--)
            {
                v[0]=center[0]-current_camera.width/2 + (pixel_width*(j-0.5) );
                v3_normalize(norm_v, v);
                out.pixmap[image_index]= color_to_pixel(colorShoot(norm_v, working_scene, head,0,-1));
                image_index--;
            }
            
        }
        write_image(out, filename);
        free_image(out);
        

    }
    return 0;
} 
