// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <ctime>
#include "simulation.h"
#include "single_relay.h"
#include "single_no_relay.h"
#include "relay_line.h"

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
