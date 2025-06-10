#ifndef VEC3_H
#define VEC3_H
#include<iostream>
#include<cmath>
class Vec3{
    private:
    double x;
    double y;
    double z;
    public:
    Vec3() {x=0;y=0;z=0;}
    Vec3(double x, double y, double z) : x{x}, y{y}, z{z} {}
    double operator[](int i) const { 
        if (i==0){
            return x;
        }
        if (i==1){
            return y;
        }
        if (i==2){
            return z;
        }
        double c = 0;
        return c;
    }
    double& operator[](int i) { 
        if (i==0){
            return x;
        }
        if (i==1){
            return y;
        }
        if (i==2){
            return z;
        }
        double c = 0;
        return c;
    }
    Vec3 operator+ (const Vec3 & v) {
        Vec3 result(v.x+this->x,v.y+this->y,v.z+this->z);
        return result;
    }
    Vec3 operator- (const Vec3 & v) {
        Vec3 result(-v.x+this->x,-v.y+this->y,-v.z+this->z);
        return result;
    }
    Vec3 operator* (const double & a) {
        Vec3 result(a*(this->x),a*(this->y),a*(this->z));
        return result;
    }
    Vec3 operator/ (const double & a) {
        Vec3 result((this->x)/a,(this->y)/a,(this->z)/a);
        return result;
    }
    Vec3 operator- (){
        Vec3 result(-this->x, -this->y, -this->z);
        return result;
    }

    double length(){
        return sqrt(x*x+y*y+z*z);
    }
    Vec3 unit(){
        Vec3 result = *this/this->length();
        return result;
    }
    double dot(const Vec3& other){
        return x*other.x+y*other.y+z*other.z;
    }
    Vec3 cross(const Vec3& other){
        Vec3 result(0,0,0);
        result.x = (this->y)*other.z-(this->z)*other.y;
        result.y = (this->z)*other.x-(this->x)*other.z;
        result.z = (this->x)*other.y-(this->y)*other.x;
        return result;
    }
    Vec3 reflect(Vec3& normal){
        return normal*(-2*((this->dot(normal))/(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z))) + *this;
    }
    Vec3 refract(const Vec3& normal, double refractive_index){
        double i = acos(-1*((this->unit()).dot(normal))/sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z));
        double r = asin(sin(i)*refractive_index);
        double theta = r - i;
        return *this*cos(theta) + (this->cross(normal)).cross(*this)*sin(theta)+(this->cross(normal))*((this->cross(normal)).dot(*this))*(1-cos(theta));
    }
};
#endif