#include "Simulation.h"
#include <chrono>


std::shared_ptr<spdlog::logger> simulation_log =
    spdlog::basic_logger_mt("Simulation_log1", "logging/simulation-log.txt");

void runSimulation(ParticleContainer &particleContainer, const double end_time,
                   const double delta_t, bool performance_measurement) {
  outputWriter::VTKWriter writer;
  Model model(particleContainer, "LennJones", delta_t);

  std::chrono::high_resolution_clock::time_point  perf_time_start,perf_time_end;


  double current_time = 0;
  int iteration = 0;

  // calculate inital force:
  model.calculateF();
  model.shiftForces();
  //std::cout << particleContainer.to_string() << '\n';
  simulation_log->debug("Particles that are simulated:\n");
  simulation_log->debug(particleContainer.to_string());


  // for this loop, we assume: current x, current f and current v are known
  if(performance_measurement)
    perf_time_start = std::chrono::high_resolution_clock::now();
  while (current_time < end_time) {
    model.calculateX();

    model.calculateF();

    model.calculateV();

    iteration++;
    
    if (iteration % 10 == 0 && !performance_measurement) {
      writer.initializeOutput(particleContainer.size());
      particleContainer.plotParticles(writer);
      writer.writeFile("out", iteration);
    }

    model.shiftForces();

    /// loading bar
    if (iteration % 50 == 0 && !performance_measurement) {
      size_t barWidth = 50;
      size_t pos = static_cast<size_t>(barWidth * (current_time / end_time));

      std::cout << "[" << std::string(pos, '=') << '>'
                << std::string(barWidth - pos, ' ') << "] "
                << int((current_time / end_time) * 100.0) << "%\r"
                << std::flush;
    }

    current_time += delta_t;
  }
  if(performance_measurement){
    perf_time_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> perf_duration = perf_time_end - perf_time_start;
    std::cout << "The Computation took: " << perf_duration.count() << " seconds" <<std::endl;
  }

  std::cout << "[" << std::string(50, '=') << ">] " << 100 << "%\r"
            << std::flush;
  std::cout << "\noutput written. Terminating...\r" << std::endl;
}

void plotParticles(ParticleContainer &particleContainer, int iteration) {
  std::string out_name("MD_vtk");
  outputWriter::XYZWriter writer;
  writer.plotParticles(particleContainer, out_name, iteration);
}