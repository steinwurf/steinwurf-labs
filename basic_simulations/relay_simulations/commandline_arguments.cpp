#include "commandline_arguments.h"

#include <vector>
#include <algorithm>


namespace po = boost::program_options;

po::variables_map parse_commandline(int argc, const char *argv[])
{
    // Declare the supported options.
    po::options_description desc("Options for basic simulations");

    desc.add_options()
        ("help", "produce help message")
        ("scenario", po::value<std::string>(),
         "Select a scenario for example --scenario=relay_line "
         "available: relay_line, single_relay and single_no_relay")
        ("field", po::value<std::string>(),
         "Select a finite field for example --field=binary8 "
         "available: binary and binary8")
        ("source_systematic", po::value<bool>(),
         "whether the source is systematic or not")
        ("generation_size", po::value<uint32_t>(),
         "the generation size")
        ("packet_size", po::value<uint32_t>(),
         "the packet size")
        ("recode", po::value<bool>(),
         "recoding at relay or not --recode=1 turns on recoding");


    // Declare a group of options that will be
    // allowed both on command line and in
    // config file
    po::options_description relay_line("relay_line Options");
    relay_line.add_options()
        ("rl_relays", po::value<uint32_t>(),
         "number of relays")
        ("rl_error_source_relays", po::value<double>(),
         "error probability between source and relays")
        ("rl_error_relays_sink", po::value<double>(),
         "error probability between relays and sink");

    desc.add(relay_line);

    po::options_description single_relay("single_relay Options");
    single_relay.add_options()
        ("sr_error_source_relay", po::value<double>(),
         "error probability between source and relay")
        ("sr_error_source_sink", po::value<double>(),
         "error probability between source and sink")
        ("sr_error_relay_sink", po::value<double>(),
         "error probability between relay and sink");

    desc.add(single_relay);

    po::options_description single_no_relay("single_no_relay Options");
    single_no_relay.add_options()
        ("snr_error_source_sink", po::value<double>(),
         "enrror probability between source and sink");

    desc.add(single_no_relay);


    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
    }

    return vm;
}


