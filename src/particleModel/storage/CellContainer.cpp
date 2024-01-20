#include <iostream>
#include "CellContainer.h"
#include "CellContainerIterators.h"
#include "particleModel/updating/CellCalculator.h"
#include <cmath>
#include <spdlog/spdlog.h>

dim_t dim_t_res = -1;

CellContainer::CellContainer(double d_width, double d_height, double d_depth, double r_cutoff, double cell_size)
            : cell_size(cell_size),cut_of_radius(r_cutoff), domain_bounds({d_width, d_height, d_depth})
              {

    domain_max_dim = {static_cast<dim_t>(d_width / cell_size + 1),
                      static_cast<dim_t>(d_height / cell_size + 1),
                      static_cast<dim_t>(d_depth / cell_size + 1)};
    //check if modulo would be 0
    if(isApproximatelyEqual(std::fmod(d_width, cell_size), 0.0)) {
        --domain_max_dim[0];
    }
    if(isApproximatelyEqual(std::fmod(d_height, cell_size), 0.0)) {
        --domain_max_dim[1];
    }
    if(isApproximatelyEqual(std::fmod(d_depth, cell_size), 0.0)) {
        --domain_max_dim[2];
    }

    particles.resize(static_cast<dim_t>(domain_max_dim[0] + 2),
                     std::vector<std::vector<std::vector<Particle*>>>(
                             static_cast<dim_t>(domain_max_dim[1] + 2),
                             std::vector<std::vector<Particle*>>(
                                     static_cast<dim_t>(domain_max_dim[2] + 2)
                             )
                     )
    );

    if (cell_size < r_cutoff) {
        comparing_depth = std::ceil(r_cutoff / cell_size);
    }

    if(domain_max_dim[2] <= 1) {
        domain_max_dim[2] = 1;

        if(domain_max_dim[0] <= comparing_depth) {
            throw std::invalid_argument("Domain width is too small for the r_cutoff to cell size ratio."
                                        "Consider increasing the cell size or using ParticleContainer instead.");

        } else if(domain_max_dim[1] < comparing_depth) {
            throw std::invalid_argument("Domain height is too small for the r_cutoff to cell size ratio."
                                        "Consider increasing the cell size.");
        }

        three_dimensions = false;

    } else {
        if(domain_max_dim[0] <= comparing_depth) {
            throw std::invalid_argument("Domain width is too small for the r_cutoff to cell size ratio."
                                        "Consider increasing the cell size.");

        } else if(domain_max_dim[1] <= comparing_depth) {
            throw std::invalid_argument("Domain height is too small for the r_cutoff to cell size ratio."
                                        "Consider increasing the cell size.");

        }
        else if(domain_max_dim[2] < comparing_depth) {
            throw std::invalid_argument("Domain depth is too small for the r_cutoff to cell size ratio."
                                        "Consider increasing the cell size.");
        }

        three_dimensions = true;
    }

    //precalculate all the patterns
    std::array<dim_t, 3> tmp{};
    std::array<dim_t, 3> pattern{};

    if(three_dimensions) {
        setNext3dPattern(pattern, tmp);

    } else {
        setNext2dPattern(pattern, tmp);
    }

    while (tmp[0] != dim_t_res) {
        patterns.emplace_back(pattern);

        if(three_dimensions) {
            setNext3dPattern(pattern, tmp);
        } else {
            setNext2dPattern(pattern, tmp);
        }
    }

    patterns.shrink_to_fit();

    std::ostringstream out_str;
    out_str << "Domain cells x: 1 - " << (domain_max_dim[0]) <<  " y: 1 - " << (domain_max_dim[1]) << " z: 1 - "  << (domain_max_dim[2]) << std::endl;
    SPDLOG_INFO(out_str.str());
}

CellContainer::~CellContainer() {}

void CellContainer::setNextCell(std::array<dim_t, 3> &next_position) {
    static dim_t x = 1;
    static dim_t y = 1;
    static dim_t z = 1;

    //previous call set x to reserved value, indicating the end
    if(x == dim_t_res) {
        next_position[0] = dim_t_res;
        x = 1;
        y = 1;
        z = 1;
        return;
    }

    while(particles[x][y][z].empty()) {

        if(x < domain_max_dim[0]) {
            ++x;

        }else if(y < domain_max_dim[1]) {
            x = 1;
            ++y;

        } else if(z < domain_max_dim[2]) {
            x = 1;
            y = 1;
            ++z;

        } else {
            //done
            next_position[0] = dim_t_res;
            x = 1;
            y = 1;
            z = 1;
            return;
        }
    }

    next_position[0] = x;
    next_position[1] = y;
    next_position[2] = z;

    if (x < domain_max_dim[0]) {
        ++x;
    }else if (y < domain_max_dim[1]) {
        x = 1;
        ++y;
    } else if (z < domain_max_dim[2]) {
        x = 1;
        y = 1;
        ++z;
    } else {
        //done
        x = dim_t_res;
    }
}

