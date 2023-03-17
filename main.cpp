#include "readfile.h"
#include "Image.h"

void saveScreenshot(string fname, Scene scene) {
  int pix = scene.size.x * scene.size.y;
  BYTE* pixels = new BYTE[3*pix];	//ray tracer

  FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, scene.size.x, scene.size.y, scene.size.x * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);

  std::cout << "Saving screenshot: " << fname << "\n";

  FreeImage_Save(FIF_PNG, img, fname.c_str(), 0);
  delete[] pixels;
}

int main(int argc, char* argv[]){
    FreeImage_Initialise();
    Scene scene;
    string output = readfile(argv[1], scene);
    if(output.length() == 0){ //no output specified
        output = "raytrace.png";
    }

    saveScreenshot(output, scene);
    FreeImage_DeInitialise();
    return 0;
}