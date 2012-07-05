#ifndef BASIC_SIMULATIONS_RELAY_SIMULATIONS_COMMANDLINE_ARGUMENTS_H
#define BASIC_SIMULATIONS_RELAY_SIMULATIONS_COMMANDLINE_ARGUMENTS_H

#include <boost/program_options.hpp>

/// Create a options map
boost::program_options::variables_map parse_commandline(int argc, const char *argv[]);


#endif