enum direction_status {
    first_subset, second_subset, third_subset
};


void CellContainer::setNext3dPattern(std::array<dim_t, 3> &pattern, std::array<dim_t, 3> &start) {
    static direction_status status = first_subset;

    switch (status) {
        case (first_subset):
            // (0, 0, 1 to depth)
            if (pattern[2] < comparing_depth) {
                ++pattern[2];
            } else {
                pattern[0] = 1;
                pattern[2] = -comparing_depth;
                status = second_subset;
            }
            break;

        case (second_subset):
            // (1 to depth, 0, -depth to depth)
            if (pattern[0] < comparing_depth) {
                ++pattern[0];

            } else if (pattern[2] < comparing_depth) {
                pattern[0] = 1;
                ++pattern[2];

            } else {
                pattern[0] = -comparing_depth;
                pattern[1] = 1;
                pattern[2] = -comparing_depth;
                status = third_subset;
            }
            break;

        case (third_subset):
            // (-depth to depth, 1 to depth, -depth to depth)
            if (pattern[0] < comparing_depth) {
                ++pattern[0];

            } else if (pattern[1] < comparing_depth) {
                pattern[0] = -comparing_depth;
                ++pattern[1];

            } else if (pattern[2] < comparing_depth) {
                pattern[0] = -comparing_depth;
                pattern[1] = 1;
                ++pattern[2];
            } else {
                //finished
                pattern[0] = 0;
                pattern[1] = 0;
                pattern[2] = 0;
                status = first_subset;
                start[0] = dim_t_res;
                return;
            }
            break;
    }
}

void CellContainer::setNext2dPattern(std::array<dim_t, 3> &pattern, std::array<dim_t, 3> &start) {
    static direction_status status = first_subset;

    switch (status) {
        case (first_subset):
            // (1 to depth, 0, 0)
            if (pattern[0] < comparing_depth) {
                ++pattern[0];

            } else {
                pattern[0] = -comparing_depth;
                pattern[1] = 1;
                status = second_subset;
            }
            break;

        case (second_subset):
            // (-depth to depth, 1 to depth, 0)
            if (pattern[0] < comparing_depth) {
                ++pattern[0];

            } else if (pattern[1] < comparing_depth) {
                pattern[0] = -comparing_depth;
                ++pattern[1];

            } else {
                //finished
                pattern[0] = 0;
                pattern[1] = 0;
                pattern[2] = 0;
                status = first_subset;
                start[0] = dim_t_res;
                return;
            }
            break;
    }
}

