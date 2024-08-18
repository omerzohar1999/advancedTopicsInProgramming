#include <chrono>
#include <dlfcn.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "AlgorithmRegistrar.h"
#include "MySimulator.h"

std::queue<std::pair<
    std::pair<std::unique_ptr<AbstractAlgorithm>, std::unique_ptr<MySimulator>>,
    std::pair<int, int>>>
    q;
std::mutex q_mutex;
std::vector<std::vector<int>> scores;
std::vector<std::string> algo_path;
std::vector<std::string> house_path;
bool create_output_files = true;

template <typename Func, typename... Args>
std::pair<bool, bool>
run_with_timeout(Func func, std::chrono::milliseconds timeout, Args &&...args) {
  // Run the function asynchronously
  auto future =
      std::async(std::launch::async, func, std::forward<Args>(args)...);

  // Wait for the function to finish or the timeout to occur
  if (future.wait_for(timeout) == std::future_status::ready) {
    return {true, future.get()}; // Function completed before timeout
  } else {
    return {false, false}; // Timeout occurred before function completed
  }
}

void thread_job_func() {
  while (!q.empty()) {
    // first is whether function completed before timeout, second is whether
    // there was an error
    std::pair<std::pair<std::unique_ptr<AbstractAlgorithm>,
                        std::unique_ptr<MySimulator>>,
              std::pair<int, int>>
        pair_sim;

    {
      std::lock_guard<std::mutex> lock(q_mutex);
      if (q.empty()) {
        break;
      }
      pair_sim = std::move(q.front());
      q.pop();
    }

    int max_steps = pair_sim.first.second->getMaxSteps();

    // run simulator.run with timeout
    auto result = run_with_timeout(&MySimulator::run,
                                   std::chrono::milliseconds(max_steps),
                                   pair_sim.first.second.get());

    bool error = result.second;
    bool completed = result.first;

    if (error) {
      std::cout << "Error in house " << pair_sim.second.second << " with algo "
                << pair_sim.second.first << std::endl;
    } else {
      scores[pair_sim.second.first][pair_sim.second.second] =
          pair_sim.first.second->calculate_score(completed);

      if (create_output_files) {
        // get house name: house file name without the extension
        std::string house_name = house_path[pair_sim.second.second].substr(
            0, house_path[pair_sim.second.second].find(".house"));
        // get algo name: algo file name without the extension .so and without
        // the lib prefix
        std::string algo_name = algo_path[pair_sim.second.first].substr(
            3, algo_path[pair_sim.second.first].find(".so") - 3);
        // output file name is housename-algoname.txt
        std::string output_file = house_name + "-" + algo_name + ".txt";
        pair_sim.first.second->createOutput(output_file, completed);
      }
    }
  }
}

int main(int argc, char *argv[]) {
  int num_threads = 10;

  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg.find("-algo_path=") == 0) {
      algo_path.push_back(arg.substr(11));
    } else if (arg.find("-house_path=") == 0) {
      house_path.push_back(arg.substr(12));
    } else if (arg.find("-num_threads=") == 0) {
      num_threads = std::stoi(arg.substr(13));
    } else if (arg.compare("-summary_only") == 0) {
      create_output_files = false;
    }
  }

  if (algo_path.empty()) {
    // get all .so files in the working directory
    std::filesystem::path dir_path = std::filesystem::current_path();
    for (const auto &entry : std::filesystem::directory_iterator(dir_path)) {
      if (entry.path().extension() == ".so") {

        std::string algo_name = entry.path().string().substr(
            3, entry.path().string().find(".so") - 3);

        void *library_handle = dlopen(entry.path().c_str(), RTLD_LAZY);

        if (!library_handle) {

          std::ofstream error_file(algo_name + ".error");

          if (error_file.is_open()) {
            error_file << dlerror() << std::endl;
            error_file.close();
          }

          else {
            std::cout << "Failed to open " << algo_name << ".error"
                      << std::endl;
          }
        }

        else
          algo_path.push_back(algo_name);
      }
    }
  } else {
    // open the .so file given as argument
    void *library_handle = dlopen(algo_path[0].c_str(), RTLD_LAZY);

    if (!library_handle) {
      std::string algo_name =
          algo_path[0].substr(3, algo_path[0].find(".so") - 3);
      std::ofstream error_file(algo_name + ".error");

      if (error_file.is_open()) {
        error_file << dlerror() << std::endl;
        error_file.close();
      }

      else {
        std::cout << "Failed to open " << algo_name << ".error" << std::endl;
      }
      return 1;
    }
  }
  if (house_path.empty()) {
    // get all .house files in the directory
    std::filesystem::path dir_path = std::filesystem::current_path();
    for (const auto &entry : std::filesystem::directory_iterator(dir_path)) {
      if (entry.path().extension() == ".house") {
        std::string house_name = entry.path().string().substr(
            0, entry.path().string().find(".house"));
        // try creating a simulator with the house file
        std::unique_ptr<MySimulator> sim(new MySimulator());
        if (sim->readHouseFile(entry.path().string())) {
          // create housename.error file
          std::ofstream error_file(house_name + ".error");
          if (error_file.is_open()) {
            error_file << "Error reading house file" << std::endl;
            error_file.close();
          } else {
            std::cout << "Failed to open " << house_name << ".error"
                      << std::endl;
          }
        } else
          house_path.push_back(house_name);
      }
    }
  } else {
    // try creating a simulator with the house file
    std::unique_ptr<MySimulator> sim(new MySimulator());
    if (sim->readHouseFile(house_path[0])) {
      // create housename.error file
      std::string house_name =
          house_path[0].substr(0, house_path[0].find(".house"));
      std::ofstream error_file(house_name + ".error");
      if (error_file.is_open()) {
        error_file << "Error reading house file" << std::endl;
        error_file.close();
      } else {
        std::cout << "Failed to open " << house_name << ".error" << std::endl;
      }
      return 1;
    }
  }

  scores.resize(algo_path.size());
  for (size_t i = 0; i < scores.size(); i++) {
    scores[i].resize(house_path.size());
  }

  AlgorithmRegistrar registrar = AlgorithmRegistrar::getAlgorithmRegistrar();

  size_t i = 0;
  for (auto &algo : registrar) {
    for (size_t j = 0; j < house_path.size(); j++) {
      std::unique_ptr<MySimulator> sim;
      std::unique_ptr<AbstractAlgorithm> concrete_algo(algo.create());
      sim->readHouseFile(house_path[j] + ".house");
      sim->setAlgorithm(*concrete_algo);
      std::pair<int, int> pair_indices = {i, j};
      std::pair<std::unique_ptr<AbstractAlgorithm>,
                std::unique_ptr<MySimulator>>
          pair_sim = {std::move(concrete_algo), std::move(sim)};

      q.push({std::move(pair_sim), pair_indices});
    }
    i++;
  }

  std::vector<std::thread> threads;
  threads.resize(num_threads);
  for (int i = 0; i < threads.size(); ++i) {
    threads[i] = std::thread(thread_job_func);
  }

  for (auto &thread : threads) {
    thread.join();
  }

  std::ofstream summary_file("summary.csv");
  if (summary_file.is_open()) {
    for (size_t i = 0; i < scores.size(); i++) {
      for (size_t j = 0; j < scores[i].size(); j++) {
        summary_file << scores[i][j] << ",";
      }
      summary_file << std::endl;
    }
    summary_file.close();
  } else {
    std::cout << "Failed to open summary.csv" << std::endl;
  }
}
