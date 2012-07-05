// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing
#ifndef BASIC_SIMULATIONS_RELAY_SIMULATION_RELAY_LINE_H
#define BASIC_SIMULATIONS_RELAY_SIMULATION_RELAY_LINE_H

#include "simulation.h"
#include <ctime>

struct relay_line_setup
{
    uint32_t m_generation_size;
    uint32_t m_packet_size;
    double m_error_relays_sink;
    double m_error_source_relays;
    uint32_t m_number_relays;
    bool m_recode;
    bool m_source_systematic;
};

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
inline void relay_line(relay_line_setup setup,
                       boost::shared_ptr<simulation_factory> factory)
{

    std::cout << "Running relay line" << std::endl;
    //std::cout << "relays->sink = " << relays_sink << std::endl;
    //std::cout << "source->relays = " << source_relays << std::endl;
    //std::cout << "number_relays = " << number_relays << std::endl;

    boost::shared_ptr<channel> channel_source_relay =
        factory->build_channel(setup.m_error_source_relays);

    boost::shared_ptr<channel> channel_relay_sink =
        factory->build_channel(setup.m_error_relays_sink);

    std::vector< boost::shared_ptr<relay> > relays;

    for(uint32_t i = 0; i < setup.m_number_relays; ++i)
    {
        std::string id = "relay" + to_string(i);
        boost::shared_ptr<relay> node_relay = factory->build_relay(id);

        if(setup.m_recode)
            node_relay->set_recode_on();
        else
            node_relay->set_recode_off();

        relays.push_back(node_relay);

        channel_source_relay->add_node(node_relay);
        node_relay->add_node(channel_relay_sink);
    }

    boost::shared_ptr<source> node_source = factory->build_source("source");

    if(setup.m_source_systematic)
        node_source->systematic_on();
    else
        node_source->systematic_off();

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

    c->value<uint32_t>("number_relays") = setup.m_number_relays;
    c->value<uint32_t>("recoding") = setup.m_recode;
    c->value<uint32_t>("generation_size") = setup.m_generation_size;
    c->value<uint32_t>("packet_size") = setup.m_packet_size;
    c->value<uint32_t>("source_systematic") = setup.m_source_systematic;
    c->value<double>("error_relays_sink") = setup.m_error_relays_sink;
    c->value<double>("error_source_relays") = setup.m_error_source_relays;

    c->print(std::cout);

    std::cout << std::endl;
}

template<class Encoder, class Decoder>
inline void run_relay_line(relay_line_setup setup, boost::random::mt19937 &random)
{
    typedef basic_simulation_factory<Encoder, Decoder> factory_type;

    {
        boost::shared_ptr<factory_type> factory =
            boost::make_shared<factory_type>(setup.m_generation_size,
                                             setup.m_packet_size,
                                             boost::ref(random));

        relay_line(setup, factory);
    }



}

#endif