void CellContainer::setNextPath(std::array<dim_t, 3> &start, std::array<dim_t, 3> &pattern) {
    static std::array<dim_t, 3> current_pattern{0,0,0};
    static std::array<dim_t, 3> begin{1,1,1};
    static std::array<dim_t, 3> end(domain_max_dim);
    static dim_t tmp_x = 0;
    static dim_t tmp_y = 0;
    static dim_t tmp_z = 0;
    static dim_t s_x, s_y, s_z;

    if(tmp_x == 0 && tmp_y == 0 && tmp_z == 0) {
        //get next pattern

        if(three_dimensions) {
            setNext3dPattern(current_pattern, start);
        } else {
            setNext2dPattern(current_pattern, start);
        }

        tmp_x = current_pattern[0];
        tmp_y = current_pattern[1];
        tmp_z = current_pattern[2];
        s_x = 1;
        s_y = 1;
        s_z = 1;
        begin = {1,1,1};
        end = domain_max_dim;
    }

    pattern[0] = current_pattern[0];
    pattern[1] = current_pattern[1];
    pattern[2] = current_pattern[2];

    //get next start
    if(tmp_x != 0) {
        start[1] = s_y;
        start[2] = s_z;

        if(0 < tmp_x) {
            start[0] = tmp_x;
            begin[0] = current_pattern[0] + 1;
        } else {
            start[0] = tmp_x + domain_max_dim[0] + 1;
            end[0] = domain_max_dim[0] + current_pattern[0];
        }

        if(s_y < end[1]) {
            ++s_y;

        } else if(s_z < end[2]) {
            s_y = 1;
            ++s_z;

        } else {
            s_y = 1;
            s_z = 1;

            if(0 < tmp_x) {
                s_x = begin[0];
                --tmp_x;
            } else {
                ++tmp_x;
            }
        }
    } else if(tmp_y != 0) {
        start[0] = s_x;
        start[2] = s_z;

        if(0 < tmp_y) {
            start[1] = tmp_y;
            begin[1] = current_pattern[1] + 1;
        } else {
            start[1] = tmp_y + domain_max_dim[1] + 1;
            end[1] = domain_max_dim[1] + current_pattern[1];
        }

        if(s_x < end[0]) {
            ++s_x;

        } else if(s_z < end[2]) {
            s_x = begin[0];
            ++s_z;

        } else {
            s_x = begin[0];
            s_z = 1;

            if(0 < tmp_y) {
                s_y = begin[1];
                --tmp_y;
            } else {
                ++tmp_y;
            }
        }
    } else if(tmp_z != 0) {
        start[0] = s_x;
        start[1] = s_y;

        if(0 < tmp_z) {
            start[2] = tmp_z;
        } else {
            start[2] = tmp_z + domain_max_dim[2] + 1;
        }

        if(s_x < end[0]) {
            ++s_x;

        } else if(s_y < end[1]) {
            s_x = begin[0];
            ++s_y;

        } else {
            s_x = begin[0];
            s_y = begin[1];

            if(0 < tmp_z) {
                --tmp_z;
            } else {
                ++tmp_z;
            }
        }
    }
}


CellContainer::CustomIterator CellContainer::begin_custom(dim_t low_x, dim_t upp_x,
                                                          dim_t low_y, dim_t upp_y,
                                                          dim_t low_z, dim_t upp_z){
    return CustomIterator(*this,low_x,upp_x,low_y,upp_y,low_z,upp_z);

}

CellContainer::CustomIterator CellContainer::end_custom(){
    return CustomIterator(*this,-1,1,1,1,1,1);
}

CellContainer::Iterator CellContainer::begin(){
    return  Iterator(*this);
}

CellContainer::Iterator CellContainer::end(){
    //corresponds to last index
    return Iterator(*this,-1,1,1);
}

std::vector<Particle*>::iterator CellContainer::begin_halo(){
    return halo_particles.begin();
}

std::vector<Particle*>::iterator CellContainer::end_halo(){
    return halo_particles.end();
}


void CellContainer::addParticle(std::array<double, 3> x_arg, std::array<double, 3> v_arg, double m_arg) {
    addParticle(x_arg, v_arg, m_arg, sigma_mixed[0][0], epsilon_mixed[0][0]);
}

void CellContainer::addParticle(const Particle& particle,double sigma, double epsilon){
    addParticle(particle.getX(),particle.getV(),particle.getM(),sigma,epsilon);
}


void CellContainer::addParticle(std::array<double, 3> x_arg, std::array<double, 3> v_arg,
                                double m_arg, double sigma, double epsilon) {
    if(domain_bounds[0] <= x_arg[0] || domain_bounds[1] <= x_arg[1] || (domain_bounds[2] <= x_arg[2] && three_dimensions) ||
      x_arg[0] < 0 || x_arg[1] < 0 || x_arg[2] < 0)
    {
        std::cerr << "Wanted to add at: " << x_arg[0] << " , " << x_arg[1] << " , " << x_arg[2] << "\n";
        throw std::invalid_argument("The provided coordinates are outside the domain borders.");
    }

    int type = 0;
    static std::vector<std::array<double,2>> types;

    //find the existing type/sigma-epsilon-pair
    auto it = std::find_if(types.begin(), types.end(), [sigma, epsilon](std::array<double,2> pair) {
       return pair[0] == sigma && pair[1] == epsilon;
    });

    if(it != types.end()) {
        type = std::distance(types.begin(), it);
    } else {
        //add new type and update Lorentz-Berthelot mixing matrices
        type = types.size();
        types.emplace_back(std::array<double,2>{sigma, epsilon});

        sigma_mixed.clear();
        sigma_mixed.resize(types.size(), std::vector<double>(types.size()));
        epsilon_mixed.clear();
        epsilon_mixed.resize(types.size(), std::vector<double>(types.size()));

        for (int i = 0; i < types.size(); ++i) {
            for (int j = 0; j < types.size(); ++j) {

                sigma_mixed[i][j] = (types[i][0] + types[j][0]) / 2;
                epsilon_mixed[i][j] = std::sqrt(types[i][1] * types[j][1]);
            }
        }
    }

    particle_instances.emplace_back(x_arg, v_arg, m_arg, type);
}


