/*
 * gcis-lite
 * Copyright (C) 2026 Racso Galvan
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 */

#pragma once

#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <stdexcept>

#include <sdsl/bit_vectors.hpp>
#include "utils.h"

template <typename sais_index_type, typename encoder_type>
class GrammarInterface {

    std::vector<encoder_type> g;
    sdsl::int_vector<> final_reduced_string;

public:

    virtual ~GrammarInterface() = default;

    GrammarInterface()
        : g() {}

    GrammarInterface(std::vector<encoder_type> grammar)
        : g(std::move(grammar)) {}

    GrammarInterface(const GrammarInterface&) = default;
    GrammarInterface(GrammarInterface&&) = default;
    GrammarInterface& operator=(const GrammarInterface&) = default;
    GrammarInterface& operator=(GrammarInterface&&) = default;

    [[nodiscard]] uint64_t size_in_bytes() const {
        uint64_t total_bytes = 0;

        for (const auto& level : g)
            total_bytes += level.size_in_bytes();

        total_bytes += sdsl::size_in_bytes(final_reduced_string);
        return total_bytes;
    }

    template<typename char_type>
    void add_rule_level(
        sais_index_type* RA,
        sais_index_type* SA,
        const char_type* T,
        sais_index_type n,
        sais_index_type k,
        sais_index_type m,
        int cs,
        bool is_last_level
    ) {

        if (m == 0) return;

        sais_index_type names = *std::max_element(RA, RA + m) + 1;

        if (g.empty()) {
            //std::cout << "Empty case" << std::endl;
            g.emplace_back(RA, SA, T, n, k, m, names, cs, is_last_level);
        }
        else {
            //std::cout << "Non-empty case" << std::endl;
            g.emplace_back(RA, SA, T, n, k, m, names, cs, is_last_level, &g.back());
        }
    }

    template<typename char_type>
    void set_reduced_string(const char_type* RA, sais_index_type m) {
        final_reduced_string.resize(m);

        for (sais_index_type i = 0; i < m; ++i)
            final_reduced_string[i] = RA[i];

        sdsl::util::bit_compress(final_reduced_string);
    }

    void serialize(std::ostream& o) const {

        if (!o)
            throw std::runtime_error("Error writing grammar size");

        final_reduced_string.serialize(o);
        uint64_t size = g.size();
        o.write(reinterpret_cast<char*>(&size), sizeof(size));

        for (const auto& level : g)
            level.serialize(o);
    }

    virtual void load(std::istream& i) {

        uint64_t size;

        final_reduced_string.load(i);

        i.read(reinterpret_cast<char*>(&size), sizeof(size));

        if (!i)
            throw std::runtime_error("Error reading grammar size");

        g.resize(size);

        for (uint64_t j = 0; j < size; ++j)
            g[j].load(i);
    }

    void decode(std::ostream& out) {
        sdsl::int_vector<> r_string(final_reduced_string.size());
        for (sais_index_type i = 0; i < final_reduced_string.size(); ++i) {
            r_string[i] = final_reduced_string[i];
        }
        /*for (auto x : r_string) {
            std::cout << static_cast<int>(x) << " ";
        }
        std::cout << std::endl;*/
        if (not g.empty()) {
            for (int64_t i = g.size() - 1; i >= 0; --i) {
                std::cout << "Decompressing level " << i << std::endl;
                auto decompressed_encoder = std::move(g[i].decompress());
                std::cout << "Decompressed level " << i << std::endl;
                sdsl::int_vector<> new_r_string(g[i].get_string_size(), 0, sdsl::bits::hi(g[i].get_alphabet_size()) + 1);
                uint64_t new_r_string_ptr = 0;
                for (auto c : g[i].tail) {
                    new_r_string[new_r_string_ptr++] = c;
                }
                for (auto c : r_string) {
                    decompressed_encoder.extend_rule(c, new_r_string, new_r_string_ptr);
                }
                r_string = std::move(new_r_string);
                //g[i].compute_new_level(r_string);
                std::cout << "Done with level " << i << std::endl;
                /*std::cout << "New decompressed string: " << std::endl;
                for (auto x : r_string) {
                    std::cout << static_cast<int>(x) << " ";
                }
                std::cout << std::endl;*/
            }
        }
        if (check_printable(r_string)) {
            char *str = new char[r_string.size()];
            for (uint64_t i = 0; i < r_string.size(); i++) {
                str[i] = static_cast<char>(r_string[i]);
            }
            out.write(str, static_cast<size_t>(r_string.size()));
        }
        else {
            std::copy(r_string.begin(),
          r_string.end(),
          std::ostream_iterator<uint64_t>(out, " "));
        }
    }

};
