#include <iostream>
#include <unistd.h>
#include <linux/limits.h>
#include <fstream>
#include <algorithm>
#include "dir.hpp"

using namespace std;

string strtoupper(string in)
{
    transform(in.begin(), in.end(), in.begin(), ::toupper);
    return in;
}
string camelCase(string in)
{
    string out = "";
    char nin = ::tolower(in[0]);
    if(nin == in[0])
        out = "_" + in;
    else
    {
        out += nin;
        for(int i = 1; i < in.size(); i++)
            out += in[i];
    }
    return out;
}

int argc;
char const** argv;

string pwd, medir;

void errhelp(string message);

string MEDIR()
{
    char result[PATH_MAX];
    size_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if(count == -1)
        throw runtime_error("Couldn't find own directory!");
    string dir = "";
    for(int i = 0; i < count; i++)
        dir += result[i];
    return PARENT(dir);
}

void init()
{

    string to = pwd;
    string from = medir + "/Template";
    copy(from, to);

    ofstream of(to + "/HardDefinitions.h");

    string title, width, height, fullscreen, bgr, bgg, bgb, bga;

    cout << "Input " << "title" << ": ";
    cin >> title;
    cout << "Input " << "width" << ": ";
    cin >> width;
    cout << "Input " << "height" << ": ";
    cin >> height;
    cout << "Input " << "whether to activate fullscreen (true/false)" << ": ";
    cin >> fullscreen;

    cout << "Input " << "R" << ": ";
    cin >> bgr;
    cout << "Input " << "G" << ": ";
    cin >> bgg;
    cout << "Input " << "B" << ": ";
    cin >> bgb;
    cout << "Input " << "A" << ": ";
    cin >> bga;

    #pragma region code def

    string precode = R"(#ifndef HARD_DEFINITIONS_HPP

#define HARD_DEFINITIONS_HPP

)";

    string postcode = R"(
#endif // !HARD_DEFINITIONS_HPP)";

    #pragma endregion

    string code = precode;

    code += "#define TITLE \"" + title + "\"\n";
    code += "#define WIDTH " + width + "\n";
    code += "#define HEIGHT " + height + "\n";
    code += "#define FULLSCREEN " + fullscreen + "\n";

    code += "#define BGR " + bgr + "\n";
    code += "#define BGG " + bgg + "\n";
    code += "#define BGB " + bgb + "\n";
    code += "#define BGA " + bga + "\n";

    code += postcode;

    of.write(code.c_str(), code.size());

    of.close();

}
void include()
{

    string path = pwd + "/Definitions.h";

    if(!exists(path))
        throw runtime_error("The current project is not valid! Definitions.h could not be found! Try the init command.");

    ofstream of(path);
    
    #pragma region code def shit

    string precode = R"(#ifndef DEFINITIONS_HPP

#define DEFINITIONS_HPP

#include "HardDefinitions.h"
#include "InitManager.hpp"

)";

    string middlecode = R"(
namespace ObjectDefinitions
{

static void Define()
{

)";

    string postcode = R"(
}

};