void CellContainer::allocateCellFromPosition(const std::array<double, 3> &x, std::array<dim_t , 3> &cell_position) {
    cell_position[0] = std::floor(x[0] / cell_size + 1);
    cell_position[1] = std::floor(x[1] / cell_size + 1);
    cell_position[2] = std::floor(x[2] / cell_size + 1);

    //cover edge case with last cell being less than cell_size
    if(domain_bounds[0] < x[0]) {
        cell_position[0] = std::ceil((x[0] - domain_bounds[0]) / cell_size + domain_max_dim[0]);
    }
    if(domain_bounds[1] < x[1]) {
        cell_position[1] = std::ceil((x[1] - domain_bounds[1]) / cell_size + domain_max_dim[1]);
    }
    if(domain_bounds[2] < x[2]) {
        cell_position[2] = std::ceil((x[2] - domain_bounds[2]) / cell_size + domain_max_dim[2]);
    }
}


bool CellContainer::isApproximatelyEqual(double a, double b, double epsilon) {
    return std::abs(a - b) < epsilon;
}


void CellContainer::createPointers(){
    for(Particle& particle : particle_instances){
        static std::array<dim_t , 3> pos;
        std::array<double,3> x_arg = particle.getX();
        allocateCellFromPosition(x_arg, pos);
        particles.at(pos[0]).at(pos[1]).at(pos[2]).push_back(&particle);
        particle_amount++;
    }
}


void CellContainer::plotParticles(outputWriter::VTKWriter &writer) {
    std::array<dim_t, 3> current_position;
  
    setNextCell(current_position);

    while(current_position[0] != dim_t_res) {
        std::vector<Particle*> *current_cell = &particles[current_position[0]][current_position[1]][current_position[2]];

        for(Particle* particle : *current_cell){
            writer.plotParticle(*particle);
        }
        setNextCell(current_position);
    }
}


std::string CellContainer::to_string() {
  std::ostringstream out_str;  

    size_t amt = 0;

  out_str << "The actual domain has  \n" <<  particles.size() <<  " cells in x dir. \n" << particles[0].size() << " cells in y dir. \n"  << particles[0][0].size() << " cells in z dir." << std::endl;
  out_str << "The actual domain is from  \nx: 1 - " << (domain_max_dim[0]) <<  "(domain_max_dim[0])\ny: 1 - " << (domain_max_dim[1]) << "(domain_max_dim[1])\nz: 1 - "  << (domain_max_dim[2]) << "(domain_max_dim[2])" << std::endl;
    out_str << "Are we in 3d?: " <<  (three_dimensions ? "Yes" : "No") << std::endl;
    out_str << "cell_size: " <<  cell_size << std::endl;
    out_str << "comparing_depth: " <<  comparing_depth << std::endl;
    out_str << "domain_bounds [0]:" << domain_bounds[0] << " [1]:" << domain_bounds[1] << " [2]:" << domain_bounds[2]  << std::endl;

  std::array<dim_t, 3> current_position;
  setNextCell(current_position);  
  while(current_position[0] != dim_t_res){
    out_str << "The cell with index x=" << current_position[0] << " y=" << current_position[1] << " z=" << current_position[2] << std::endl;
    out_str << "Has the following Particles: " << std::endl;

    std::vector<Particle*>* cell = &particles[current_position[0]][current_position[1]][current_position[2]];
    for(auto* particle : *cell){
      out_str << (*particle).toString() << std::endl;
      amt++;
    }
    out_str << "\n\n";
    setNextCell(current_position);  
  }


    out_str << "in total amt: " << size() << std::endl;

  return out_str.str();
}


std::list<Particle> CellContainer::to_list(){
    std::list<Particle> particles;
    for(auto iter = begin(); iter != end(); ++iter){
        for(Particle* particle_ptr : *iter){
            particles.push_back(*particle_ptr);
        }
    }
    return particles;
}


size_t CellContainer::size() {
    return particle_instances.size();
}

std::array<dim_t, 3> CellContainer::domain_max_dim{};
std::vector<std::array<dim_t,3>> CellContainer::patterns{};
std::vector<std::vector<std::vector<std::vector<Particle*>>>> CellContainer::particles{};









