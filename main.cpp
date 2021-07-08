#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdio>
// Надо было ставить Linux
// Но у меня винда(
// #include <sys/mman.h>

const size_t NUMBER_OF_THREADS = 4;
const size_t LENGTH_LINE = 4096 - 1;

using namespace std;

void process_line(char (*line)) {
    for (size_t i = 0; i < LENGTH_LINE; ++i) {
        if (line[i] <= 'Z' && line[i] >= 'A') {
            line[i] += 32;
        } else if (line[i] >= 'a' && line[i] <= 'z') {
            line[i] -= 32;
        } else if (line[i] >= '0' && line[i] < '9') {
            line[i]++;
        } else if (line[i] == '9') {
            line[i] = '0';
        }
    }
}

void process_thread(vector<char *> lines, size_t offset, size_t number_of_threads) {
    size_t n = lines.size();
    size_t i = offset * (n / number_of_threads);
    size_t end = min(n, i + (n / number_of_threads));
    while (i < end) {
        process_line(lines[i]);
        i++;
    }
}

int main() {
    chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    chrono::steady_clock::time_point data_load;
    try {
        cout << "Load input data" << endl;
        FILE *in_file = fopen("../input.data", "r");

        if (in_file == NULL) {
            cout << "Fail to open file!" << endl;
            return 0;
        }

        vector<char *> lines;
        while (!feof(in_file)) {
            char *buffer = (char *) malloc(LENGTH_LINE);
            fgets(buffer, LENGTH_LINE, in_file);
            lines.push_back(buffer);
        }
        fclose(in_file);

        data_load = std::chrono::steady_clock::now();

        cout << "Start " << NUMBER_OF_THREADS << " threads" << endl;
        vector<thread> thread_pool;
        for (size_t i = 0; i < NUMBER_OF_THREADS; i++) {
            thread_pool.emplace_back(process_thread, ref(lines), i, NUMBER_OF_THREADS);
        }

        cout << "Wait for finish" << endl;
        for (auto &th : thread_pool) {
            th.join();
        }

        cout << "Save output data" << endl;
        FILE *out_file = fopen("../output_cpp.data", "w");
        size_t n = lines.size();
        for (size_t i = 0; i < n; ++i) {
            fputs(lines[i], out_file);
        }
        fclose(out_file);
        cout << "Done" << endl;
    } catch (const exception &e) {
        cout << e.what();
    }

    chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "data load time: " << std::chrono::duration_cast<std::chrono::milliseconds>(data_load - begin).count()
              << "ms" << std::endl;
    std::cout << "total execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
              << "ms" << std::endl;
    return 0;
}
