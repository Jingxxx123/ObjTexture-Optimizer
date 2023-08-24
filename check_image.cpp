#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <list>
#include <algorithm>
#include <cmath>
#include "read_mtl.h"
#include <cstdio>
#include <limits>

using namespace std;


void calculateNewCoord( std::vector<Coordinate>& vt_list, /*std::list<std::string>& uncombined_usemtl,*/ std::unordered_map<std::string, Material>& all_usemtl, std::list<int> overrange_vt_index);


int main(int argc, char *argv[])
{
    //
    for (int i = 1; i < argc; i++){
        std::string myText;
        std::string file_name;
        std::string mtl_filename;
        file_name = argv[i];
        
        auto found_sorted_file = file_name.find("Sorted_");
        auto found_new_file = file_name.find("New_");
        if(found_sorted_file != std::string::npos  || found_new_file != std::string::npos){
            continue;
        }

        std::unordered_map<std::string, Material> all_usemtl;
        std::vector<Coordinate> v_list;
        std::vector<Coordinate> vt_list;
        std::vector<Coordinate> vn_list;
        std::vector<Coordinate> vp_list;
        std::list<std::string> uncombined_usemtl;
        std::list<int> overrange_vt_index;

        Material* mtl_ptr= &all_usemtl[""];
        Material* s_ptr = NULL;
        std::string s_line; 

        // Material coord_list;
        // all_usemtl[""] = coord_list;
        // mtl_ptr = &all_usemtl[""];
        std::list<Group> groups;
        std::string group_name = "";
        Group group;
        group.groupname = "";
        groups.push_back(group);
        Group* g_ptr;
        g_ptr = &groups.front();
        

        std::string path;
        std::string file;
        auto found = file_name.find_last_of("/\\");
        path = file_name.substr(0, found + 1);
        file = file_name.substr(found + 1);

        ifstream MyReadFile;
        ofstream OutputFile;
        MyReadFile.open(file_name, ios::in);
        OutputFile.open(path + "New_"+file, ios::out | ios::trunc); 

        while (getline(MyReadFile, myText))
        {
            if(!myText.empty() && myText.back() == '\r'){
                myText.pop_back();
            }

           
            //delete the comment
            if (myText[0] == '#' || myText.empty())
            {
                continue;
            }
            //check if myText is a usemtl
            auto found_usemtl = myText.find("usemtl");
            auto found_mtl_file = myText.find("mtllib");

            if (found_mtl_file != std::string::npos){
                mtl_filename = myText.erase(0, 7);

                // Find the position of the target substring
                size_t targetIndex = mtl_filename.find(".mtl");

                // Delete characters after the target substring
                if (targetIndex != std::string::npos) {
                    mtl_filename.erase(targetIndex + 4);
                }

                ifstream MyMtlFile(path+mtl_filename);

                if(!MyMtlFile.is_open()){
                    std::string new_file = path + "New_"+file;
                    std::remove(new_file.c_str());
                    break;
                }else{
                    MyMtlFile.close();
                }
                
                
            }
            else if (myText[0] == 'v')
            {
                bool vt = false;
                bool vt_pass = true;
                std::vector<Coordinate> *coord_list_ptr;
                if (myText.substr(0, 2) == "v ")
                {
                    coord_list_ptr = &v_list;
                }
                else if (myText.substr(0, 2) == "vt")
                {
                    vt = true;
                    coord_list_ptr = &vt_list;
                }
                else if (myText.substr(0, 2) == "vn")
                {
                    coord_list_ptr = &vn_list;
                }
                else if (myText.substr(0, 2) == "vp")
                {
                    coord_list_ptr = &vp_list;
                }

                std::istringstream iss(myText);
                std::string temp;
                Coordinate coord;

                if(vt){
                    while (iss >> temp)
                    {
                        std::istringstream tempIss(temp);
                        double value;

                        if (tempIss >> value)
                        {
                            coord.org_points.push_back(value);
                            coord.points.push_back(value);
                            if (abs(value) > 1.1 && coord.org_points.size() < 3)
                            {
                                vt_pass = false;
                            }
                            
                        }
                    }
                }
                
                coord.line = myText;
                (*coord_list_ptr).push_back(coord);
            

                if (!vt_pass)
                {
                    int index = vt_list.size() - 1;
                    overrange_vt_index.push_back(index);
                }
            }
            else if (found_usemtl != std::string::npos)
            {
                
                if(all_usemtl.find(myText) == all_usemtl.end()){
                    Material mtl_list;
                    mtl_list.usemtl = myText;
                    mtl_list.groupname = group_name;
                    all_usemtl[myText] = mtl_list;
                    (*g_ptr).usemtls.push_back(myText);
                }

                mtl_ptr = &all_usemtl[myText];
                
                if(s_ptr != NULL){
                    Face face;
                    face.line = "s off";
                    (*s_ptr).faces.push_back(face);
                    face.line = s_line;
                    (*mtl_ptr).faces.push_back(face);
                    s_ptr = mtl_ptr;
                }
            }
            else if (myText[0] == 'f')
            {

                std::istringstream iss(myText);
                std::string temp;
                int v_num = v_list.size();
                int vt_num = vt_list.size();
                int vn_num = vn_list.size();

                double min_x = std::numeric_limits<double>::infinity();
                double min_y = std::numeric_limits<double>::infinity();
                double max_x = -std::numeric_limits<double>::infinity();
                double max_y = -std::numeric_limits<double>::infinity();
		        std::string new_line = "";
                int new_index;
                Face face;

                while (iss >> temp)
                {
                    std::istringstream tempIss(temp);
                    std::string num_str;
                    int num = 0;
                   
                    while (std::getline(tempIss, num_str, '/'))
                    {
   
                        int index = (num_str.empty() ? 0 : std::atoi(num_str.c_str()));

                        
                        num++;
			            
                        if(index < 0){
                            face.positive = false;
                        }

                        int i = index - 1;
                        if (num == 1 && index != 0)
                        {    
                            if(index < 0){
                                i = v_num + index;
                                new_index = i + 1;
                                std::string num_str = std::to_string(new_index);
                                new_line = new_line + " " + num_str;
                            }
                            face.coords.push_back(v_list[i]);
                        }
                        else if (num == 2 && index != 0)
                        {
                            if(index < 0){
                                i = vt_num + index;
			                    new_index = i + 1;                             
                                std::string num_str = std::to_string(new_index);
                                new_line = new_line + "/" + num_str;
                            }

                            face.coords.push_back(vt_list[i]);


                            
                            if (std::find(face.vt_indexs.begin(), face.vt_indexs.end(), i) == face.vt_indexs.end())
                            {
                                face.vt_indexs.push_back(i);
                            }
                            
                            if (std::find(overrange_vt_index.begin(), overrange_vt_index.end(), i) != overrange_vt_index.end())
                            {
                                std::string mtl = (*mtl_ptr).usemtl;
                                all_usemtl[mtl].combine = false; 
                                // if (std::find(uncombined_usemtl.begin(), uncombined_usemtl.end(), mtl) == uncombined_usemtl.end())
                                // {
                                //     uncombined_usemtl.push_back(mtl); 
                                //     all_usemtl[mtl].combine = false;          
                                // }
                            }else{
                                double x = vt_list[i].org_points[0];
                                double y = vt_list[i].org_points[1];

                                if(x < min_x){
                                    min_x = x;
                                }
                                if(x > max_x){
                                    max_x = x;
                                }
                                if(y < min_y){
                                    min_y = y;
                                }
                                if(y > max_y){
                                    max_y = y;
                                }
                            }
                        }
                        else if (num == 3 && index != 0)
                        {
                            if(index < 0){
                                i = vn_num + index;
				                new_index = i + 1;
                                std::string num_str = std::to_string(new_index);
				                new_line = new_line + "/" + num_str;
                            }
                            
                            face.coords.push_back(vn_list[i]);
                        }else if(num == 2 && index == 0){
                            if(!face.positive){
                            new_line = new_line + "/";
                            }
			            }

                        
                    }

                    if(new_line.empty()){
                        new_line = temp;
                    }
                }

                std::string mtl = (*mtl_ptr).usemtl;
                if(abs(max_x - min_x) > 1.1 || abs(max_y - min_y) > 1.1){
                    // if (std::find(uncombined_usemtl.begin(), uncombined_usemtl.end(), mtl) == uncombined_usemtl.end())
                    // {
                    //     uncombined_usemtl.push_back(mtl); 
                    //     all_usemtl[mtl].combine = false;       
                    // }
                    all_usemtl[mtl].combine = false; 
                }
                
                if(face.positive){
                    face.line = myText;
                }else{
                    face.line = new_line;
                    face.positive = true;
                }
                (*mtl_ptr).faces.push_back(face);
            }
            else if(myText[0] == 'g'){
                
                group_name = myText;
                mtl_ptr = &all_usemtl[myText];
                Group group;
                group.groupname = myText;
                groups.push_back(group);
                g_ptr = &(groups.back());
            }
            else{
                Face face;
                face.line = myText;
                (*mtl_ptr).faces.push_back(face);
                
                if(myText[0] == 's'){
                    auto found_off = myText.find("off");
                    if (found_off != std::string::npos || myText == "s 0"){
                        s_ptr = NULL;
                    }else{
                        s_ptr = mtl_ptr;
                        s_line = myText; 
                    }    
                    
                }
            }
        }

        if(s_ptr != NULL){
            Face face;
            face.line = "s off";
            (*s_ptr).faces.push_back(face);
        }

        MyReadFile.close();
        std::list<Properties> combined_image_info;
        
        if(createCombinedImage(path, mtl_filename, /*uncombined_usemtl,*/ all_usemtl,combined_image_info)){
            
           calculateNewCoord(vt_list, /*uncombined_usemtl,*/ all_usemtl, overrange_vt_index);
           
            OutputFile<<"mtllib New_"+mtl_filename<<"\n\n\n";
            for(const auto& v: v_list){
                OutputFile<<v.line<<"\n";
            }

            for(const auto& vn: vn_list){
                OutputFile<<vn.line<<"\n";
            }

            for(const auto& vp: vp_list){
                OutputFile<<vp.line<<"\n";
            }
            
            for(const auto& vt: vt_list){
                OutputFile<<"vt "<<vt.points[0]<<" "<<vt.points[1]<<" "<<vt.points[2]<<"\n";
            }

            std::list<std::string> uncombined_list;
            std::list<std::string> g_list;
            s_line = "";
            bool s_erase = false;
            std::list<std::string>::iterator temp_s;

            for(const auto& m: all_usemtl){
                if(!m.first.empty()){
                 
                    for(auto& p: combined_image_info){
                        if(p.images.size() > 1 && find(p.usemtls.begin(), p.usemtls.end(), m.first) != p.usemtls.end()){
                            std::string comment = "#"+m.first;
                            std::list<Face> faces = m.second.faces;
                            p.faces.push_back(comment);
                            //cout<<num<<": "<<m.first<<endl;
                            for(const auto&i: faces){

                                p.faces.push_back(i.line);
                            }
                            continue;
                        }
                    }
                }
            }

            for(auto& g:groups){
                
                if(!all_usemtl[g.groupname].faces.empty()){
                    std::list<Face> faces = all_usemtl[g.groupname].faces;
                    for(const auto&i: faces){

                        g.faces.push_back(i.line);
                    }
                }
                for(auto& p: combined_image_info){
                    if(p.images.size() > 1 && p.groupname == g.groupname){
                        std::string combine_name = "usemtl "+p.combine_mtl_name;
                        g.faces.push_back(combine_name);
                        for(const auto& f: p.faces){
                            g.faces.push_back(f);
                        }
                        
                    } 
                    
                }
                for(const auto& u: g.usemtls){
                    if(!all_usemtl[u].combine){
                    //if(find(uncombined_usemtl.begin(), uncombined_usemtl.end(), u) != uncombined_usemtl.end()){
                        Material m = all_usemtl[u];
                        g.faces.push_back(u);
                        for(const auto& f: m.faces){
                            g.faces.push_back(f.line);
                        }
                    }
                    
                }
            }

            for(const auto& g:groups){
                OutputFile<<g.groupname<<endl;
                    

                std::list<std::string> faces = g.faces;

                for (auto it = faces.begin(); it != faces.end(); ++it) {
                    
                    std::string f=(*it);
                    if(f[0] == 's'){
                        auto it_next = std::next(it, 1);
                        if((it_next) != faces.end()){
                            std::string next = *(it_next);
                                
                            if(next[0] == 's'){
                                continue;
                            }
                            
                        }

                        auto found_off = f.find("off");
                        
                        if(found_off != std::string::npos || f == "s 0")
                        {
                            if(s_line.empty()){
                                continue;
                            }else{
                                s_line.clear();
                            }
                            
                            
                        }else if(f == s_line){
                            continue;
                        }else{
                            s_line = f;
                        }
                    }
                    OutputFile<<f<<"\n";
                }
            }
        }
        

        OutputFile.close();

        //clear
        all_usemtl.clear();
        v_list.clear();
        vt_list.clear();
        vn_list.clear();
        uncombined_usemtl.clear();
        overrange_vt_index.clear();
        combined_image_info.clear();
        groups.clear();
        

        //all_usemtl.re;
        v_list.resize(0);
        vt_list.resize(0);
        vn_list.resize(0);
        uncombined_usemtl.resize(0);
        overrange_vt_index.resize(0);
        combined_image_info.resize(0);
        groups.resize(0);
        

    }



}




