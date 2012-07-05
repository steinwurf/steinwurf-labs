// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing
#ifndef BASIC_SIMULATIONS_RELAY_SIMULATION_RELAY_LINE_H
#define BASIC_SIMULATIONS_RELAY_SIMULATION_RELAY_LINE_H

#include "simulation.h"
#include <ctime>

//
//                     +--------+
//                     | relay  |
//                     +--------+
//
//                     +--------+
//                     | relay  |
//  +--------+         +--------+        +---------+
//  | source | +--->              +----> | sink    |
//  +--------+         +--------+        +---------+
//                     | relay  |
//                     +--------+
//                         x
//                         x
//                         x
//                         x
//                     +--------+
//                     | relay  |
//                     +--------+
//
inline void relay_line(double relays_sink,
                       double source_relays,
                       uint32_t number_relays,
                       bool recode,
                       boost::shared_ptr<simulation_factory> factory)
{

    std::cout << "Running relay line" << std::endl;
    std::cout << "relays->sink = " << relays_sink << std::endl;
    std::cout << "source->relays = " << source_relays << std::endl;
    std::cout << "number_relays = " << number_relays << std::endl;

    boost::shared_ptr<channel> channel_source_relay =
        factory->build_channel(source_relays);

    boost::shared_ptr<channel> channel_relay_sink =
        factory->build_channel(relays_sink);

    std::vector< boost::shared_ptr<relay> > relays;

    for(uint32_t i = 0; i < number_relays; ++i)
    {
        std::string id = "relay" + to_string(i);
        boost::shared_ptr<relay> node_relay = factory->build_relay(id);

        if(recode)
            node_relay->set_recode_on();
        else
            node_relay->set_recode_off();

        relays.push_back(node_relay);

        channel_source_relay->add_node(node_relay);
        node_relay->add_node(channel_relay_sink);
    }

    boost::shared_ptr<source> node_source = factory->build_source("source");
    boost::shared_ptr<sink> node_sink = factory->build_sink("sink");
    boost::shared_ptr<tick_scheduler> scheduler = factory->scheduler();

    // Wire-up the channels
    node_source->add_node(channel_source_relay);
    channel_relay_sink->add_node(node_sink);

    while(!node_sink->is_complete())
    {
        scheduler->tick();
    }

    boost::shared_ptr<counter_list> c =
        factory->counter();

    c->value<std::string>("recoding "+to_string(recode));

    c->print(std::cout);

    std::cout << std::endl;
}

template<class Encoder, class Decoder>
inline void run_relay_line(boost::random::mt19937 &random)
{
    typedef basic_simulation_factory<Encoder, Decoder> factory_type;

    // {
    //     boost::shared_ptr<factory_type> factory =
    //         boost::make_shared<factory_type>(1024, 1400, boost::ref(random));
    //
    //     relay_line(0.2, 0.8, 4, true, factory);
    // }

    {
        boost::shared_ptr<factory_type> factory =
            boost::make_shared<factory_type>(1024, 1400, boost::ref(random));

        relay_line(0.5, 0.5, 3, false, factory);
    }

    // {
    //     boost::shared_ptr<factory_type> factory =
    //         boost::make_shared<factory_type>(1024, 1400, boost::ref(random));
    //
    //     relay_line(0.8, 0.2, 4, true, factory);
    // }

}

#endif

