// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <ctime>
#include "simulation.h"
#include "single_relay.h"
#include "single_no_relay.h"
#include "relay_line.h"
#include "commandline_arguments.h"

namespace po = boost::program_options;

typedef kodo::full_rlnc_encoder<fifi::binary> Encoder;
typedef kodo::full_rlnc_decoder<fifi::binary> Decoder;

typedef kodo::full_rlnc_encoder<fifi::binary8> Encoder8;
typedef kodo::full_rlnc_decoder<fifi::binary8> Decoder8;

typedef kodo::full_rlnc_encoder<fifi::binary16> Encoder16;
typedef kodo::full_rlnc_decoder<fifi::binary16> Decoder16;


void run_benchmarks(int argc, const char *argv[])
{
    po::variables_map options = parse_commandline(argc, argv);

    if(options.count("help"))
        return;

    if(options.count("topology"))
    {
        std::cout << topology() << std::endl;
        return;
    }

    boost::random::mt19937 random_generator;
    random_generator.seed(time(0));

    std::string field = get_option<std::string>("field", options);

    if(field == "binary")
    {
        run_relay_line<Encoder, Decoder>(options, random_generator);
    }
    else if(field == "binary8")
    {
        run_relay_line<Encoder8, Decoder8>(options, random_generator);
    }
    else if(field == "binary16")
    {
        run_relay_line<Encoder16, Decoder16>(options, random_generator);
    }
    else
    {
        throw std::runtime_error("Unknown field");
    }
}


int main(int argc, const char *argv[])
{
    try
    {
        run_benchmarks(argc, argv);
    }
    catch(const std::exception &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    std::cout << "Simulation exit, run with --help to see options" << std::endl;

    return 0;
}
