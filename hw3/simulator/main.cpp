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

#include "common/AlgorithmRegistrar.h"
#include "simulator/MySimulator.h"

std::vector<std::pair<std::string, std::unique_ptr<MySimulator>>> simulators;
std::vector<std::string> algorithm_names;
std::queue<
    std::pair<std::pair<int, int>, std::pair<std::unique_ptr<AbstractAlgorithm>,
                                             std::unique_ptr<MySimulator>>>>
    q;
std::mutex q_mutex;
std::vector<std::vector<int>> scores;
std::vector<void *> algorithm_handles;

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
    std::pair<std::pair<int, int>, std::pair<std::unique_ptr<AbstractAlgorithm>,
                                             std::unique_ptr<MySimulator>>>
        pair_sim;

    {
      std::lock_guard<std::mutex> lock(q_mutex);
      if (q.empty()) {
        break;
      }
      pair_sim = std::move(q.front());
      q.pop();
    }

    std::pair<int, int> pair_indices = pair_sim.first; // first is algo index,
                                                       // second is house index

    std::string house_name = simulators[pair_indices.second].first;
    std::string algo_name = algorithm_names[pair_indices.first];

    std::unique_ptr<MySimulator> sim = std::move(pair_sim.second.second);
    std::unique_ptr<AbstractAlgorithm> algo = std::move(pair_sim.second.first);

    sim->setAlgorithm(*algo);

    int max_steps = sim->getMaxSteps();

    // run simulator.run with timeout
    auto result = run_with_timeout(
        &MySimulator::run, std::chrono::milliseconds(max_steps), sim.get());

    bool error = result.second;
    bool completed = result.first;

    if (error) {
      std::cout << "Error in house " << house_name << " with algo " << algo_name
                << std::endl;
    } else {
      scores[pair_indices.first][pair_indices.second] =
          sim->calculate_score(completed);

      if (create_output_files) {
        // output file name is housename-algoname.txt
        std::string output_file = house_name + "-" + algo_name + ".txt";
        sim->createOutput(output_file, completed);
      }
    }
  }
}

int main(int argc, char *argv[]) {
  int num_threads = 10;

  std::filesystem::path house_path = std::filesystem::current_path();
  std::filesystem::path algo_path = std::filesystem::current_path();
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg.find("-algo_path=") == 0) {
      algo_path = std::filesystem::path(arg.substr(11));
    } else if (arg.find("-house_path=") == 0) {
      house_path = std::filesystem::path(arg.substr(12));
    } else if (arg.find("-num_threads=") == 0) {
      num_threads = std::stoi(arg.substr(13));
    } else if (arg.compare("-summary_only") == 0) {
      create_output_files = false;
    }
  }

  // get all .so files in the working directory
  for (const auto &entry : std::filesystem::directory_iterator(algo_path)) {
    if (entry.path().extension() == ".so") {

      // find name of algorithm: it's an .so file with maybe lib prefix,
      // remove both
      std::string algo_name = entry.path().filename().string();
      if (algo_name.find("lib") == 0) {
        algo_name = algo_name.substr(3);
      }

      void *library_handle = dlopen(entry.path().c_str(), RTLD_LAZY);

      if (!library_handle) {

        std::ofstream error_file(algo_name + ".error");

        if (error_file.is_open()) {
          error_file << dlerror() << std::endl;
          error_file.close();
        }

        else {
          std::cout << "Failed to open " << algo_name << ".error" << std::endl;
        }
      }

      else
        algorithm_handles.push_back(library_handle);
    }
  }

  // get all .house files in the directory
  for (const auto &entry : std::filesystem::directory_iterator(house_path)) {
    if (entry.path().extension() == ".house") {
      // get house name: house file name without the extension or the path
      std::string house_name = entry.path().filename().string();

      // try creating a simulator with the house file
      std::unique_ptr<MySimulator> sim = std::make_unique<MySimulator>();
      if (sim->readHouseFile(entry.path().string())) {
        std::cout << "Error reading house file" << std::endl;
        // create housename.error file
        std::ofstream error_file(house_name + ".error");
        if (error_file.is_open()) {
          error_file << "Error reading house file" << std::endl;
          error_file.close();
        } else {
          std::cout << "Failed to open " << house_name << ".error" << std::endl;
        }
      } else {
        simulators.emplace_back(house_name.substr(0, house_name.find(".house")),
                                std::move(sim));
      }
    }
  }

  scores.resize(algorithm_handles.size());
  for (size_t i = 0; i < scores.size(); i++) {
    scores[i].resize(simulators.size());
  }

  AlgorithmRegistrar registrar = AlgorithmRegistrar::getAlgorithmRegistrar();

  size_t i = 0;
  for (auto &algo : registrar) {
    algorithm_names.emplace_back(algo.name());
    for (size_t j = 0; j < simulators.size(); j++) {

      std::unique_ptr<MySimulator> sim =
          std::make_unique<MySimulator>(*simulators[j].second);
      std::unique_ptr<AbstractAlgorithm> concrete_algo = algo.create();

      std::pair<int, int> pair_indices = {i, j};
      std::pair<std::unique_ptr<AbstractAlgorithm>,
                std::unique_ptr<MySimulator>>
          pair_sim = {std::move(concrete_algo), std::move(sim)};

      q.push({pair_indices, std::move(pair_sim)});
    }
    i++;
  }

  std::vector<std::thread> threads;
  threads.resize(num_threads);
  for (int i = 0; i < (int)threads.size(); ++i) {
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

  simulators.clear();
  registrar.clear();
  algorithm_handles.clear();

  for (auto handle : algorithm_handles) {
    dlclose(handle);
  }
}
