#ifndef VEC3_H
#define VEC3_H
#include<iostream>
#include<sycl/sycl.hpp>
#include<cmath>
class Vec3{
    private:
    float x;
    float y;
    float z;
    public:
    Vec3() {x=0.0f;y=0.0f;z=0.0f;}
    Vec3(float x, float y, float z) : x{x}, y{y}, z{z} {}
    float operator[](int i) const { 
        if (i==0){
            return x;
        }
        if (i==1){
            return y;
        }
        if (i==2){
            return z;
        }
        float c = 0.0f;
        return c;
    }
    float& operator[](int i) { 
        if (i==0){
            return x;
        }
        if (i==1){
            return y;
        }
        if (i==2){
            return z;
        }
        return z;
    }
    Vec3 operator+ (const Vec3 & v) {
        Vec3 result(v.x+this->x,v.y+this->y,v.z+this->z);
        return result;
    }
    Vec3 operator- (const Vec3 & v) {
        Vec3 result(-v.x+this->x,-v.y+this->y,-v.z+this->z);
        return result;
    }
    Vec3 operator- (const Vec3 & v) const {
        Vec3 result(-v.x+this->x,-v.y+this->y,-v.z+this->z);
        return result;
    }
    Vec3 operator* (const float & a) {
        Vec3 result(a*(this->x),a*(this->y),a*(this->z));
        return result;
    }
    Vec3 operator/ (const float & a) {
        Vec3 result((this->x)/a,(this->y)/a,(this->z)/a);
        return result;
    }
    Vec3 operator- (){
        Vec3 result(-this->x, -this->y, -this->z);
        return result;
    }

    float length(){
        return sycl::sqrt(x*x+y*y+z*z);
    }
    Vec3 unit(){
        Vec3 result = *this/this->length();
        return result;
    }
    float dot(const Vec3& other){
        return x*other.x+y*other.y+z*other.z;
    }
    Vec3 cross(const Vec3& other){
        Vec3 result(0.0f,0.0f,0.0f);
        result.x = (this->y)*other.z-(this->z)*other.y;
        result.y = (this->z)*other.x-(this->x)*other.z;
        result.z = (this->x)*other.y-(this->y)*other.x;
        return result;
    }
    Vec3 reflect(Vec3& normal){
        return normal*(-2.0f*((this->dot(normal))/(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z))) + *this;
    }
    Vec3 refract(const Vec3& normal, float refractive_index){
        float i = sycl::acos(-1.0f*((this->unit()).dot(normal))/sycl::sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z));
        float r = sycl::asin(sycl::sin(i)*refractive_index);
        float theta = r - i;
        return *this*sycl::cos(theta) + (this->cross(normal)).cross(*this)*sycl::sin(theta)+(this->cross(normal))*((this->cross(normal)).dot(*this))*(1.0f-sycl::cos(theta));
    }
};
#endif