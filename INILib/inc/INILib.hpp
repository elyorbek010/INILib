#ifndef INILIB_HPP
#define INILIB_HPP

#include <fstream>
#include <memory>

namespace INILib
{
class INITree;
class INIFile;
using INIFileImpl = std::shared_ptr<INITree>;

class INIValue
{
    friend class INISection;

    public:
    std::string get();

    void set(std::string value);

    INIValue operator[](std::string context)
    {
        key.resize(key.find_last_of('.') + 1); // trim the context part
        key.append(context);

        return *this;
    }

    private:
    INIValue(INIFileImpl impl, std::string key)
        : impl(impl), key(key + ".__DEFAULT__")
    { }

    private:
    INIFileImpl impl;
    std::string key;
};

class INISection
{
    friend class INIFile;

    public:
    INIValue operator[](std::string key)
    {
        return INIValue(impl, section + "." + key);
    }

    private:
    INISection(INIFileImpl impl, std::string section)
        : impl(impl), section(section)
    { }

    private:
    INIFileImpl impl;
    std::string section;
};

class INIFile
{
    public:
    // Constructor
    INIFile(std::string input_file_name)
    {
        std::ifstream input_file(input_file_name, std::ios_base::in);
        if(!input_file)
            throw std::runtime_error(std::string("Ifstream failed opening: ") + input_file_name);

        impl = render(std::move(input_file));
    }

    // Copy Constructor
    INIFile(const INIFile& other)
    {
        impl = other.impl;
    }

    INISection operator[](std::string section)
    {
        return INISection(impl, section);
    }

    void save_as(std::string output_file_name);

    private:
    INIFileImpl render(std::ifstream&& input_file);

    INIFileImpl impl;
};

} // namespace INILib

#endif // INILIB_HPP