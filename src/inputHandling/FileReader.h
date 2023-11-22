/*
 * FileReader.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once

#include <list>
#include <sstream>
#include <string>

#include "particleModel/Particle.h"
#include "particleModel/ParticleContainer.h"

class FileReader {
 public:
  FileReader();
  virtual ~FileReader();

  struct SphereData {
    std::array<double, 3> CenterPosition;
    std::array<double, 3> Velocity;
    double mass;
    double radius;
    double meshWidth;
    double sigma;
    double epsilon;

    std::string to_string() const{
     auto sphereData = (*this);
    std::ostringstream oss;

    oss << "CenterPosition: [" << sphereData.CenterPosition[0] << ", "
        << sphereData.CenterPosition[1] << ", " << sphereData.CenterPosition[2] << "]" << std::endl;
    oss << "Velocity: [" << sphereData.Velocity[0] << ", "
        << sphereData.Velocity[1] << ", " << sphereData.Velocity[2] << "]" << std::endl;
    oss << "Mass: " << sphereData.mass << std::endl;
    oss << "Radius: " << sphereData.radius << std::endl;
    oss << "MeshWidth: " << sphereData.meshWidth << std::endl;
    oss << "Sigma: " << sphereData.sigma << std::endl;
    oss << "Epsilon: " << sphereData.epsilon << std::endl;

    return oss.str();
    }
  };

  struct CuboidData {
    /// initial velocity and position vectors
    std::array<double, 3> x, v;

    /// N1: amount of particles along dimension 1
    /// N2: amount of particles along dimension 2
    /// N3: amount of particles along dimension 3
    uint64_t N1, N2, N3;

    /// Mass m of the particles in the cuboid
    /// Mesh width h
    double m, h;

    /// sigma and epsilon parameters for the force calculation
    /// between particles of this cuboid
    double sigma, epsilon;

    /// Average velocity (always 0.1 in our case)
    double avg_v = 0.1;

    /**
     * @brief Convert CuboidData to a string
     *
     * @return String representation of CuboidData
     */
    std::string to_string() const {
      std::stringstream ss;

      ss << "CuboidData:" << std::endl;
      ss << "  x: (" << x[0] << ", " << x[1] << ", " << x[2] << ")"
         << std::endl;
      ss << "  v: (" << v[0] << ", " << v[1] << ", " << v[2] << ")"
         << std::endl;
      ss << "  N1: " << N1 << std::endl;
      ss << "  N2: " << N2 << std::endl;
      ss << "  N3: " << N3 << std::endl;
      ss << "  m: " << m << std::endl;
      ss << "  h: " << h << std::endl;
      ss << "  sigma: " << sigma << std::endl;
      ss << "  epsilon: " << epsilon << std::endl;

      return ss.str();
    }

    /**
     * @brief Compare two CuboidData structs. Used for Testing
     *
     * @return True if this CuboidData struct has the same content as other
     */
    bool operator==(const CuboidData &other) const {
      return (x == other.x && v == other.v && N1 == other.N1 &&
              N2 == other.N2 && N3 == other.N3 && m == other.m &&
              h == other.h && sigma == other.sigma &&
              epsilon == other.epsilon && avg_v == other.avg_v);
    }
  };

  struct ProgramArgs {
    // arguments of the simulation
    double delta_t;
    double t_end;

    std::string file_basename = "out";
    size_t write_frequency = 10;

    // spheres and cuboids to simulate
    std::vector<SphereData> spheres;
    std::vector<CuboidData> cuboids;

    std::string to_string() {
    
    std::ostringstream oss;

    oss << "Delta_t: " << delta_t << std::endl;
    oss << "T_end: " << t_end << std::endl;
    oss << "File basename: " << file_basename << std::endl;
    oss << "Write frequency: " << write_frequency << std::endl;

    oss << "Spheres: [" << std::endl;
    for (const auto& sphere : spheres) {
        oss << "  " << sphere.to_string() << std::endl;
    }
    oss << "]" << std::endl;

    oss << "Cuboids: [" << std::endl;
    for (const auto& cuboid : cuboids) {
        oss << "  " << cuboid.to_string() << std::endl;
    }
    oss << "]" << std::endl;

    return oss.str();
}
  };


  /**
   * @brief read all programm arguments
   * 
   * @param filename name from which the arguments are read (has to be in xml format)
   * 
  */
  ProgramArgs readProgramArguments(std::string filename);

  /**
   * @brief Reads particle data from a file and adds them into a
   * ParticleContainer
   *
   * reads particle data from a the file specified by filename(file has to have
   * specific format) and adds them inplace into the particleContainer
   *
   *
   * @param particleContainer reference to the ParticleContainer to add to
   * @param filename Filename of the file containing particle data
   */
  void readParticleFile(ParticleContainer &particleContainer, char *filename);

  /**
   * @brief Reads Cuboids of Particles from a file and returns a list of
   * CuboidData structs
   *
   * reads cuboids from a file specified by the given filename(file has to have
   * specific format). A cuboid in the file is just a list of the parameters
   * that are in the CuboidData struct. Turns the parameters given in the file
   * into a struct containing the parameters and creates a list of CuboidData
   * structs based on the read data.
   *
   * @param particleContainer reference to the ParticleContainer to add to
   * @param filename Filename of the file containing CuboidData
   *
   *
   * @return Returns a list of structs that contain the data of the Cuboids that
   * were read. Mainly used for testing and debugging purposes.
   *
   */
  std::list<CuboidData> readCuboidFile(std::string filename);
};
