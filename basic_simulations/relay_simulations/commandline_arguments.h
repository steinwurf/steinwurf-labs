#ifndef BASIC_SIMULATIONS_RELAY_SIMULATIONS_COMMANDLINE_ARGUMENTS_H
#define BASIC_SIMULATIONS_RELAY_SIMULATIONS_COMMANDLINE_ARGUMENTS_H

#include <boost/program_options.hpp>

/// Returns an option
/// @param key the options name
/// @param v variable map containing the parsed options
/// @return the value
template<class T>
inline T get_option(const std::string key,
                    const boost::program_options::variables_map &v)
{
    T t;
    if(!v.count(key))
    {
        throw std::runtime_error("You have to specify "+key+" use"
                                 " --help to see more");
    }
    else
    {
        t = v[key].as<T>();
    }
    return t;
}

/// Create a options map
boost::program_options::variables_map parse_commandline(int argc, const char *argv[]);

#endif
