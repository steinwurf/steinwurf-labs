// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing
#ifndef BASIC_SIMULATIONS_RELAY_SIMULATION_SINGLE_NO_RELAY_H
#define BASIC_SIMULATIONS_RELAY_SIMULATION_SINGLE_NO_RELAY_H


#include "simulation.h"
#include <ctime>

//
//  +--------+                      +---------+
//  | source | +------------------> | sink    |
//  +--------+                      +---------+
//
inline void single_no_relay(double source_sink,
                            boost::shared_ptr<simulation_factory> factory)
{

    std::cout << "Running single no relay" << std::endl;
    std::cout << "source->sink = " << source_sink << std::endl;

    boost::shared_ptr<channel> channel_source_sink = factory->build_channel(source_sink);

    boost::shared_ptr<source> node_source = factory->build_source();
    boost::shared_ptr<sink> node_sink = factory->build_sink();
    boost::shared_ptr<tick_scheduler> scheduler = factory->scheduler();

    // Wire-up the channels
    node_source->add_node(channel_source_sink);
    channel_source_sink->add_node(node_sink);

    while(!node_sink->is_complete())
    {
        scheduler->tick();
    }

    boost::shared_ptr<counter_list> c =
        factory->counter();

    c->print(std::cout);

    std::cout << std::endl;
}

template<class Encoder, class Decoder>
inline void run_single_no_relay(boost::random::mt19937 &random)
{
    typedef basic_simulation_factory<Encoder, Decoder> factory_type;

    boost::shared_ptr<factory_type> factory =
        boost::make_shared<factory_type>(1024, 1400, boost::ref(random));

    single_no_relay(0.5, factory);
}

#endif

