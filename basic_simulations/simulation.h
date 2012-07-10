// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef STEINWURF_KODO_SIMULATIONS_SIMULATION_H
#define STEINWURF_KODO_SIMULATIONS_SIMULATION_H

#include <kodo/rlnc/full_vector_codes.h>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/variant.hpp>

#include <fstream>
#include <sstream>

class value_printer
    : public boost::static_visitor<std::string>
{
public:

    template<class T>
    std::string operator()(T t) const
    {
        std::stringstream ss;
        ss << t;
        return ss.str();
    }

    std::string operator()(const std::string& s) const
    {
        std::stringstream ss;
        ss << "\"" << s << "\"";
        return ss.str();
    }

};

// Access to counters based on a key
struct counter_list
{
    // Value type
    typedef boost::variant<uint32_t, std::string, double, bool> value_type;

    // Counter data
    typedef std::map<std::string, value_type> counter_data;

    template<class ValueType>
    ValueType& value(const std::string &key)
        {
            if(m_values.size() == 0)
            {
                // make sure we have space
                m_values.resize(1);
            }

            uint32_t index = m_values.size() - 1;

            if(m_values[index].find(key) == m_values[index].end())
                m_values[index][key] = ValueType();

            return boost::get<ValueType>(m_values[index][key]);
        }


    bool accept(const std::string &s, const std::string &filter)
        {
            if(filter == "")
            {
                return true;
            }
            else if(std::string::npos != s.find(filter))
            {
                return true;
            }
            else
            {
                return false;
            }
        }

    void print(std::ostream &o, const std::string &filter = "")
        {
            std::vector<counter_data>::iterator it_values;

            uint32_t runs = 0;

            for(it_values = m_values.begin();
                it_values != m_values.end(); ++it_values)
                {
                    std::cout << "Run: " << runs << std::endl;

                    counter_data::iterator it_data;

                    for(it_data = it_values->begin();
                        it_data != it_values->end(); ++it_data)
                    {
                        if(accept(it_data->first, filter))
                        {
                            o << it_data->first << " "
                              << it_data->second << std::endl;
                        }
                    }
                    std::cout << std::endl;

                    ++runs;
                }
        }


    void dump_to_file()
        {
            std::ofstream out("results.py", std::ios::trunc);

            std::vector<counter_data>::iterator it_values;

            out << "results = [";

            for(it_values = m_values.begin();
                it_values != m_values.end(); ++it_values)
            {
                counter_data::iterator it_data;

                out << "{";

                for(it_data = it_values->begin();
                    it_data != it_values->end(); ++it_data)
                {
                    out << "\"" << it_data->first << "\": ";
                    out << boost::apply_visitor(value_printer(), (it_data->second));
                    out << ",";
                }
                out << "},";
            }

            out << "]\n";

        }

    void new_run()
        {
            m_values.push_back(counter_data());
        }


    std::vector<counter_data> m_values;
};

/// Simple packet with copy-on-write semantics
class packet
{
    struct impl
    {
        impl()
            {}

        impl(const impl &i)
            : m_data(i.m_data)
            {}

        impl(const std::vector<uint8_t> &d)
            : m_data(d)
            {}

        std::vector<uint8_t> m_data;
        std::string m_sender_id;
    };

public:
    packet()
        : m_impl(boost::make_shared<impl>())
        { }

    packet(const std::vector<uint8_t> &d)
        : m_impl(boost::make_shared<impl>(d))
        { }

    void detach()
        {
            if( !m_impl.unique() )
            {
                m_impl = boost::make_shared<impl>(*m_impl);
            }
        }

    void set_data(const uint8_t *data, uint32_t size)
        {
            detach();
            m_impl->m_data.resize(size);
            std::copy(data, data + size, &m_impl->m_data[0]);
        }

    void set_sender(const std::string &sender_id)
        {
            m_impl->m_sender_id = sender_id;
        }

    const std::string& get_sender() const
        {
            return m_impl->m_sender_id;
        }

    std::string get_sender()
        {
            return m_impl->m_sender_id;
        }

    const uint8_t* get_data() const
        {
            return &m_impl->m_data[0];
        }

    uint8_t* get_data()
        {
            detach();
            return &m_impl->m_data[0];
        }

    const uint8_t* data_begin() const
        {
            return get_data();
        }

    const uint8_t* data_end() const
        {
            return get_data() + get_data_size();
        }

    uint32_t get_data_size() const
        {
            return m_impl->m_data.size();
        }

    boost::shared_ptr<impl> m_impl;
};


// The randomizer
class random_bool
{
public:
    random_bool(boost::random::mt19937 &random_generator, double true_probability)
        : m_random_generator(random_generator),
          m_distribution(true_probability)
        { }

    bool generate()
        {
            return m_distribution(m_random_generator);
        }

private:

