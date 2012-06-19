// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

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
    node_source->add_output(channel_source_sink);
    node_source->add_output(channel_source_relay);

    channel_source_sink->add_output(node_sink);
    channel_source_relay->add_output(node_relay);

    node_relay->add_output(channel_relay_sink);

    channel_relay_sink->add_output(node_sink);

    while(!node_sink->is_complete())
    {
        node_source->send();
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

    uint32_t iterations = 10;

    double max = 0.5;
    double min = 0.0;
    double step = 0.1;

    for(double rs = min; rs <= max; rs += step)
    {
        for(double ss = min; ss <= max; ss += step)
        {
            for(double sr = min; sr <= max; sr += step)
            {

                // to run multiple times use for loop e.g.
                c->value<uint32_t>("iterations") = iterations;
                for(uint32_t i = 0; i < iterations; ++i)
                    single_relay(rs, ss, sr, false, factory);

                c->new_run();
                c->value<uint32_t>("iterations") = iterations;
                for(uint32_t i = 0; i < iterations; ++i)
                    single_relay(rs, ss, sr, true, factory);
                c->new_run();
            }
        }
    }


    // Print the counters
    c->print(std::cout);
    c->dump_to_file();

    std::cout << std::endl;


}

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

    // Wire-up the channels
    node_source->add_output(channel_source_sink);
    channel_source_sink->add_output(node_sink);

    while(!node_sink->is_complete())
    {
        node_source->send();
    }

    boost::shared_ptr<counter_list> c =
        factory->counter();

    c->print(std::cout);

    std::cout << std::endl;
}

template<class Encoder, class Decoder>
void run_single_no_relay(boost::random::mt19937 &random)
{
    typedef basic_simulation_factory<Encoder, Decoder> factory_type;

    boost::shared_ptr<factory_type> factory =
        boost::make_shared<factory_type>(1024, 1400, boost::ref(random));

    single_no_relay(0.4, factory);
}

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
                       boost::shared_ptr<simulation_factory> factory)
{

    std::cout << "Running relay line" << std::endl;
    std::cout << "relays->sink = " << relays_sink << std::endl;
    std::cout << "source->relays = " << source_relays << std::endl;
    std::cout << "number_relays = " << number_relays << std::endl;

    boost::shared_ptr<channel> channel_source_relay = factory->build_channel(source_relays);
    boost::shared_ptr<channel> channel_relay_sink = factory->build_channel(relays_sink);

    std::vector< boost::shared_ptr<relay> > relays;

    for(uint32_t i = 0; i < number_relays; ++i)
    {
        boost::shared_ptr<relay> node_relay = factory->build_relay();
        relays.push_back(node_relay);

        channel_source_relay->add_output(node_relay);
        node_relay->add_output(channel_relay_sink);
    }

    boost::shared_ptr<source> node_source = factory->build_source();
    boost::shared_ptr<sink> node_sink = factory->build_sink();


    // Wire-up the channels
    node_source->add_output(channel_source_relay);
    channel_relay_sink->add_output(node_sink);

    while(!node_sink->is_complete())
    {
        node_source->send();
    }

    boost::shared_ptr<counter_list> c =
        factory->counter();

    c->print(std::cout);

    std::cout << std::endl;
}

template<class Encoder, class Decoder>
void run_relay_line(boost::random::mt19937 &random)
{
    typedef basic_simulation_factory<Encoder, Decoder> factory_type;

    {
        boost::shared_ptr<factory_type> factory =
            boost::make_shared<factory_type>(1024, 1400, boost::ref(random));

        relay_line(0.2, 0.8, 4, factory);
    }

    {
        boost::shared_ptr<factory_type> factory =
            boost::make_shared<factory_type>(1024, 1400, boost::ref(random));

        relay_line(0.5, 0.5, 4, factory);
    }

    {
        boost::shared_ptr<factory_type> factory =
            boost::make_shared<factory_type>(1024, 1400, boost::ref(random));

        relay_line(0.8, 0.2, 4, factory);
    }

}



int main()
{

    boost::random::mt19937 random_generator;
    random_generator.seed(time(0));

    typedef kodo::full_rlnc_encoder<fifi::binary> Encoder;
    typedef kodo::full_rlnc_decoder<fifi::binary> Decoder;

    typedef kodo::full_rlnc_encoder<fifi::binary8> Encoder8;
    typedef kodo::full_rlnc_decoder<fifi::binary8> Decoder8;

    run_single_relay<Encoder, Decoder>(random_generator);
    //run_single_no_relay<Encoder8, Decoder8>(random_generator);
    //run_relay_line<Encoder8, Decoder8>(random_generator);

    return 0;
}
