#include <stdexcept>
#include <map>

#include "INILib.hpp"
#include "INITokenizer.hpp"

//#include "tsl/ordered_map.h"

namespace INILib
{
class INITree
{
    public:

    INITree(const std::list<Token>& tokens_list)
    {
        std::string global_section = "__GLOBAL__";
        std::string cur_sec = global_section;
        std::string cur_key;
        std::string cur_context = "__DEFAULT__";
        std::string cur_value;

        for(const auto& token: tokens_list)
        {
            if (token.type == TokenType::Section)
                cur_sec = token.value;
            else if (token.type == TokenType::Key)
                cur_key = token.value;
            else if (token.type == TokenType::Context)
                cur_context = token.value;
            else
            {
                cur_value = token.value;
                auto result = tree.try_emplace(cur_sec + "." + cur_key + "." + cur_context, cur_value);
                if (result.second == false)
                    throw std::runtime_error(std::string("Value already exists"));

                cur_context = "__DEFAULT__"; // reset context value
            }
        }
    }

    std::string find(std::string key)
    {
        auto kv_pair = tree.find(key);
        if (kv_pair == tree.cend())
            throw std::runtime_error(std::string("key does not exist"));
        
        return kv_pair->second;
    }

    void insert_or_assign(std::string key, std::string value)
    {
        tree.insert_or_assign(std::move(key), value);
    }

    void save_as(std::string output_file_name)
    {
        std::ofstream output_file(output_file_name);
        if(!output_file)
            throw std::runtime_error(std::string("Ofstream failed opening: ") + output_file_name);

        std::stringstream ss;

        std::string global_section = "__GLOBAL__";
        std::string cur_sec = global_section;
        std::string prev_sec = cur_sec;
        std::string cur_key;
        std::string cur_context = "__DEFAULT__";
        std::string cur_value;

        for(const auto& kvpair: tree)
        {
            ss.clear();
            ss.str(kvpair.first);

            std::getline(ss, cur_sec, '.');     // section
            std::getline(ss, cur_key, '.');     // key
            std::getline(ss, cur_context, '.'); // context
            
            // print [section]
            if (prev_sec != cur_sec)
            {
                output_file << "[" << cur_sec << "]" << std::endl;
                prev_sec = cur_sec;
            }

            // print key
            output_file << cur_key;

            // print <context>
            if (cur_context != "__DEFAULT__")
                output_file << "<" << cur_context << ">";

            // print ' = '
            output_file << " = ";

            // print value
            output_file << kvpair.second << std::endl;
        }

    }

    private:
    //tsl::ordered_map<std::string, std::string> tree; // if we use this, when adding new kv_pair, it'll be placed on the bottom of the .ini file
    std::map<std::string, std::string> tree;
};

INIFileImpl INIFile::render(std::ifstream&& input_file_stream)
{
    Tokenizer fsm;
    tokens.clear();
    
    std::string line;
    while(std::getline(input_file_stream, line))
    {
        fsm.start();

        for (char& ch : line)
            fsm.process_event(ProcessCharacter(ch));
        
        fsm.stop();
    }

    return std::make_shared<INITree>(tokens);
}

std::string INIValue::get()
{
    return impl->find(key);
}

void INIValue::set(std::string value)
{
    impl->insert_or_assign(key, value);
}

void INIFile::save_as(std::string output_file_name)
{
    impl->save_as(output_file_name);
}

} // namespace INILib