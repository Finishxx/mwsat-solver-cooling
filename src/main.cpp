#include <iostream>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

// https://www.boost.org/doc/libs/1_86_0/doc/html/program_options/tutorial.html#id-1.3.30.4.3
int main(int ac, char ** av) {
  // Declare the supported options.
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("compression", po::value<int>(), "set compression level")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(ac, av, desc), vm);
  po::notify(vm);    

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 1;
  }

  if (vm.count("compression")) {
    std::cout << "Compression level was set to " 
 << vm["compression"].as<int>() << ".\n";
  } else {
    std::cout << "Compression level was not set.\n";
  }

}
