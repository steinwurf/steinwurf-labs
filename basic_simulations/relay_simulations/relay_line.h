// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing
#ifndef BASIC_SIMULATIONS_RELAY_SIMULATION_RELAY_LINE_H
#define BASIC_SIMULATIONS_RELAY_SIMULATION_RELAY_LINE_H

#include <ctime>

#include "commandline_arguments.h"
#include "simulation.h"

/// @return an ascii-art drawing of the topology
inline std::string topology()
{
    return std::string("                                                    \n"
                       "      .------------------------------------.        \n"
                       "      |                                    |        \n"
                       "      |              +--------+            |        \n"
                       "      +              | relay  |            v        \n"
                       "  +--------+         +--------+        +---------+  \n"
                       "  | source | +--->              +----> | sink    |  \n"
                       "  +--------+         +--------+        +---------+  \n"
                       "                     | relay  |                     \n"
                       "                     +--------+                     \n"
                       "                         x                          \n"
                       "                         x                          \n"
                       "                         x                          \n"
                       "                         x                          \n"
                       "                     +--------+                     \n"
                       "                     | relay  |                     \n"
                       "                     +--------+                     \n"
                       "                                                    \n");
}


inline void relay_line(const boost::program_options::variables_map &options,
                       boost::shared_ptr<simulation_factory> factory)
{

    bool recode =
        get_option<bool>("recode", options);

    bool source_systematic =
        get_option<bool>("source_systematic", options);

    double error_relays_sink =
        get_option<double>("error_relays_sink", options);

    double error_source_sink =
        get_option<double>("error_source_sink", options);

    double error_source_relays =
        get_option<double>("error_source_relays", options);

    uint32_t number_relays =
        get_option<uint32_t>("relays", options);

    bool relay_transmit_on_receive =
        get_option<bool>("relay_transmit_on_receive", options);

    boost::shared_ptr<tick_scheduler> scheduler = factory->scheduler();

    boost::shared_ptr<channel> channel_source_relay =
        factory->build_channel(error_source_relays);

    boost::shared_ptr<channel> channel_relay_sink =
        factory->build_channel(error_relays_sink);

    boost::shared_ptr<channel> channel_source_sink =
        factory->build_channel(error_source_sink);

    std::vector< boost::shared_ptr<relay> > relays;

    for(uint32_t i = 0; i < number_relays; ++i)
    {
        std::string id = "relay" + to_string(i);
        boost::shared_ptr<relay> node_relay = factory->build_relay(id);

        scheduler->add_node(node_relay);

        if(recode)
        {
            node_relay->set_recode_on();
        }
        else
        {
            node_relay->set_recode_off();
        }

        node_relay->set_transmit_on_receive(relay_transmit_on_receive);

        relays.push_back(node_relay);

        channel_source_relay->add_node(node_relay);
        node_relay->add_node(channel_relay_sink);
    }

    boost::shared_ptr<source> node_source = factory->build_source("source");
    scheduler->add_node(node_source);

    if(source_systematic)
    {
        node_source->systematic_on();
    }
    else
    {
        node_source->systematic_off();
    }

    boost::shared_ptr<sink> node_sink = factory->build_sink("sink");
    scheduler->add_node(node_sink);

    // Wire-up the channels
    node_source->add_node(channel_source_relay);
    node_source->add_node(channel_source_sink);

    channel_relay_sink->add_node(node_sink);
    channel_source_sink->add_node(node_sink);

    while(!node_sink->is_complete())
    {
        scheduler->tick();
    }

    boost::shared_ptr<counter_list> c =
        factory->counter();

    c->value<uint32_t>("number_relays") = number_relays;
    c->value<uint32_t>("recoding") = recode;
    c->value<uint32_t>("source_systematic") = source_systematic;
    c->value<double>("error_source_sink") = error_source_sink;
    c->value<double>("error_relays_sink") = error_relays_sink;
    c->value<double>("error_source_relays") = error_source_relays;

}

template<class Encoder, class Decoder>
inline void run_relay_line(const boost::program_options::variables_map &options,
                           boost::random::mt19937 &random)
{
    uint32_t generation_size = get_option<uint32_t>("generation_size", options);
    uint32_t packet_size = get_option<uint32_t>("packet_size", options);
    uint32_t iterations = get_option<uint32_t>("iterations", options);
    typedef basic_simulation_factory<Encoder, Decoder> factory_type;

    {
        boost::shared_ptr<factory_type> factory =
            boost::make_shared<factory_type>(generation_size,
                                             packet_size,
                                             boost::ref(random));

        boost::shared_ptr<counter_list> c =
            factory->counter();

        c->value<uint32_t>("generation_size") = generation_size;
        c->value<uint32_t>("packet_size") = packet_size;
        c->value<uint32_t>("iterations") = iterations;

        for(uint32_t i = 0; i < iterations; ++i)
        {
            std::cout << "Running relay line ( " << i
                      << " / " << iterations << " )" << std::endl;
            relay_line(options, factory);
        }

        std::string filter = get_option<std::string>("filter", options);
        c->print(std::cout, filter);

        std::cout << std::endl;

    }



}

#endif

