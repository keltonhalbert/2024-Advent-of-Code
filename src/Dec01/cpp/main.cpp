#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <algorithm>
#include <unordered_map>
#include <cstdint>

constexpr int N_CHARS_PER_LINE = 13; 
constexpr int N_FILE_LINES = 1000;
constexpr int IDX_START_NUM1 = 0;
constexpr int IDX_START_NUM2 = 8;
constexpr int LEN_NUM1 = 5;
constexpr int LEN_NUM2 = 5;

using int_t = std::int32_t; 
using arr_t = std::array<int_t, N_FILE_LINES>;
using map_t = std::unordered_map<int_t, int_t>;

// Parse the inout file into two arrays. 
static int parse_input(arr_t& arr1, arr_t& arr2, map_t lookup_table) {
    std::ifstream input_file("./input.txt", std::ios::in);
    std::string line;

    if (input_file.is_open()) {
        line.reserve(N_CHARS_PER_LINE);
        std::size_t idx = 0;
        while ( (std::getline(input_file, line)) && (idx < N_FILE_LINES) ) {
            int_t num1 = std::stoi(line.substr(IDX_START_NUM1, LEN_NUM2));
            int_t num2 = std::stoi(line.substr(IDX_START_NUM2, LEN_NUM1));
            arr1[idx] = num1;
            arr2[idx] = num2;
            lookup_table[num1] = 0;
            ++idx;
        }
        input_file.close();
        return 1;
    }
    else {
        printf("Unable to open file.\n");
        input_file.close();
        return 0;
    }
}

static int_t calc_distances(const arr_t& arr1, const arr_t& arr2, arr_t& distances) noexcept {
    int_t sum = 0;
    for (std::size_t idx = 0; idx < arr1.size(); ++idx) {
        const int_t dist = std::abs(arr1[idx] - arr2[idx]);
        distances[idx] = dist;
        sum += dist;
    }
    return sum;
}

static int_t calc_similarity(const arr_t& arr1, const arr_t& arr2, map_t& lookup_table) {
    for (std::size_t idx = 0; idx < arr2.size(); ++idx) {
        const int_t key2 = arr2[idx];
        lookup_table[key2] += 1;
    } 

    int_t similarity = 0;
    for (std::size_t idx = 0; idx < arr1.size(); ++idx) {
        similarity += arr1[idx] * lookup_table[arr1[idx]];
    }
    return similarity;
}

int main(int argc, const char** argv) {

    arr_t arr1;
    arr_t arr2;
    arr_t distances;
    map_t lookup_table;
    lookup_table.reserve(N_FILE_LINES);

    if (parse_input(arr1, arr2, lookup_table)) {
        std::sort(arr1.begin(), arr1.end());
        std::sort(arr2.begin(), arr2.end());

        // Part 1
        const int_t dists_sum = calc_distances(arr1, arr2, distances);
        printf("Sum: %d\n", dists_sum);

        // Part 2 
        const int_t similarity = calc_similarity(arr1, arr2, lookup_table);
        printf("Similarity: %d\n", similarity);
        return 0;
    };
}
