#include <iostream>
#include <fstream>
#include <chrono>
#include <variant>

#include "simple8b.h"
#include "headers/eliasfano.h"
#include "headers/gcis.h"
#include "headers/utils.h"

using timer = std::chrono::high_resolution_clock;

using EncoderVariant = std::variant<S8B<32>, EliasFano<32>>;

using sais_index_type = int;

EncoderVariant make_encoder(const std::string& type) {
    if (type == "-s8b") return S8B<32>{};
    return EliasFano<32>{};
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        print_usage_message();
        return -1;
    }
    std::string mode(argv[1]);
    std::string encoder_type(argv[4]);

    EncoderVariant encoder;

    if (encoder_type == "-s8b")
        encoder.emplace<S8B<32>>();
    else if (encoder_type == "-ef")
        encoder.emplace<EliasFano<32>>();
    else {
        std::cout << "Invalid encoder type" << std::endl;
        assert(false);
    }

    if (mode == "-d") {
        std::cerr << "Decompression." << std::endl;

        std::ifstream input(argv[2], std::ios::binary);
        std::ofstream output(argv[3], std::ios::binary);
        EncoderVariant encoder_variant = make_encoder(encoder_type);

        std::visit([&]<typename Encoder>(Encoder& enc) {

            GrammarInterface<sais_index_type, Encoder> gi;

            gi.load(input);

            auto start = timer::now();

            gi.decode(output);

            auto stop = timer::now();

            std::cout << "Time: "
                  << static_cast<double>(
                         std::chrono::duration_cast<std::chrono::seconds>(stop - start).count()
                     )
                  << " seconds\n";
        }, encoder_variant);

        input.close();
        output.close();
    }
    else if (mode == "-c") {
        std::cerr << "Compression." << std::endl;
        int n;
        char *str;
        load_string_from_file(str, argv[2], n);
        std::ofstream output(argv[3], std::ios::binary);
        auto SA = new int[n];

        EncoderVariant encoder_variant = make_encoder(encoder_type);

        std::visit([&]<typename Encoder>(Encoder& enc) {

            GrammarInterface<sais_index_type, Encoder> gi;

            auto start = timer::now();

            if (gcis(gi, str, SA, n) == 0) {
                std::cout << "Compression done successfully\n";
            } else {
                throw std::runtime_error("Compression failed");
                return;
            }

            auto stop = timer::now();

            std::cout << "Input:\t" << n << " bytes\n";
            std::cout << "Output:\t" << gi.size_in_bytes() << " bytes\n";
            std::cout << "Time: "
                      << static_cast<double>(
                             std::chrono::duration_cast<std::chrono::seconds>(stop - start).count()
                         )
                      << " seconds\n";
            gi.serialize(output);
        }, encoder_variant);
        delete[] SA;
        delete[] str;
    }
    else if (mode == "-e") {
        std::cerr << "Extract substring. (WIP)" << std::endl;

        /*std::ifstream input(argv[2], std::ios::binary);
        std::ifstream queries_file(argv[3], std::ios::binary);
        EncoderVariant encoder_variant = make_encoder(encoder_type);

        std::visit([&]<typename Encoder>(Encoder& enc) {

            GrammarInterface<sais_index_type, Encoder> gi;

            gi.load(input);

            std::vector<std::pair<sais_index_type, sais_index_type>> queries;

            read_queries(queries_file, queries);

            auto start = timer::now();

            gi.extract_batch(queries);

            auto stop = timer::now();

            std::cout << "Time: "
                  << static_cast<double>(
                         std::chrono::duration_cast<std::chrono::seconds>(stop - start).count()
                     )
                  << " seconds\n";
        }, encoder_variant);

        input.close();
        output.close();*/
    }
    else if (mode == "-s") {
        std::cerr << "Suffix array construction. (WIP)" << std::endl;
    }
    else if (mode == "-l") {
        std::cerr << "Suffix array and LCP array construction. (WIP)" << std::endl;
    }
    else {
        std::cerr << "Incorrect algorithm mode" << std::endl;
    }
    return 0;
}