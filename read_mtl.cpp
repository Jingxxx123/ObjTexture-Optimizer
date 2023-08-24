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
#include "read_mtl.h"
#include <png.h>
#include <cstdio>


bool isJPEGImage(const std::string& filename);

bool createCombinedImage(const std::string& path, std::string& filename, /*std::list<std::string>& uncombined_usemtl,*/ std::unordered_map<std::string, Material>& all_usemtl, std::list<Properties>& combined_image_info){

    std::string myText;


    Material* newmtl_ptr;
    
    //std::list<std::string> temp_file_txt;
    std::list<Property> temp_file_txt;
    
    bool new_properties = true;
    Properties* exist_prop;

    ImageInfo* temp_image;
    std::list<std::string> temp_png;
    std::string temp_newmtl;
    std::string temp_usemtl;
    std::string temp_new_image;
    bool found_image = false;
    
    newmtl_ptr = &all_usemtl[""];

    ifstream MyReadFile(path+filename);
    ofstream NewMtlFile;
    if(!MyReadFile.is_open()){
        cout<<"Mtl file: "<<path<<filename<<"cannot open.\n";
        return false;
    }

    
    NewMtlFile.open(path+"New_"+filename, ios::out);

    
    
    std::string group_name;

    while(getline(MyReadFile, myText)){
        Material newmtl;
        
        //delete the comment
        if(!myText.empty() && myText.back() == '\r'){
            myText.pop_back();
        }
        
        if(myText[0] == '#' || myText.empty()){
            NewMtlFile<<"\n";
            continue;
        }
        //check if myText is a usemtl
        auto found_newmtl = myText.find("newmtl"); 
        
        if(found_newmtl != std::string::npos){
            if(!temp_file_txt.empty()){
                if(!found_image){
                    NewMtlFile<<((*newmtl_ptr).newmtl)<<endl;
                    for(const auto& t: temp_file_txt){
                        if(t.map_data.empty()){
                            NewMtlFile<<t.name;
                            for(const auto& d: t.data){
                                NewMtlFile<<" "<<d;
                            }

                            NewMtlFile<<endl;
                        }else{
                            NewMtlFile<<t.map_data<<endl;
                        }
                    }
                    
                    NewMtlFile<<"\n\n";
                    (*newmtl_ptr).combine = false;
                    //uncombined_usemtl.push_back((*newmtl_ptr).usemtl);
                }else{
                    if(!combined_image_info.empty()){
                        
                        for(auto& prop: combined_image_info){
                            new_properties = false;
                            auto it1 = temp_file_txt.begin();
                            auto it2 = prop.properties.begin();
                            
                            //if(prop.groupname == group_name){
                                while (it1 != temp_file_txt.end() && it2 != prop.properties.end()) {
                                    
                                    if (*it1 != *it2) {
                                        new_properties = true;
                                        break;
                                    }
                                    ++it1;
                                    ++it2;
                                }

                                if(!new_properties){
                                    prop.images.push_back(temp_image);
                                    prop.usemtls.push_back(temp_usemtl);
                                    break;
                                }

                            //}else{
                                //new_properties = true;
                            //}
                            
                            
                            
                        }

                    }
                    
                    if(new_properties){
                        Properties prop;
                        for(auto& t: temp_file_txt){
                            prop.properties.push_back(t);
                        }
                        prop.first_image = temp_new_image;
                        prop.images.push_back(temp_image);
                        prop.first_newmtl = temp_newmtl;
                        prop.usemtls.push_back(temp_usemtl);
                        prop.groupname = group_name;
                        combined_image_info.push_back(prop);
                        
                        
                    }
                }

            }

            std::string temp = myText;
            temp.replace(0, 3, "use");
            
            if(all_usemtl.find(temp) == all_usemtl.end()){
                while(temp.back() == 'X'){
                    temp.pop_back();
                    if(all_usemtl.find(temp) != all_usemtl.end()){
                        break;
                    }
                }
                all_usemtl[temp].usemtl = temp;
            }
            temp_usemtl = temp;
            newmtl_ptr = &all_usemtl[temp];
            (*newmtl_ptr).newmtl = myText;
            group_name = (*newmtl_ptr).groupname;
            temp_newmtl = myText;
            
            temp_file_txt.clear();
            temp_file_txt.resize(0);
            found_image = false;

            if(!(*newmtl_ptr).combine){
                NewMtlFile<<myText<<"\n";
            }
            // if(std::find(uncombined_usemtl.begin(), uncombined_usemtl.end(), temp) == uncombined_usemtl.end()){
            //     (*newmtl_ptr).combine = true;
            // }else{
            //     (*newmtl_ptr).combine = false;
            //     NewMtlFile<<myText<<"\n";
            // }
            
                
        }else{
            
            auto found_map_Kd = myText.find("map_Kd"); 
            auto found_map = myText.find("map"); 
                
            if(found_map_Kd != std::string::npos){
                std::istringstream iss(myText);
                if(! (*newmtl_ptr).combine){
                    std::string image = myText.substr(found_map_Kd).erase(0, 7);
                    if(!image.empty() && image.back() == '\r'){
                        image.pop_back();
                    }

                    auto found = image.find_last_of("/\\");
                    image = image.substr(found + 1);
                    std::string temp = path+image;
                    FILE* jpegFile = fopen(temp.c_str(), "rb");
                    if (!jpegFile) {
                        std::string temp_a = image;
                        std::string temp_path;
                        size_t pos_1 = temp_a.find(".JPG");
                        size_t pos_2 = temp_a.find(".jpg");
                        size_t pos_3 = temp_a.find(".PNG");
                        size_t pos_4 = temp_a.find(".png");
                        if (pos_1 != std::string::npos){
                            temp_a.replace(pos_1, 4, ".jpg");
                        }else if (pos_2 != std::string::npos){
                            temp_a.replace(pos_2, 4, ".JPG");
                        }else if (pos_3 != std::string::npos){
                            temp_a.replace(pos_3, 4, ".png");
                        }else if (pos_4 != std::string::npos){
                            temp_a.replace(pos_4, 4, ".PNG");
                        }
                        temp_path = path + temp_a;
                        jpegFile = fopen(temp_path.c_str(), "rb");
                        if (!jpegFile) {
                            bool next = false;
                            std::string result_1;
                            std::string result_2;
                            std::string temp_1;
                            std::string temp_2;
                            for(char& c: image){
                                if(c == ' ' && !next){
                                    result_1 += '_';
                                    next = true;
                                }else if(c != ' '){
                                    result_1 += c;
                                    result_2 += c;
                                    next = false;
                                }
                            
                            }
                            temp_1 = path + result_1;
                            temp_2 = path + result_2;
                            

                            jpegFile = fopen(temp_1.c_str(), "rb");
                            
                            if (!jpegFile){
                                jpegFile = fopen(temp_2.c_str(), "rb");
                                if(!jpegFile){
                                    std::cerr << "Failed to open JPEG file: " << temp << std::endl;
                                    NewMtlFile<<myText<<"\n";
                                    //return false;
                                }else{
                                    fclose(jpegFile);
                                    NewMtlFile<<"map_Kd "<<result_2<<"\n";
                                }
                                
                            }else{
                                fclose(jpegFile);
                                NewMtlFile<<"map_Kd "<<result_1<<"\n";
                            }
                        }else{
                            NewMtlFile<<"map_Kd "<<temp_a<<"\n";
                        }
                        
                    }else{
                        fclose(jpegFile);
                        NewMtlFile<<myText<<"\n";
                    }
                    
                }

                
                
                std::string image = myText.substr(found_map_Kd).erase(0, 7);
                if(!image.empty() && image.back() == '\r'){
                    image.pop_back();
                }

                auto found = image.find_last_of("/\\");
                std::string image_path = image.substr(0, found + 1);
                image = image.substr(found + 1);
                
                
                bool found_jpg = isJPEGImage(image);
                size_t found_PNG = image.find(".PNG");
                size_t found_png = image.find(".png");
                
                if(found_jpg || (found_PNG != std::string::npos) || (found_png != std::string::npos)){
                    if((*newmtl_ptr).combine){
                        temp_new_image = image;
                        
                        if(found_jpg){
                            size_t dotPos = image.find_last_of('.');

                            std::string newFilename = image.substr(0, dotPos + 1) + "png";
                            
                            
                            convertJPEGtoPNG(path, image, newFilename);
                            image = newFilename;
                            temp_png.push_back(path+image);
                            

                        }
                        (*newmtl_ptr).image_info.filename = image;
                        if(loadPNG(path, image, (*newmtl_ptr).image_info)){
                            temp_image = &((*newmtl_ptr).image_info);

                            (*newmtl_ptr).image = image;
                            found_image = true;
                        
                        }
                    
                    }
                }
            }else {
                
                if(! (*newmtl_ptr).combine){
                    
                    NewMtlFile<<myText<<"\n";
                    
                }else if(found_map == std::string::npos){
                    // temp_file_txt.push_back(myText);
                    Property prop;
                    std::istringstream iss(myText);
                    std::string temp;

                    while (iss >> temp)
                    {
                        std::istringstream tempIss(temp);
                        double value;

                        if (tempIss >> value)
                        {
                            prop.data.push_back(value);
                            
                        }else{
                            if(prop.name.empty()){
                                prop.name = temp;
                            }
                            
                        }
                    }
                    
                    temp_file_txt.push_back(prop);
                    
                }else{
                    Property prop;
                    prop.map_data = myText;
                    temp_file_txt.push_back(prop);
                }
            }
        }  
    }

    MyReadFile.close();
    
    if((*newmtl_ptr).combine){
        if(!found_image){
            NewMtlFile<<((*newmtl_ptr).newmtl)<<endl;
            for(const auto& t: temp_file_txt){
                if(t.map_data.empty()){
                    NewMtlFile<<t.name;
                    for(const auto& d: t.data){
                        NewMtlFile<<" "<<d;
                    }

                    NewMtlFile<<endl;
                }else{
                    NewMtlFile<<t.map_data<<endl;
                }
            }
            NewMtlFile<<"\n\n";
            (*newmtl_ptr).combine = false;
            //uncombined_usemtl.push_back((*newmtl_ptr).usemtl);
        }else{
                    
            for(auto& prop: combined_image_info){
                new_properties = false;
                auto it1 = temp_file_txt.begin();
                auto it2 = prop.properties.begin();
                if(prop.groupname == group_name){
                    while (it1 != temp_file_txt.end() && it2 != prop.properties.end()) {
                        if (*it1 != *it2) {
                            new_properties = true;
                            break;
                        }
                        ++it1;
                        ++it2;
                    }
                    if(!new_properties){
                        prop.images.push_back(temp_image);
                        prop.usemtls.push_back(temp_usemtl);
                        break;
                    }
                }else{
                    new_properties = true;
                }
                
            }

    

            if(new_properties){
                Properties prop;
                for(auto& t: temp_file_txt){

                    prop.properties.push_back(t);
                }
                prop.first_image = temp_new_image;
                prop.images.push_back(temp_image);
                prop.first_newmtl = temp_newmtl;
                prop.usemtls.push_back(temp_usemtl);
                combined_image_info.push_back(prop);
            
            }
        }

    }

    //write on output file
    int num = 0;
    for(auto it = combined_image_info.begin(); it != combined_image_info.end();++it){
        auto& prop = *it;
        if(prop.images.size() > 1){
            NewMtlFile<<"\nnewmtl combine_mtls_"<<num<<"\n";
            prop.combine_mtl_name = "combine_mtls_"+std::to_string(num);
            for(const auto& p: prop.properties){
                if(p.map_data.empty()){
                    NewMtlFile<<"\t"<<p.name;
                    for(const auto&d: p.data){
                        NewMtlFile<<" "<<d;
                    }
                    NewMtlFile<<"\n";
                }
                
            }
            NewMtlFile<<"   map_Kd spritesheet_"<<num<<".png\n\n";


            std::sort(prop.images.begin(), prop.images.end(), sortImages);
            std::string outfile = "spritesheet_"+std::to_string(num)+".png";
            generateSpriteSheet(prop.images, path, outfile);
            num++;
            
        }else{
            std::string temp = prop.first_newmtl;
            temp.replace(0, 3, "use");
            //uncombined_usemtl.push_back(temp);
            all_usemtl[temp].combine = false;
            NewMtlFile<<prop.first_newmtl<<"\n";

            for(const auto& p: prop.properties){
                if(p.map_data.empty()){
                    NewMtlFile<<"\t"<<p.name;
                    for(const auto&d: p.data){
                        NewMtlFile<<" "<<d;
                    }
                    NewMtlFile<<"\n";
                }else{
                    NewMtlFile<<p.map_data<<endl;
                }
            }
            NewMtlFile<<"   map_Kd "<<prop.first_image<<"\n\n";
        }
    }
    

    NewMtlFile.close();

    

    

    for(auto t: temp_png){
        std::remove(t.c_str());
    }

    return true;
}


