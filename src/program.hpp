#include <string>
#include <regex>

class Program
{
public:
    std::wstring path;
    std::wstring dir;
    std::wstring args;
    bool include_app_path_in_args = true;

    Program(){};

    Program(const std::wstring path, const std::wstring dir, const std::wstring args)
    {
        this->path = path;
        this->dir = dir;
        this->args = args;
    }

    [[nodiscard]] std::wstring get_full_args() const;

    [[nodiscard]] std::wstring get_command(const std::wsmatch &) const;
};
