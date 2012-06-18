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


    void print(std::ostream &o)
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
                        o << it_data->first << " "
                          << it_data->second << std::endl;
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

            bool first = true;

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


class receiver
{
public:
    virtual ~receiver(){}
    virtual void receive(const uint8_t *payload) = 0;
};

class output_helper
{
public:

    // Adds a receiver to the output
    virtual void add_output(const boost::shared_ptr<receiver> &output)
        {
            assert(output);
            m_output.push_back(output);
        }

    // Condition for the forwarding
    virtual bool should_forward()
        {
            return true;
        }

    // Forwards a payload to the output
    virtual void forward(const uint8_t *payload)
        {
            for(uint32_t i = 0; i < m_output.size(); ++i)
            {
                if(should_forward())
                {
                    m_output[i]->receive(payload);
                }
            }
        }

private:

    // Stores the output receivers
    std::vector< boost::shared_ptr<receiver> > m_output;
};


// Convenience class
class channel : public receiver, public output_helper
{
};

// Convenience class
class relay : public receiver, public output_helper
{
};

struct sink : public receiver
{
    virtual bool is_complete() = 0;
};

struct source : public output_helper
{
    virtual void send() = 0;

    virtual void systematic_off() = 0;

    virtual void systematic_on() = 0;

};



// Channel
class basic_channel : public channel
{

public:

    // Create a simple coin-flip channel
    basic_channel(const boost::shared_ptr<random_bool> &channel_condition,
                  const boost::shared_ptr<counter_list> &counter)
        : m_channel_condition(channel_condition),
          m_counter(counter)
        {}

    // Receives a payload
    virtual void receive(const uint8_t *payload)
        {
            forward(payload);
        }

    // Override the should forward function to
    virtual bool should_forward()
        {
            // If true we drop
            if(m_channel_condition->generate())
            {
                ++m_counter->value<uint32_t>("channel_dropped");
                return false;
            }
            else
            {
                ++m_counter->value<uint32_t>("channel_sent");
                return true;
            }
        }

private:

    // The channel
    boost::shared_ptr<random_bool> m_channel_condition;

    // Nodes connected via this channel
    std::vector<boost::shared_ptr<receiver> > m_nodes;

    // Statistics
    boost::shared_ptr<counter_list> m_counter;
};



// Sink
template<class Decoder>
class basic_sink : public sink
{
public:

    basic_sink(const typename Decoder::pointer &decoder,
               const boost::shared_ptr<counter_list> &counter)
        : m_decoder(decoder),
          m_counter(counter)
        {
            assert(m_decoder);
            assert(m_counter);

            m_decode_buffer.resize(m_decoder->payload_size());
        }

    void receive(const uint8_t *payload)
        {
            assert(payload != 0);

            ++m_counter->value<uint32_t>("sink_receive");

            if(m_decoder->is_complete())
            {
                ++m_counter->value<uint32_t>("sink_waste");
                return;
            }

            std::copy(payload, payload + m_decoder->payload_size(),
                      &m_decode_buffer[0]);

            uint32_t rank = m_decoder->rank();
            m_decoder->decode(&m_decode_buffer[0]);

            if(m_decoder->rank() > rank)
            {
                ++m_counter->value<uint32_t>("sink_innovative");
            }
            else
            {
                ++m_counter->value<uint32_t>("sink_linear_dependent");
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

    basic_source(const typename Encoder::pointer &encoder,
                 const boost::shared_ptr<counter_list> counter)
        : m_encoder(encoder),
          m_counter(counter)
        {
            assert(m_encoder);
            assert(m_counter);

            m_payload.resize(encoder->payload_size());
        }

    void send()
        {
            ++m_counter->value<uint32_t>("source_sent");

            m_encoder->encode(&m_payload[0]);
            forward(&m_payload[0]);
        }

    void systematic_off()
        {
            m_encoder->systematic_off();
        }

    void systematic_on()
        {
            m_encoder->systematic_on();
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

    basic_relay(const typename Decoder::pointer &decoder,
                const boost::shared_ptr<counter_list> &counter)
        : m_decoder(decoder),
          m_counter(counter)
        {
            assert(m_decoder);
            assert(m_counter);

            m_recode_buffer.resize(m_decoder->payload_size());
            m_decode_buffer.resize(m_decoder->payload_size());
        }

    virtual void receive(const uint8_t *payload)
        {
            if(m_decoder->is_complete())
            {
                ++m_counter->value<uint32_t>("relay_waste");
            }
            else
            {
                std::copy(payload, payload + m_decoder->payload_size(),
                          &m_decode_buffer[0]);

                uint32_t rank = m_decoder->rank();
                m_decoder->decode(&m_decode_buffer[0]);

                if(rank < m_decoder->rank())
                {
                    ++m_counter->value<uint32_t>("relay_innovative");
                }
                else
                {
                    ++m_counter->value<uint32_t>("relay_linear_dependent");
                }
            }

            m_decoder->recode(&m_recode_buffer[0]);

            forward(&m_recode_buffer[0]);

        }

    virtual bool should_forward()
        {
            ++m_counter->value<uint32_t>("relay_forward");
            return true;
        }

private:

    // Buffer for packets
    std::vector<uint8_t> m_recode_buffer;
    std::vector<uint8_t> m_decode_buffer;

    // Decoder used by the relay to recode
    typename Decoder::pointer m_decoder;

    // Statistics
    boost::shared_ptr<counter_list> m_counter;

};


class simulation_factory
{
public:

    virtual ~simulation_factory(){}
    virtual boost::shared_ptr<channel> build_channel(double error_probability) = 0;
    virtual boost::shared_ptr<sink> build_sink() = 0;
    virtual boost::shared_ptr<source> build_source() = 0;
    virtual boost::shared_ptr<relay> build_relay() = 0;
    virtual boost::shared_ptr<counter_list> counter() = 0;
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

    boost::shared_ptr<channel> build_channel(double error_probability)
        {
            assert(error_probability >= 0.0);
            assert(error_probability <= 1.0);
            boost::shared_ptr<random_bool> channel_conditions =
                build_random_bool(error_probability);

            return boost::make_shared<basic_channel>(channel_conditions, m_counter);
        }

    boost::shared_ptr<sink> build_sink()
        {
            typename Decoder::pointer decoder = build_decoder();
            return boost::make_shared< basic_sink<Decoder> >(decoder, m_counter);
        }

    boost::shared_ptr<relay> build_relay()
        {
            typename Decoder::pointer decoder = build_decoder();
            return boost::make_shared< basic_relay<Decoder> >(decoder, m_counter);
        }

    boost::shared_ptr<source> build_source()
        {
            typename Encoder::pointer encoder = build_encoder();
            return boost::make_shared< basic_source<Encoder> >(encoder, m_counter);
        }


    boost::shared_ptr<counter_list> counter()
        {
            return m_counter;
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

    typename Decoder::factory m_decoder_factory;
    typename Encoder::factory m_encoder_factory;

    // Data for the encoders
    std::vector<uint8_t> m_data;

    // Statistics
    boost::shared_ptr<counter_list> m_counter;

    // The random generator
    boost::random::mt19937 &m_random_generator;
};





#endif

