#include "v3math.h"

void v3_from_points (float *dst, float *a, float *b){
    float  newVector[3];
    newVector[0]=b[0]-a[0];
    newVector[1]=b[1]-a[1];
    newVector[2]=b[2]-a[2];

    for(int i=0; i<3; i++){
        dst[i]=newVector[i];
    }
}

void v3_add(float *dst, float *a, float *b){
    float temp[3];

    int i;
    for(i=0; i<3; i++){
        temp[i]=a[i]+b[i];
    }
    for(i=0; i<3; i++){
        dst[i]=temp[i];
    }
}

void v3_subtract(float *dst, float *a, float *b){
    float temp[3];

    int i;
    for(i=0; i<3; i++){
        temp[i]=a[i]-b[i];
    }
    for(i=0; i<3; i++){
        dst[i]=temp[i];
    }
}

float v3_dot_product(float *a, float *b){
    int i;
    float dot=0.0f;
    for(i=0;i<3;i++){
        dot+=(a[i]*b[i]);
    }
    return dot;    
}

void v3_cross_product(float *dst, float *a, float *b){
    float temp[3];
    temp[0]=(a[1]*b[2])-(a[2]*b[1]);
    temp[1]=(a[2]*b[0])-(a[0]*b[2]);
    temp[2]=(a[0]*b[1])-(a[1]*b[0]);
    int i;
    for(i=0; i<3; i++){
        dst[i]=temp[i];
    }
}

void v3_scale(float *dst, float s){
    int i=0;
    for(i=0; i<3; i++){
        dst[i]*=s;
    }
}

float v3_angle(float *a, float *b){
    return acos((v3_dot_product(a,b))/(v3_length(a) * v3_length(b)));
}

float v3_angle_quick(float *a, float *b){
    return ((v3_dot_product(a,b))/(v3_length(a) * v3_length(b)));
}

void v3_reflect(float *dst, float  *v, float *n){
    float temp[3];
    float norm_n[3];
    v3_normalize(norm_n, n);

    float nscale = (v3_dot_product(norm_n,v));
    int i;
    v3_scale(n, 2.0f*nscale);
    v3_subtract(temp, v, n);
    for(i=0; i<3; i++){
        dst[i]=temp[i];
    }
    v3_scale(n, 1/(2.0f*nscale));

}

float v3_length(float *a){
    float length = sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
    return length;
}

void v3_normalize(float *dst, float *a){
    float length = v3_length(a);
    float temp[3];
    int i;
    for(i=0; i<3; i++){
        temp[i]=a[i]/length;
    }

    for(i=0; i<3; i++){
        dst[i]=temp[i];
    }
}


bool v3_equals(float *a, float *b, float tolerance){
    bool x_match= fabs(a[0]-b[0])<=tolerance;
    bool y_match=fabs(a[1]-b[1])<=tolerance;
    bool z_match= fabs(a[2]-b[2])<=tolerance;
    return x_match && y_match && z_match;

}