    // The random generator
    boost::random::mt19937 &m_random_generator;

    // The distribution wrapping the random generator
    boost::random::bernoulli_distribution<> m_distribution;

};


class node
{
public:


    node(const std::string &id)
        : m_id(id)
        {}

    virtual ~node()
        {
            std::cout << "Node " << m_id << " died" << std::endl;
        }

    virtual void receive(packet payload) = 0;
    virtual void tick() = 0;

    // Adds a receiver to the output
    virtual void add_node(const boost::shared_ptr<node> &recv)
        {
            assert(recv);
            m_receivers.push_back(recv);
        }

    // Forwards a payload to the output
    void forward(uint32_t receiver_index, packet payload)
        {
            m_receivers[receiver_index]->receive(payload);
        }

    uint32_t receiver_count()
        {
            return m_receivers.size();
        }

    boost::shared_ptr<node> get_receiver(uint32_t receiver_index)
        {
            return m_receivers[receiver_index];
        }

    std::string node_id()
        {
            return m_id;
        }

    const std::string & node_id() const
        {
            return m_id;
        }

private:

    /// The id of this node
    std::string m_id;

    // Stores the output receivers
    std::vector< boost::shared_ptr<node> > m_receivers;
};


// Convenience class
class channel : public node
{
public:
    channel(const std::string &id)
        : node(id)
        {}
};

// Convenience class
class relay : public node
{
public:

    relay(const std::string &id)
        : node(id)
        {}

    virtual void set_recode_on() = 0;
    virtual void set_recode_off() = 0;
    virtual bool is_recode_on() const = 0;
};

class sink : public node
{
public:

    sink(const std::string &id)
        : node(id)
        {}

    /// @return true when the sink has completed decoding
    virtual bool is_complete() = 0;
};

class source : public node
{
public:

    source(const std::string &id)
        : node(id)
        {}

    virtual void systematic_off() = 0;
    virtual void systematic_on() = 0;

};

/// Simple scheduler task which sends a tick to each
/// node added
class tick_scheduler
{
public:

    typedef boost::shared_ptr<node> node_ptr;

    void add_node(const node_ptr &node)
        {
            m_nodes.push_back(node);
        }

    void tick()
        {
            for(uint32_t i = 0; i < m_nodes.size(); ++i)
            {
                m_nodes[i]->tick();
            }
        }

    // Stores the output receivers
    std::vector<node_ptr> m_nodes;
};

// Channel
class basic_channel : public channel
{

public:

    // Create a simple coin-flip channel
    basic_channel(const std::string &id,
                  const boost::shared_ptr<random_bool> &channel_condition,
                  const boost::shared_ptr<counter_list> &counter)
        : channel(id),
          m_channel_condition(channel_condition),
          m_counter(counter)
        {}

    // Receives a payload
    virtual void receive(packet payload)
        {
            for(uint32_t j = 0; j < receiver_count(); ++j)
            {
                std::string recv_id = get_receiver(j)->node_id();
                std::string sender_id = payload.get_sender();

                // If true we drop
                if(m_channel_condition->generate())
                {
                    ++m_counter->value<uint32_t>(node_id()+"_"+sender_id+"_to_"+recv_id+"_dropped");
                }
                else
                {
                    ++m_counter->value<uint32_t>(node_id()+"_"+sender_id+"_to_"+recv_id+"_ok");

                    // Deliver packet to receiver j
                    forward(j, payload);
                }
            }

        }

    virtual void tick()
        {
            // Channels just forward only sinks, relays, sources
            // use the tick()
        }

private:

    // Incoming packet queue
    std::vector<packet> m_packets;

    // The channel
    boost::shared_ptr<random_bool> m_channel_condition;

    // Statistics
    boost::shared_ptr<counter_list> m_counter;
};



// Sink
template<class Decoder>
class basic_sink : public sink
{
public:

    basic_sink(const std::string &id,
               const typename Decoder::pointer &decoder,
               const boost::shared_ptr<counter_list> &counter)
        : sink(id),
          m_decoder(decoder),
          m_counter(counter)
        {
            assert(m_decoder);
            assert(m_counter);

            m_decode_buffer.resize(m_decoder->payload_size());
        }

    void tick()
        {
            // no implementation needed
        }

    void receive(packet payload)
        {
            assert(payload.get_data_size() > 0);
            std::string sender = payload.get_sender();

            ++m_counter->value<uint32_t>(node_id()+"_receive_from_"+sender);

            if(m_decoder->is_complete())
            {
                ++m_counter->value<uint32_t>(node_id()+"_waste_from_"+sender);
                return;
            }

            assert(m_decoder->payload_size() == payload.get_data_size());

            std::copy(payload.data_begin(), payload.data_end(),
                      &m_decode_buffer[0]);

            uint32_t rank = m_decoder->rank();
            m_decoder->decode(&m_decode_buffer[0]);

            if(m_decoder->rank() > rank)
            {
                ++m_counter->value<uint32_t>(node_id()+"_innovative_from_"+sender);
            }
            else
            {
                ++m_counter->value<uint32_t>(node_id()+"_linear_dept_from_"+sender);
            }
        }

