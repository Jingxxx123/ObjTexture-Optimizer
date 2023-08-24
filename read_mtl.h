//#pragma once //protects against multiple inclusions of this header file

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <list>
#include <sstream>
#include <jpeglib.h>
#include <cstring>
#include <climits>

using namespace std;

class Coordinate{
    public:
        std::vector<double> points;
        std::vector<double> org_points;
        std::string line;
        std::string usemtl;
    
};

class Face{
    public:
        std::vector<int> vt_indexs;
        std::list<int> new_vt_indexes;
        std::vector<Coordinate> coords;
        std::string line;
        bool positive = true;
        
    
};

class ImageInfo{
    public:
        std::string filename;
        int width;
        int height; 
        int channel; 
        std::vector<uint8_t> pixels;   
        std::vector<double> uv_points;
        std::vector<int> points;

        //ImageInfo(int w, int h) : width(w), height(h) {}                             
};

class Material{
    public:
        std::string usemtl;
        std::list<Face> faces;
        std::string newmtl;
        std::list<std::string> value; 
        bool combine = true;    
        std::string image; 
        ImageInfo image_info;
        std::string groupname;

    
    
};

class Group{
    public:
        std::string groupname;
        std::list<std::string> usemtls;
        std::list<std::string> faces;
};

class Property{
    public:
        std::string name;
        std::list<double> data;
        std::string map_data;

    bool operator==(const Property& other) const {
        if(name == other.name && data.size() == other.data.size()){
            auto it1 = data.begin();
            auto it2 = other.data.begin();
            while(it1 != data.end() && it2 != other.data.end()){
                if(*it1 != *it2){
                    return false;
                }
                ++it1;
                ++it2;
            }
            return true;
        }else{
            return false;
        }
    }

    bool operator!=(const Property& other) const{
        return !(*this == other);
    }
};

class Properties{
    public:
        // std::list<std::string> properties;
        std::list<Property> properties;
        std::vector<ImageInfo*> images;
        std::string first_image;
        std::string first_newmtl;
        std::list<std::string> usemtls;
        std::list<std::string> faces;
        std::string groupname;
        std::string combine_mtl_name;
};

bool loadPNG(const std::string& path, std::string filename, ImageInfo& imageInfo);
void generateSpriteSheet(std::vector<ImageInfo*>& images_ptr, const std::string& path, std::string output_file);
bool sortImages(const ImageInfo* a, const ImageInfo* b);
bool createCombinedImage(const std::string& path, std::string& filename, /*std::list<std::string>& uncombined_usemtl,*/ std::unordered_map<std::string, Material>& all_usemtl, std::list<Properties>& combined_image_info);
bool convertJPEGtoPNG(const std::string& path, std::string jpegFilename, std::string pngFilename);