// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing
#ifndef BASIC_SIMULATIONS_RELAY_SIMULATION_SINGLE_RELAY_H
#define BASIC_SIMULATIONS_RELAY_SIMULATION_SINGLE_RELAY_H

#include "simulation.h"
#include <ctime>

//
//                  +--------+
//        +-------> |  relay | +---------+
//        |         +--------+           |
//        +                              v
//  +--------+                      +---------+
//  | source | +------------------> | sink    |
//  +--------+                      +---------+
//
//
// @param relay_sink the error probability from relay to sink
// @param source_sink the error probabilty from source to sink
// @param source_relay the error probability from source to relay
// @param source_systematic whether the source is initially systematic
// @param factory simulation factory
inline void single_relay(double relay_sink,
                         double source_sink,
                         double source_relay,
                         bool source_systematic,
                         boost::shared_ptr<simulation_factory> factory)
{

    std::cout << "Running single relay" << std::endl;
    std::cout << "relay->sink = " << relay_sink << std::endl;
    std::cout << "source->sink = " << source_sink << std::endl;
    std::cout << "source->relay = " << source_relay << std::endl;

    boost::shared_ptr<counter_list> c =
        factory->counter();

    c->value<std::string>("test_name") = "single_relay";
    c->value<double>("error_relay_to_sink") = relay_sink;
    c->value<double>("error_source_to_sink") = source_sink;
    c->value<double>("error_source_to_relay") = source_relay;

    boost::shared_ptr<channel> channel_relay_sink =
        factory->build_channel(relay_sink);

    boost::shared_ptr<channel> channel_source_sink =
        factory->build_channel(source_sink);

    boost::shared_ptr<channel> channel_source_relay =
        factory->build_channel(source_relay);

    boost::shared_ptr<source> node_source = factory->build_source();

    c->value<bool>("source_systematic") = source_systematic;
    if(source_systematic)
    {
        node_source->systematic_on();
    }
    else
    {
        node_source->systematic_off();
    }

    boost::shared_ptr<sink> node_sink = factory->build_sink();
    boost::shared_ptr<relay> node_relay = factory->build_relay();

    // Wire-up the channels
    node_source->add_node(channel_source_sink);
    node_source->add_node(channel_source_relay);

    channel_source_sink->add_node(node_sink);
    channel_source_relay->add_node(node_relay);

    node_relay->add_node(channel_relay_sink);

    channel_relay_sink->add_node(node_sink);

    boost::shared_ptr<tick_scheduler> scheduler =
        factory->scheduler();


    while(!node_sink->is_complete())
    {
        scheduler->tick();
    }

}

template<class Encoder, class Decoder>
void run_single_relay(boost::random::mt19937 &random)
{
    typedef basic_simulation_factory<Encoder, Decoder> factory_type;

    boost::shared_ptr<factory_type> factory =
        boost::make_shared<factory_type>(32, 1400, boost::ref(random));

    // Get the statistics
    boost::shared_ptr<counter_list> c =
        factory->counter();

    single_relay(0.5, 0.5, 0.5, false, factory);
    c->new_run();
    single_relay(0.5, 0.5, 0.5, true, factory);

    // Print the counters
    c->print(std::cout);
    c->dump_to_file();

    std::cout << std::endl;


}

#endif