    bool is_complete()
        {
            return m_decoder->is_complete();
        }

private:


    // Buffer for packets
    std::vector<uint8_t> m_decode_buffer;

    // The decoder
    typename Decoder::pointer m_decoder;

    // Counter for statistics
    boost::shared_ptr<counter_list> m_counter;

};


//
// Source
//
template<class Encoder>
class basic_source : public source
{
public:

    basic_source(const std::string &id,
                 const typename Encoder::pointer &encoder,
                 const boost::shared_ptr<counter_list> counter)
        : source(id),
          m_encoder(encoder),
          m_counter(counter)
        {
            assert(m_encoder);
            assert(m_counter);

            m_payload.resize(encoder->payload_size());
        }

    void tick()
        {
            ++m_counter->value<uint32_t>("source_sent");

            m_encoder->encode(&m_payload[0]);

            packet p(m_payload);
            p.set_sender(node_id());
            forward_packet(p);
        }

    void forward_packet(packet p)
        {
            for(uint32_t i = 0; i < receiver_count(); ++i)
                forward(i,p);
        }

    void systematic_off()
        {
            m_encoder->systematic_off();
        }

    void systematic_on()
        {
            m_encoder->systematic_on();
        }

    void receive(packet /*p*/)
        {
            assert(0);
        }

private:

    // Payload buffer
    std::vector<uint8_t> m_payload;

    // Pointer to the encoder
    typename Encoder::pointer m_encoder;

    // Counter for statistics
    boost::shared_ptr<counter_list> m_counter;
};



// Relay
template<class Decoder>
class basic_relay : public relay
{
public:

    basic_relay(const std::string &id,
                const typename Decoder::pointer &decoder,
                const boost::shared_ptr<counter_list> &counter)
        : relay(id),
          m_decoder(decoder),
          m_counter(counter),
          m_recode_on(true),
          m_new_packet(false)
        {
            assert(m_decoder);
            assert(m_counter);

            m_recode_buffer.resize(m_decoder->payload_size());
            m_decode_buffer.resize(m_decoder->payload_size());
        }


    virtual void receive(packet payload)
        {
            m_last_packet = payload;
            m_new_packet = true;

            if(m_decoder->is_complete())
            {
                std::string counter_id =
                    "relay_"+node_id()+"_waste_from_"+payload.get_sender();

                ++m_counter->value<uint32_t>(counter_id);
            }
            else
            {
                std::copy(payload.data_begin(), payload.data_end(),
                          &m_decode_buffer[0]);

                uint32_t rank = m_decoder->rank();
                m_decoder->decode(&m_decode_buffer[0]);

                if(rank < m_decoder->rank())
                {
                    std::string counter_id =
                        "relay_"+node_id()+"_innovative_from_"+payload.get_sender();

                    ++m_counter->value<uint32_t>(counter_id);
                }
                else
                {
                    std::string counter_id =
                        "relay_"+node_id()+"_linear_dept_from_"+payload.get_sender();

                    ++m_counter->value<uint32_t>(counter_id);
                }
            }
        }

    void tick()
        {
            if(m_recode_on)
            {
                m_decoder->recode(&m_recode_buffer[0]);
                packet p(m_recode_buffer);
                p.set_sender(node_id());
                forward_packet(p);
            }
            else
            {
                if(m_new_packet)
                {
                    m_last_packet.set_sender(node_id());
                    forward_packet(m_last_packet);
                }
            }
        }

    void forward_packet(packet payload)
        {
            for(uint32_t j = 0; j < receiver_count(); ++j)
            {
                forward(j, payload);
            }
        }


    void set_recode_on()
        {
            m_recode_on = true;
        }

    void set_recode_off()
        {
            m_recode_on = false;
        }

    bool is_recode_on() const
        {
            return m_recode_on;
        }


private:


    /// Buffer for packets
    std::vector<uint8_t> m_recode_buffer;
    std::vector<uint8_t> m_decode_buffer;

    /// Decoder used by the relay to recode
    typename Decoder::pointer m_decoder;

    /// Statistics
    boost::shared_ptr<counter_list> m_counter;

    /// Boolean whether we recode or simply forward packets
    bool m_recode_on;

    /// We store the last packet for forwarding
    bool m_new_packet;
    packet m_last_packet;

};


class simulation_factory
{
public:

