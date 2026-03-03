//
// Created by racso on 21/01/2026.
//

#pragma once
#include <sdsl/int_vector.hpp>
#include "utils.h"

#ifndef ENCODERS_H
#define ENCODERS_H

#endif //ENCODERS_H


class DummyDecompressedLevel {

public:

    template<typename string_type>
    void extend_rule(uint64_t rule_num, string_type &r_string, uint64_t &l) {
        uint64_t rule_start = rule_pos[rule_num];
        uint64_t rule_length = rule_pos[rule_num + 1] - rule_pos[rule_num];
        for (uint64_t i = 0; i < rule_length; ++i) {
            r_string[l++] = rule[rule_start + i];
        }
    }

    sdsl::int_vector<> rule;
    sdsl::int_vector<> rule_pos;
};

class Dummy {
    uint32_t string_size{};
    uint32_t alphabet_size{};
    std::vector<int> lcps;
    std::vector<std::string> rhs;
    std::string level_reduced_string;


public:

    sdsl::int_vector<> tail;

    Dummy() = default;
    Dummy(Dummy &rhs) = default;
    Dummy(Dummy &&rhs) = default;

    template<typename sais_index_type, typename char_type>
    Dummy(sais_index_type* RA,
        sais_index_type* SA,
        const char_type* T,
        sais_index_type n,
        sais_index_type k,
        sais_index_type m,
        sais_index_type names,
        int cs,
        bool is_last_level = false,
        Dummy* prev_level = nullptr
        ) {

        sais_index_type i, j, p, plen;
        sais_index_type head_section_size;
        sais_index_type c0, c1;

        level_reduced_string.resize(m);
        for (i = 0; i < m; ++i) {
            level_reduced_string[i] = RA[i];
        }

        head_section_size = *std::min_element(RA, RA + m);

        std::vector<sdsl::int_vector<>> rules(names + 1);

        std::cout << n << std::endl;
        int lms_pos = m - 1;
        i = n - 2; j = n - 1; c0 = chr(n - 1);
        while (head_section_size <= i) {
            while (head_section_size <= i && (c1 = chr(i)) >= c0) {
                c0 = c1;
                --i;
            }
            while (head_section_size <= i && (c1 = chr(i)) <= c0) {
                c0 = c1;
                --i;
            }
            plen = j - i - static_cast<int>(j + 1 < n);
            j = i + 1;
            sdsl::int_vector<> current_rule(plen);
            for (p = 0; p < plen; ++p) {
                current_rule[p] = chr(i + 1 + p);
            }
            rules[RA[lms_pos--]] = std::move(current_rule);
        }

        sdsl::int_vector<> head_section(head_section_size);
        for (i = 0; i < head_section_size; ++i)
            head_section[i] = chr(i);

        rules[names] = std::move(head_section);

        rhs.resize(names + 1);
        std::cout << "Adding this level :v" << std::endl;
        std::cout << names << std::endl;
        for (i = 0; i < names; ++i) {
            if (check_printable(rules[i])) {
                std::cout << i << " " << std::string(rules[i].begin(), rules[i].end()) << std::endl;
            }
            else {
                std::cout << i << " ";
                for (auto x : rules[i]) {
                    std::cout << static_cast<int>(x) << " ";
                }
                std::cout << std::endl;
            }

            rhs[i].resize(rules[i].size());
            for (j = 0; j < rhs[i].size(); ++j) {
                rhs[i][j] = rules[i][j];
            }
        }
    }

    uint64_t size_in_bytes() const {
        uint64_t total_bytes = 0;
        total_bytes += level_reduced_string.size();
        for (auto &rule : rhs) {
            total_bytes += rule.size();
        }
        return total_bytes;
    }

    void serialize(std::ostream& out) const {
        out << level_reduced_string.size() << '\n';
        for(int i = 0; i < level_reduced_string.size(); i++) {
            out << static_cast<int>(level_reduced_string[i]) << " \n"[i + 1 == level_reduced_string.size()];
        }
        out << rhs.size() << '\n';
        for (auto &rule : rhs) {
            out << rule.size() << " ";
            for (auto x : rule) {
                out << static_cast<int>(x) << " ";
            }
            out << '\n';
        }
    };

    void load(std::istream& in) {
        int len;
        in >> len;
        level_reduced_string.resize(len);
        for(int i = 0; i < len; i++) {
            in >> level_reduced_string[i];
        }
        int rules;
        in >> rules;
        rhs.resize(rules);
        for (int i = 0; i < rules; i++) {
            int rule_len;
            in >> rule_len;
            rhs[i].resize(rule_len);
            for (int j = 0; j < rule_len; j++) {
                int x;
                in >> x;
                rhs[i][j] = static_cast<char>(x);
            }
        }
    }

    uint32_t get_string_size() const {
        return this -> string_size;
    }

    uint32_t get_alphabet_size() const {
        return this -> alphabet_size;
    }

    DummyDecompressedLevel decompress() {


        return DummyDecompressedLevel();
    }

};