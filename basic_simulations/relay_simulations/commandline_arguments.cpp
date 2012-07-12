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
        ("topology", "show the topology")
        ("field", po::value<std::string>()->default_value("binary"),
         "Select a finite field for example --field=binary8 "
         "available: binary, binary8, binary16")
        ("filter", po::value<std::string>()->default_value(""),
         "Only prints counters which match the filter "
         "example --filter=source_sent")
        ("source_systematic", po::value<bool>()->default_value(true),
         "whether the source is systematic or not --systematic=1 "
         "turns on systematic source. Systematic means that all packets "
         "in a generation are sent first once without coding. After sending "
         "everything once coding starts.")
        ("generation_size", po::value<uint32_t>()->default_value(32),
         "the generation size, the number of packets which are coded together.")
        ("packet_size", po::value<uint32_t>()->default_value(100),
         "the packet size")
        ("iterations", po::value<uint32_t>()->default_value(1),
         "how many times to run the simulation")
        ("recode", po::value<bool>()->default_value(true),
         "recoding at relay or not --recode=1 turns on recoding")
        ("relays", po::value<uint32_t>()->default_value(1),
         "number of relays example --relays=3")
        ("error_source_sink", po::value<double>()->default_value(0.5),
         "error probability between source and sink "
         "example --error_source_sink=0.5")
        ("error_source_relays", po::value<double>()->default_value(0.5),
         "error probability between source and relays "
         "example --error_source_relays=0.5")
        ("error_relays_sink", po::value<double>()->default_value(0.5),
         "error probability between relays and sink "
         "example --error_relays_sink=0.5");


    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
    }

    return vm;
}


