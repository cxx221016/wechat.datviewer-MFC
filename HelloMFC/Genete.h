#pragma once
#include<fstream>
#include<sstream>
#include<iostream>
#include<unordered_map>
#include<string>
#include<cstdio>
#include<Windows.h>
#include<io.h>
#include <tchar.h>

using namespace std::literals;


namespace std {
    template<>
    class hash<pair<unsigned char, unsigned char>>
    {
    public:
        size_t operator()(const pair<unsigned char, unsigned char>& p) const {
            return hash<int>()(p.first) ^ hash<int>()(p.second);
        }
    };
};

using head = std::pair<unsigned char, unsigned char>;

static std::unordered_map<head, std::string> fileheadmap
{
    {head{0x42, 0x4D}, "bmp"},
    { head{0x47,0x49},"gif" },
    { head{0x89,0x50},"png" },
    { head{0xFF,0xD8},"jpg" },
    { head{0x49,0x49},"tif" },
    { head{0x00,0x00},"ico" }
};

static std::string gfoutpath;

inline std::pair<std::string, std::string> getinfo(const std::string& filepath)
{
    std::cout << filepath << std::endl;
    auto idx = filepath.find_last_of('.');
    if (idx == std::string::npos)
    {
        return { filepath,"" };
    }
    else
    {
        std::cout << filepath.substr(0, idx) << " " << filepath.substr(idx + 1) << std::endl;
        return { filepath.substr(0,idx),filepath.substr(idx + 1) };
    }
}

inline std::pair<unsigned char, std::string> getfiletype(const head& h)
{
    for (auto& [k, v] : fileheadmap)
    {
        //std::cout<<(int)(k.first^h.first)<<" "<<(int)(k.second^h.second)<<std::endl;
        if ((k.first ^ h.first) == (k.second ^ h.second))
        {
            //std::cout<<(int)(k.first^h.first)<<" "<<(int)(k.second^h.second)<<std::endl;
            return { (k.first ^ h.first),v };
        }
    }
    return { 0,"" };
}

static std::string logpath = "D:\\MY_CODE\\VISUAL CPP\\HelloMFC\\data\\log.txt";

inline bool parse(const std::string& filepath,const std::string& finpath)
{
    auto info = getinfo(filepath);
    if (info.second == "")
    {
        std::cout << "not a dat file" << std::endl;
        return false;
    }
    if (info.second != "dat")
    {
        std::cout << "not a dat file" << std::endl;
        return false;
    }
    std::ifstream fin(filepath, std::ios::binary);
    if (!fin)
    {
        std::cout << "file not found" << std::endl;
        return false;
    }
    int length = fin.seekg(0, std::ios::end).tellg();
    fin.seekg(0, std::ios::beg);
    unsigned char* buffer = new unsigned char[length];
    fin.read((char*)buffer, length);
    fin.close();
    //std::cout<<length<<std::endl; 
    auto filetype = getfiletype({ buffer[0],buffer[1] });
    if (filetype.second == "")
    {
        std::cout << "unknown file type" << std::endl;
        return false;
    }
    unsigned char key = filetype.first;
    for (int i = 0; i < length; ++i)
    {
        buffer[i] ^= key;
    }
    std::string foutfilepath=info.first + "." + filetype.second;
    foutfilepath=foutfilepath.replace(foutfilepath.find(finpath),finpath.length(),gfoutpath);
    //MessageBox(NULL, foutfilepath.c_str(), _T("Path"), 0);
    std::ofstream fout(foutfilepath, std::ios::binary);
    fout.write((char*)buffer, length);
    fout.close();
    delete[] buffer;
    return true;
}

inline void parseall(const std::string& prefilepath)
{
    uintptr_t handle;
    _finddata_t findData;
    std::string filepath = prefilepath+"/*.*";
    if (!(handle = _findfirst(filepath.c_str(), &findData)))
    {
        
    }
    do
    {
        if (findData.attrib & _A_SUBDIR)
        {
            if(findData.name==std::string(".")||findData.name==std::string(".."))
				continue;
            std::string subdir(prefilepath);
            subdir += "/";
            subdir += findData.name;
            parseall(subdir);
        }
        else
        {
            //MessageBox(NULL, (prefilepath + "\\" + findData.name).c_str(), _T("Path"), 0);
            parse(prefilepath+"\\"+findData.name, prefilepath);
        }
    }while(_findnext(handle,&findData) == 0);
    _findclose(handle);
}

inline void Parser(const std::string& filepath,const std::string& foutpath)
{
	gfoutpath=foutpath;

	parseall(filepath);
    gfoutpath="";
}
