#include <filesystem>
#include <vector>
#include <string>
#include <memory>
#include <iostream>

using namespace std;

string PWD()
{
    return filesystem::current_path();
}
string PARENT(string path)
{
    return ((filesystem::path)path).parent_path();
}
string EXECPATH(const char* argv0)
{
    return filesystem::absolute(PWD() + '/' + PARENT(argv0));
}
void copy(string from, string to)
{
    filesystem::copy(from, to, filesystem::copy_options::overwrite_existing | filesystem::copy_options::recursive);
}
void READDIR(string path, vector<string>* files)
{
    for(const auto& file : filesystem::directory_iterator(path))
        files->emplace_back(move(file.path().stem()));
}
bool exists(string path)
{
    return filesystem::exists(path);
}
string filename(string path)
{
    return ((filesystem::path)path).filename();
}
string stem(string path)
{
    return ((filesystem::path)path).stem();
}