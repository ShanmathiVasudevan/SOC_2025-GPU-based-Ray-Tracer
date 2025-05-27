#include <iostream>
int main() {
    int image_width = 256;
    int image_height = 256;
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (int i = 0; i < image_height; i++) {
        for (int j = 0; j < image_width; j++) {
            auto r = 0.0;
            if (i<=2*image_height/3){
                r = double(image_height - 3*i/2 - 1) / (image_height-1);
            }
            auto g = 0.0;
            if (i< image_height/2){
                g = double(2*i) / (image_height-1);
            }
            else if (i >= image_height/2){
                g = double(2*(image_height - i))/ (image_height - 1);
            }
            auto b = 0.0;
            if (i > image_height/3){
                b = double(3*(i-image_height/3)/2) / (image_height-1);
            }
            int ir = int(255.999 * r);
            int ig = int(255.999 * g);
            int ib = int(255.999 * b);
            std::cout << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }
}