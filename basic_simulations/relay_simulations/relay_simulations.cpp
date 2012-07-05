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

template<class T>
T get_option(const std::string key, const po::variables_map &v)
{
    T t;
    if(!v.count(key))
    {
        throw std::runtime_error("You have to specify "+key+" use --help to see more");
    }
    else
    {
        t = v[key].as<T>();
    }
    return t;
}


int main(int argc, const char *argv[])
{
    po::variables_map options = parse_commandline(argc, argv);

    if(options.count("help"))
        return 0;

    std::string scenario = get_option<std::string>("scenario", options);
    std::string field = get_option<std::string>("field", options);
    uint32_t generation_size = get_option<uint32_t>("generation_size", options);
    uint32_t packet_size = get_option<uint32_t>("packet_size", options);
    bool recode = get_option<bool>("recode", options);
    bool source_systematic = get_option<bool>("source_systematic", options);

    boost::random::mt19937 random_generator;
    random_generator.seed(time(0));

    typedef kodo::full_rlnc_encoder<fifi::binary> Encoder;
    typedef kodo::full_rlnc_decoder<fifi::binary> Decoder;

    typedef kodo::full_rlnc_encoder<fifi::binary8> Encoder8;
    typedef kodo::full_rlnc_decoder<fifi::binary8> Decoder8;

    if(scenario == "relay_line")
    {
        relay_line_setup setup;
        setup.m_generation_size = generation_size;
        setup.m_packet_size = packet_size;
        setup.m_error_relays_sink = get_option<double>("rl_error_relays_sink", options);
        setup.m_error_source_relays = get_option<double>("rl_error_source_relays",options);
        setup.m_number_relays = get_option<uint32_t>("rl_relays",options);
        setup.m_recode = recode;
        setup.m_source_systematic = source_systematic;


        if(field == "binary")
        {
            run_relay_line<Encoder, Decoder>(setup, random_generator);
        }
        else if(field == "binary8")
        {
            run_relay_line<Encoder8, Decoder8>(setup, random_generator);
        }
        else
        {
            throw std::runtime_error("Unknown field");
        }
    }
    else
    {
        throw std::runtime_error("Not implemented");
    }

//    run_single_relay<Encoder, Decoder>(random_generator);
    //run_single_no_relay<Encoder8, Decoder8>(random_generator);
    //

    return 0;
}