    virtual ~simulation_factory(){}
    boost::shared_ptr<channel> build_channel(double error_probability,
                                             const std::string &id = "channel")
        {
            return do_build_channel(id, error_probability);
        }
    boost::shared_ptr<sink> build_sink(const std::string &id = "sink")
        {
            return do_build_sink(id);
        }
    boost::shared_ptr<source> build_source(const std::string &id = "source")
        {
            return do_build_source(id);
        }
    boost::shared_ptr<relay> build_relay(const std::string &id = "relay")
        {
            return do_build_relay(id);
        }
    boost::shared_ptr<counter_list> counter()
        {
            return do_counter();
        }
    boost::shared_ptr<tick_scheduler> scheduler()
        {
            return do_scheduler();
        }

private:

    virtual boost::shared_ptr<channel> do_build_channel(const std::string &id,
                                                        double error_probability) = 0;
    virtual boost::shared_ptr<sink> do_build_sink(const std::string &id) = 0;
    virtual boost::shared_ptr<source> do_build_source(const std::string &id) = 0;
    virtual boost::shared_ptr<relay> do_build_relay(const std::string &id) = 0;
    virtual boost::shared_ptr<counter_list> do_counter() = 0;
    virtual boost::shared_ptr<tick_scheduler> do_scheduler() = 0;


};


// Factory for build elements in the simulation
template<class Encoder, class Decoder>
class basic_simulation_factory : public simulation_factory
{
public:

    basic_simulation_factory(uint32_t symbols,
                             uint32_t symbol_size,
                             boost::random::mt19937 &random_generator)
        : m_decoder_factory(symbols, symbol_size),
          m_encoder_factory(symbols, symbol_size),
          m_random_generator(random_generator)
        {
            m_counter = boost::make_shared<counter_list>();

            uint32_t max_block_size =
                m_encoder_factory.max_symbols() * m_encoder_factory.max_symbol_size();

            m_data.resize(max_block_size);

            kodo::random_uniform<uint8_t> fill_data;
            fill_data.generate(&m_data[0], m_data.size());
        }

    boost::shared_ptr<channel> do_build_channel(const std::string &id,
                                                double error_probability)
        {
            assert(error_probability >= 0.0);
            assert(error_probability <= 1.0);
            boost::shared_ptr<random_bool> channel_conditions =
                build_random_bool(error_probability);

            return boost::make_shared<basic_channel>(id, channel_conditions, m_counter);
        }

    boost::shared_ptr<sink> do_build_sink(const std::string &id)
        {
            typename Decoder::pointer decoder = build_decoder();

            boost::shared_ptr<sink> sink =
                boost::make_shared< basic_sink<Decoder> >(id, decoder, m_counter);

            return sink;
        }

    boost::shared_ptr<relay> do_build_relay(const std::string &id)
        {
            typename Decoder::pointer decoder = build_decoder();

            boost::shared_ptr<relay> relay =
                boost::make_shared< basic_relay<Decoder> >(id, decoder, m_counter);

            return relay;
        }

    boost::shared_ptr<source> do_build_source(const std::string &id)
        {
            typename Encoder::pointer encoder = build_encoder();

            boost::shared_ptr<source> source =
                boost::make_shared< basic_source<Encoder> >(id, encoder, m_counter);

            return source;
        }


    boost::shared_ptr<counter_list> do_counter()
        {
            return m_counter;
        }

    boost::shared_ptr<tick_scheduler> do_scheduler()
        {
            return boost::make_shared<tick_scheduler>();
        }

private:

    typename Decoder::pointer build_decoder()
        {
            // We always just build decoders of the maximum size
            uint32_t max_symbols = m_decoder_factory.max_symbols();
            uint32_t max_symbol_size = m_decoder_factory.max_symbol_size();

            typename Decoder::pointer decoder =
                m_decoder_factory.build(max_symbols, max_symbol_size);

            return decoder;
        }

    typename Encoder::pointer build_encoder()
        {
            // We always just build decoders of the maximum size
            uint32_t max_symbols = m_encoder_factory.max_symbols();
            uint32_t max_symbol_size = m_encoder_factory.max_symbol_size();

            typename Encoder::pointer encoder =
                m_encoder_factory.build(max_symbols, max_symbol_size);

            kodo::set_symbols(kodo::storage(m_data), encoder);

            return encoder;
        }


    boost::shared_ptr<random_bool> build_random_bool(double success_probability)
        {
            return boost::make_shared<random_bool>(boost::ref(m_random_generator),
                                                   success_probability);
        }


private:

    /// Factory for building decoders
    typename Decoder::factory m_decoder_factory;

    /// Factory for building encoders
    typename Encoder::factory m_encoder_factory;

    /// Data for the encoders
    std::vector<uint8_t> m_data;

    /// Statistics
    boost::shared_ptr<counter_list> m_counter;

    /// The random generator
    boost::random::mt19937 &m_random_generator;
};


template<class T>
std::string to_string(T t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

#endif