void generateSpriteSheet(std::vector<ImageInfo*>& images_ptr, const std::string& path, std::string output_file) {
    

    int x = 0;
    int y = 0;
    int y_next = y + (*images_ptr[0]).height;
    int totalWidth = 0;
    int totalHeight = 0;



    for (auto& image_p : images_ptr) {
        
        if (x + (*image_p).width > 1024 && x != 0) {
            
            x = 0;
            y = y_next;
            y_next = y + (*image_p).height;
            
            
        }

        std::vector<int>& p = (*image_p).points;
        (*image_p).points.push_back(x);
        (*image_p).points.push_back(y);
        (*image_p).points.push_back(x + (*image_p).width - 1);
        (*image_p).points.push_back(y + (*image_p).height - 1);

        x += (*image_p).width;

        if(totalWidth < x){
            totalWidth = x;
        }
        totalHeight = y_next;
    }

    std::vector<uint8_t> spriteSheet(totalWidth * totalHeight * 4, 0); 

    for (auto& image_p : images_ptr) {
        //cout<<(*image_p).filename<<endl;
        auto found_png = (*image_p).filename.find(".png");
        auto found_PNG = (*image_p).filename.find(".PNG");
        if(found_png != std::string::npos || found_PNG != std::string::npos){
            
            int x = (*image_p).points[0];
            int y = (*image_p).points[1];
            for (int i = 0; i < (*image_p).height; ++i) {
                memcpy(&spriteSheet[((totalHeight - y - i - 1) * totalWidth + x) * 4], &(*image_p).pixels[((*image_p).height - i - 1  ) * (*image_p).width * 4], (*image_p).width * 4);
                
            }
            std::vector<double>& u = (*image_p).uv_points;
            (*image_p).uv_points.push_back((double)x/totalWidth);
            (*image_p).uv_points.push_back((double)y/totalHeight);
            (*image_p).uv_points.push_back((double)(x + (*image_p).width - 1)/totalWidth);
            (*image_p).uv_points.push_back((double)(y + (*image_p).height - 1)/totalHeight);
 
        }
        
    }


    // Save the sprite sheet as an image file
    output_file = path + output_file;

    FILE* file = fopen(output_file.c_str(), "wb");

    if (!file) {
        std::cerr << "Failed to create output file: "<<output_file << std::endl;
        return;
    }

    png_structp pngWritePtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!pngWritePtr) {
        std::cerr << "Failed to create PNG write structure." << std::endl;
        fclose(file);
        return;
    }

    png_infop pngInfoPtr = png_create_info_struct(pngWritePtr);
    if (!pngInfoPtr) {
        std::cerr << "Failed to create PNG info structure." << std::endl;
        png_destroy_write_struct(&pngWritePtr, nullptr);
        fclose(file);
        return;
    }

    png_init_io(pngWritePtr, file);
    png_set_IHDR(pngWritePtr, pngInfoPtr, totalWidth, totalHeight, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(pngWritePtr, pngInfoPtr);

    png_bytep* rowPointers = new png_bytep[totalHeight];
    for (int y = 0; y < totalHeight; ++y) {
        rowPointers[y] = &spriteSheet[y * totalWidth * 4];
    }

    png_write_image(pngWritePtr, rowPointers);
    png_write_end(pngWritePtr, nullptr);

    delete[] rowPointers;

    png_destroy_write_struct(&pngWritePtr, &pngInfoPtr);
    fclose(file);

}