#endif // !DEFINITIONS_HPP)";

    #pragma endregion

    string odir = pwd + "/Objects";

    if(!exists(odir))
        throw runtime_error("The current project is not valid! Objects could not be found! Try the init command.");

    vector<string> objects;
    READDIR(odir, &objects);
    
    string code = precode;

    for(int i = 0; i < objects.size(); i++)
        code += "#include \"Objects/" + objects[i] + ".hpp\"\n";

    code += middlecode;

    for(int i = 0; i < objects.size(); i++)
        code += "\t\tInitManager::requestGameObject(" + objects[i] + ");\n";

    code += postcode;

    of.write(code.c_str(), code.size());
    of.close();

    cout << "Includes in Definitions.h were defined!" << endl;

}
void add()
{

    if(argc < 3)
        throw runtime_error("Must define what to add!");
    string type = argv[2];
    if(type == "component")
    {
        string name;
        cout << "Insert valid component name: ";
        cin >> name;
        string fname = name + ".hpp";
        string path = pwd + "/Components/" + fname;
        ofstream of(path);
        string NAME = strtoupper(name);
        #pragma region gencode

        string code = R"(#ifndef )" + NAME + R"(_COMPONENT_HPP

#define )" + NAME + R"(_COMPONENT_HPP

#include "../DDEngine.hpp"

using namespace DDEngine;

class )" + name + R"( : public Component
{

public:

// start is called at the beginning of the game
void start() override
{

}

//update is called every frame
void update() override
{

}

string name() override { return ")" + fname + R"("; }

};

#endif // !)" + NAME + R"(_COMPONENT_HPP)";

        #pragma endregion
        of.write(code.c_str(), code.size());
        of.close();
    }
    else if(type == "object" || type == "gameobject")
    {
        string name;
        vector<string> comps;
        cout << "Insert valid game object name: ";
        cin >> name;
        while(true)
        {
            string decision;
            cout << "Add component? (y/n): ";
            cin >> decision;
            if(decision == "y" || decision == "Y")
            {
                cout << "Insert name: ";
                cin >> decision;
                comps.emplace_back(move(decision));
            }
            else if(decision == "n" || decision == "N")
                break;
            else
                cout << "\"" << decision << "\" is not valid!" << endl;
        }
        string fname = name + ".hpp";
        string path = pwd + "/Objects/" + fname;
        ofstream of(path);
        string NAME = strtoupper(name);
        #pragma region codegen

        string compinc = "";

        for(int i = 0; i < comps.size(); i++)
            compinc += "#include \"../Components/" + comps[i] + ".hpp\"\n";

        string compadd = "";

        for(int i = 0; i < comps.size(); i++)
            compadd += "\t" + comps[i] + "* " + camelCase(comps[i]) + " = go->addComponent<" + comps[i] + ">();\n";

        string code = compinc + R"(
#include "../DDEngine.hpp"
using namespace DDEngine;

void )" + name + R"((GameObject* go)
{

go->transform.scale = Scale(100, 100);

RectRender* rectRender = go->addComponent<RectRender>();

rectRender->color = Color(100, 100, 100, 255);

)" + compadd + R"(
})";

        #pragma endregion
        of.write(code.c_str(), code.size());
        of.close();
        include();
    }
    else
        throw runtime_error("Cannot add '" + type + "'!");
        
}
void help()
{

    string end = "\0\0\0";
    string cmds[]{
        "init",
        "include",
        "add",
        end
    };
    string helps[]{
        "Initializes a DDEngine project",
        "Call to automatically include defined objects",
        "Add a new component or game object to the project",
        end
    };

    if(argc >= 3)
    {
        string search = argv[2];
        int found = -1;
        for(int i = 0; cmds[i] != end; i++)
            if(cmds[i] == search)
            {
                found = i;
                break;
            }
        if(found == -1)
        {
            errhelp("Couldn't find '" + search + "'!\n");
            return;
        }
        cout << search << ": '" << helps[found] << "'" << endl;
    }
    else
    {
        cout << "The following commands are available: " << endl;
        for(int i = 0; cmds[i] != end; i++)
            cout << "\t- " << cmds[i] << endl;
    }

}
void errhelp(string message)
{
    cerr << message << endl << endl;
    argc = 2;
    help();
}

int main(int _argc, char const* _argv[])
{

    argc = _argc;
    argv = _argv;

    try
    {

        if(argc < 2)
        {
            errhelp("No command specified!");
            return -1;
        }

        string cmd = argv[1];

        pwd = PWD();
        medir = MEDIR();

        if(cmd == "help")
            help();
        else if(cmd == "init")
            init();
        else if(cmd == "include")
            include();
        else if(cmd == "add")
            add();
        else
            errhelp("'" + cmd + "' is not valid!\n");

    }
    catch(runtime_error e)
    {
        cerr << "ERROR: " << e.what() << endl;
        return -1;
    }
    
    return 0;

}