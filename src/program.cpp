#include "program.hpp"
#include <format>

std::wstring Program::get_full_args() const
{
    if (this->include_app_path_in_args)
    {
        return this->path + L" " + this->args;
    }
    else
    {
        return this->args;
    }
}

std::wstring Program::get_command(const std::wsmatch &match) const
{
    std::wstring ret_val = this->get_full_args();
    for (int i = 0; i < 10; i++)
    {
        auto search_txt = std::format(L"{}", i);
        search_txt = L"{" + search_txt + L"}";
        auto rx_fmt = std::format(L"{}", i);
        rx_fmt = L"\\{" + rx_fmt + L"\\}";
        auto m_str = match[i].str();
        if (ret_val.find(search_txt) != std::wstring::npos)
        {
            ret_val = std::regex_replace(ret_val, std::wregex(rx_fmt), m_str);
        }
    }

    return ret_val;
}