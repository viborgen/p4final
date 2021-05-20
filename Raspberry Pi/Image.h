#include <fstream>
#include <vector>
#ifndef IMAGE_H
#define IMAGE_H

//Defineres til at indsætte color til Image.
struct Color {
    volatile float r, g, b;
    Color();
    Color(volatile float r, volatile float g, volatile float b);
    ~Color(); 
};

//Benyttes til at oprette billede. Anvender funktioner fra Image.cpp
class Image{
public:
    Image(volatile int width, volatile int height);
    ~Image();

    Color GetColor(volatile int x, volatile int y) const; //Farveplacering
    void SetColor(const Color& color, int x, int y); //Sætter farven

    void Export(const char* path) const; //Generere filen

private:
    volatile int m_width;
    volatile int m_height;
    std::vector<Color> m_colors; 
};
#endif