void calculateNewCoord(/*const std::vector<ImageInfo>& images, */std::vector<Coordinate>& vt_list, /*std::list<std::string>& uncombined_usemtl, */std::unordered_map<std::string, Material>& all_usemtl, std::list<int> overrange_vt_index){
    bool share_coord = false;
    int org_vt_size = vt_list.size();
    int changed_num = 0;
    bool not_find = false;
    bool changed_vt = false;
    for(auto& mtl: all_usemtl){
        std::string usemtl = mtl.first;
        if(usemtl == ""){
            continue;
        }

        Material& material = mtl.second;
        ImageInfo& image_info = material.image_info;
        std::vector<double>& uv = image_info.uv_points;
        std::string& n = image_info.filename;

        bool combine = all_usemtl[usemtl].combine;
        //bool combine = (std::find(uncombined_usemtl.begin(), uncombined_usemtl.end(), usemtl) == uncombined_usemtl.end());
        if(combine && n == ""){ 
            //uncombined_usemtl.push_back(usemtl);
            all_usemtl[usemtl].combine = false;
            combine = false;
        }

        std::unordered_map<int, int> changed_vt_index;
        double a, b, c, d;
        if(combine){
            a = uv[0];
            b = uv[1];
            c = uv[2];
            d = uv[3];
        }       


        for(auto& f: material.faces){
            std::vector<int> indexs = f.vt_indexs;
            bool u_neg = false;
            bool v_neg = false;
            for(auto& i: indexs){

                auto it = std::find(overrange_vt_index.begin(), overrange_vt_index.end(), i);
                if(it == overrange_vt_index.end()){
                    Coordinate coord_copy;

                    coord_copy = vt_list[i];

                    double u = coord_copy.org_points[0];
                    double v = coord_copy.org_points[1];
                    
                    if(u < -0.1){
                        
                        u_neg = true;
                    }

                    if(v < -0.1){
                        
                        v_neg = true;
                    }

                    if(coord_copy.usemtl.empty()){

                        Coordinate* coord_ptr;
                        coord_ptr = &vt_list[i];

                        if(combine){
                            

                            double new_u, new_v;
                            new_u = a+(c-a)*u;
                            new_v = b+(d-b)*v;
    
                            (*coord_ptr).points[0] = (new_u);
                            (*coord_ptr).points[1] = (new_v);
                        }
                        
                        f.new_vt_indexes.push_back(i);
                        (*coord_ptr).usemtl = usemtl;
                        
                    }else if(coord_copy.usemtl != usemtl){
                        int new_index;
                        if(changed_vt_index.find(i) == changed_vt_index.end()){
                            
                            share_coord = true;
                           
                            if(combine){
                                double new_u, new_v;
                                new_u = a+(c-a)*u;
                                new_v = b+(d-b)*v;
                            
                                coord_copy.points[0] = (new_u);
                                coord_copy.points[1] = (new_v);
                            }else{
                                coord_copy.points[0] = coord_copy.org_points[0];
                                coord_copy.points[1] = coord_copy.org_points[1];
                                
                            }

                            coord_copy.usemtl = usemtl;

                            vt_list.push_back(coord_copy);
                            new_index = vt_list.size();
                            f.new_vt_indexes.push_back(new_index - 1);
                            
                            changed_vt_index[i] =  new_index;
                            
                        }else{
                            
                            new_index = changed_vt_index[i];
                            f.new_vt_indexes.push_back(new_index - 1);
                        }
                        

                        std::string& line = f.line;
                        
                        int org_index;
                        org_index = i + 1;
                        

                        std::istringstream iss(line);
                        std::string temp;
                        std::string new_line = "";
                        std::string org_string = std::to_string(org_index);
                        std::string new_string = std::to_string(new_index);
                        bool check = false;

                        while (iss >> temp)
                        {
                            std::istringstream tempIss(temp);
                            std::string num_str;
                            int num = 0;
                            
                            size_t pos = temp.find(org_string);

                            if(pos != std::string::npos){
                                
                                while (std::getline(tempIss, num_str, '/'))
                                {
                                    
                                    int index = (num_str.empty() ? 0 : std::atoi(num_str.c_str()));

                                    num++;

                                    if (num == 2 && index == org_index)
                                    {
                                        new_line = new_line + "/" + new_string;
                                        check = true;
                                    }else if((num == 3 && index != 0) || num == 2){
                                        new_line = new_line + "/" + num_str;
                                    }else if(num == 1 && index != 0){
                                        new_line = new_line + " " + num_str;
                                    }
                                }
                            }else{
                                if(new_line.empty()){
                                    new_line = temp;
                                }else{
                                    new_line = new_line + " " + temp;
                                }
                            }
                            
                        }

                        if(!check){
                            not_find = true;
                        }

                        f.line = new_line;

                    }

                }
            }

            if(combine){
                for(auto i: f.new_vt_indexes){
                    Coordinate* ptr = &vt_list[i];
                    double u = (*ptr).org_points[0];
                    double v = (*ptr).org_points[1];
                    if(u_neg){
                        u = u+1;
                        (*ptr).points[0] = a+(c-a)*u;
                        changed_vt = true;
                    }

                    if(v_neg){
                        v = v+1;
                        (*ptr).points[1] = b+(d-b)*v;
                        changed_vt = true;
                    }
                }
            }
        }
    }

}


