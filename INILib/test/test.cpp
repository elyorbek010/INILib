#include "INILib.hpp"

int main()
{
    INILib::INIFile file("../../INILib/example_files/example_input.ini");
    
    file["core"]["config_path"]["minesmth"].set("ElyorbekKholikov");

    file.save_as("../../INILib/example_files/out.ini");
}