bool isJPEGImage(const std::string& filename) {
    // Find the position of the last occurrence of the dot (.)
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) {
        // Dot not found, invalid filename
        return false;
    }

    // Get the file extension from the filename
    std::string extension = filename.substr(dotPos + 1);

    // Convert the extension to lowercase for case-insensitive comparison
    for (auto& ch : extension) {
        ch = std::tolower(ch);
    }

    // Check if the extension matches ".jpg" or ".jpeg"
    return (extension == "jpg" || extension == "jpeg");
}


bool sortImages(const ImageInfo* a_p, const ImageInfo* b_p) {
    
    if((*a_p).height == (*b_p).height){
        return (*a_p).width > (*b_p).width;
    }else{
        return (*a_p).height > (*b_p).height;
    }
    
}


bool loadPNG(const std::string& path, std::string filename, ImageInfo& imageInfo) {
    
    std::string temp = filename;
    filename = path + filename;
    
    
    FILE* file = fopen(filename.c_str(), "rb");
    

    if (!file) {
        bool next = false;
        std::string result;
        for(char& c: temp){
            if(c == ' ' && !next){
                result += '_';
                next = true;
            }else if(c != ' '){
                result += c;
                next = false;
            }
        
        }
        
        result = path + result;

        file = fopen(result.c_str(), "rb");
        if (!file){
            std::cerr << "Failed to open PNG file: " << result << std::endl;
            return false;
        }
        
    }

    png_structp pngReadPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!pngReadPtr) {
        std::cerr << "Failed to create PNG read structure." << std::endl;
        fclose(file);
        return false;
    }

    png_infop pngInfoPtr = png_create_info_struct(pngReadPtr);
    if (!pngInfoPtr) {
        std::cerr << "Failed to create PNG info structure." << std::endl;
        png_destroy_read_struct(&pngReadPtr, nullptr, nullptr);
        fclose(file);
        return false;
    }

    png_init_io(pngReadPtr, file);
    png_set_option(pngReadPtr, PNG_SKIP_sRGB_CHECK_PROFILE, PNG_OPTION_ON);
    png_read_info(pngReadPtr, pngInfoPtr);

    //png_set_crc_action(pngReadPtr, PNG_CRC_QUIET_USE, PNG_CRC_QUIET_USE);
    png_byte interlace_type = png_get_interlace_type(pngReadPtr, pngInfoPtr);
    if (interlace_type != PNG_INTERLACE_NONE) {
        png_set_interlace_handling(pngReadPtr);
    }

    //png_read_update_info(pngReadPtr, pngInfoPtr);



    imageInfo.width = png_get_image_width(pngReadPtr, pngInfoPtr);
    imageInfo.height = png_get_image_height(pngReadPtr, pngInfoPtr);
    png_byte colorType = png_get_color_type(pngReadPtr, pngInfoPtr);
    png_byte bitDepth = png_get_bit_depth(pngReadPtr, pngInfoPtr);

    if (bitDepth == 16)
        png_set_strip_16(pngReadPtr);

    if (colorType == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(pngReadPtr);

    if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
        png_set_expand_gray_1_2_4_to_8(pngReadPtr);

    if (png_get_valid(pngReadPtr, pngInfoPtr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(pngReadPtr);

    if (colorType == PNG_COLOR_TYPE_RGB || colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(pngReadPtr, 0xFF, PNG_FILLER_AFTER);

    if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(pngReadPtr);

    png_read_update_info(pngReadPtr, pngInfoPtr);

    int numChannels = png_get_channels(pngReadPtr, pngInfoPtr);
    int rowStride = png_get_rowbytes(pngReadPtr, pngInfoPtr);

    imageInfo.pixels.resize(imageInfo.width * imageInfo.height * numChannels);

    png_bytep* rowPointers = new png_bytep[imageInfo.height];
    for (int y = 0; y < imageInfo.height; ++y) {
        rowPointers[y] = &imageInfo.pixels[y * rowStride];
    }

    png_read_image(pngReadPtr, rowPointers);
    png_read_end(pngReadPtr, nullptr);

    delete[] rowPointers;

    png_destroy_read_struct(&pngReadPtr, &pngInfoPtr, nullptr);
    fclose(file);

    return true;
}


bool convertJPEGtoPNG(const std::string& path, std::string jpegFilename, std::string pngFilename) {
    //cout<<"in this:";
    // Load JPEG image using libjpeg
    std::string temp = jpegFilename;
    jpegFilename = path + jpegFilename;
    pngFilename = path + pngFilename;

    FILE* jpegFile = fopen(jpegFilename.c_str(), "rb");
    if (!jpegFile) {
        std::string temp_1 = temp;
        size_t pos_1 = temp_1.find(".JPG");
        size_t pos_2 = temp_1.find(".jpg");
        if (pos_1 != std::string::npos){
            temp_1.replace(pos_1, 4, ".jpg");
        }else if(pos_2 != std::string::npos){
            temp_1.replace(pos_2, 4, ".JPG");
        }
        temp_1 = path + temp_1;
        jpegFile = fopen(temp_1.c_str(), "rb");
        if(!jpegFile){
            bool next = false;
            std::string result_1;
            std::string result_2;
            for(char& c: temp){
                if(c == ' ' && !next){
                    result_1 += '_';
                    next = true;
                }else if(c != ' '){
                    result_1 += c;
                    result_2 += c;
                    next = false;
                }
            
            }
            result_1 = path + result_1;
            result_2 = path + result_2;
            

            jpegFile = fopen(result_1.c_str(), "rb");
            
            if (!jpegFile){
                jpegFile = fopen(result_2.c_str(), "rb");
                if(!jpegFile){
                    std::cerr << "Failed to open JPEG file: " << jpegFilename << std::endl;
                    return false;
                }
                
            }

        }
        
        
    }

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, jpegFile);
    jpeg_read_header(&cinfo, TRUE);

    jpeg_start_decompress(&cinfo);

    int imageWidth = cinfo.output_width;
    int imageHeight = cinfo.output_height;
    int numChannels = cinfo.output_components;

    std::vector<uint8_t> imagePixels(imageWidth * imageHeight * numChannels);

    while (cinfo.output_scanline < cinfo.output_height) {
        uint8_t* rowPointer = &imagePixels[cinfo.output_scanline * imageWidth * numChannels];
        jpeg_read_scanlines(&cinfo, &rowPointer, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(jpegFile);

    // Convert JPEG image to PNG using libpng
    FILE* pngFile = fopen(pngFilename.c_str(), "wb");
    if (!pngFile) {
        std::cerr << "Failed to create PNG file: " << pngFilename << std::endl;
        return false;
    }

    png_structp pngWritePtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!pngWritePtr) {
        std::cerr << "Failed to create PNG write structure." << std::endl;
        fclose(pngFile);
        return false;
    }

    png_infop pngInfoPtr = png_create_info_struct(pngWritePtr);
    if (!pngInfoPtr) {
        std::cerr << "Failed to create PNG info structure." << std::endl;
        png_destroy_write_struct(&pngWritePtr, nullptr);
        fclose(pngFile);
        return false;
    }

    png_init_io(pngWritePtr, pngFile);
    //png_set_crc_action(pngWritePtr, PNG_CRC_QUIET_USE, PNG_CRC_QUIET_USE);

    png_set_IHDR(pngWritePtr, pngInfoPtr, imageWidth, imageHeight, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(pngWritePtr, pngInfoPtr);

    

    png_bytep* rowPointers = new png_bytep[imageHeight];
    for (int y = 0; y < imageHeight; ++y) {
        rowPointers[y] = &imagePixels[y * imageWidth * numChannels];
    }

    png_write_image(pngWritePtr, rowPointers);
    png_write_end(pngWritePtr, nullptr);

    delete[] rowPointers;

    png_destroy_write_struct(&pngWritePtr, &pngInfoPtr);
    fclose(pngFile);
    return true;
    //std::cout << "Conversion complete: JPEG to PNG" << std::endl;
}
