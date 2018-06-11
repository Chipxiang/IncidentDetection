/*
 * estimate.c - main program for maximum-likelihood estimation of PMM
 *
 * Copyright (C) 2016  Akira Kinoshita <kinoshita@nii.ac.jp>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <chrono>
#include <time.h>
#include <boost/program_options.hpp>
#include "class/PoissonMixtureModel.h"
#include "lib/util.h"

using namespace std::chrono;
using namespace boost::program_options;

//T is a template parameter
template<class T>
class Estimator {
public:
	static void estimate(size_t k, size_t d, size_t nItr, size_t nThres,	//size_t = unsigned int(32-bit)/long unsigned int(64-bit)
			std::string dumpPath, bool fixedItr, bool doSrand) {
		std::cerr << "K = " << k << ", D = " << d << ", N_ITER = " << nItr
				<< std::endl;
		T tm(k, d, doSrand);
		tm.setThres(nThres);

		/* load data */
		if (dumpPath.empty()) {	// if not using dump file, but csv file
			tm.readDataFile(stdin, true);
		} else {	// if using dumpfile
			FILE *fp = fopen(dumpPath.c_str(), "rb");
			if (fp == NULL)
				die("fopen");
			tm.loadDataDump(fp);
			if (fclose(fp) != 0)
				die("fclose");
		}
		tm.validateDataset();
		tm.printDataStats();

		/* EM! */
		double prev, now;	// previous log-likelihood and current log-likelihood
		size_t n_conv = 0;
		auto start = std::chrono::system_clock::now();
		clock_t start_c = clock();
		prev = DBL_MIN;
		for (size_t i = 0; i < nItr; i++) {
#ifdef debug	//output debug info
			std::cerr << std::endl;
			for (size_t k = 0; k < hb.K; k++) {
				for (size_t d = 0; d < hb.D; d++) {
					std::cerr << almighty->poissons[k]->lambda[d] << ",";
				}
				std::cerr << std::endl;
			}
#endif
			tm.EMAlgorithm();
			now = tm.logLikelihood();
			std::cerr << i + 1 << " " << now << " " << now - prev << std::endl;
			if (!fixedItr && i > 0) {
				/* convergence test */
				if (fabs((now - prev) / now) < 0.001) {	// the difference percentage is less than 1%
					if (++n_conv >= 3) {	// for continuous 3 times
						break;
					}
				} else {
					n_conv = 0;
				}
			}
			prev = now;
		}
		auto end = system_clock::now();
		clock_t end_c = clock();
		std::cerr << "elapsed real time: "
				<< duration_cast<microseconds>(end - start).count() * 1e-6
				<< "s" << std::endl;
		std::cerr << "elapsed CPU time: "
				<< (double) (end_c - start_c) / CLOCKS_PER_SEC << "s"
				<< std::endl;

		/* print result */
		tm.dump();
		tm.AIC();
	}
};

int main(int argc, char **argv) {
	/* program options */
	//seems you can use bin/estimate -o(options) to use options
	options_description description1("General options");
	options_description description2("Program options");
	description1.add_options()("help,h", "show help");
	description2.add_options()
		("noSrand,f", "do not srand() (for debug)")	//srand() is used to provide seeds for rand() funcion
		("dumpPath,b", value<std::string>(), "use dump file instead of csv file")
		("dim,d", value<size_t>()->default_value(1), "data dimension")	//only speed
		("nmix,k", value<size_t>()->default_value(4), "number of mixture")	// number of traffic states
		("maxItr,i", value<size_t>()->default_value(20), "maximum number of iterations")
		("fixedItr,c", "fix the number of iterations")
		("minData,t", value<size_t>()->default_value(1), "minimum number of records per segment"); // what is it used for
	description1.add(description2);

	/* parse parameters from command-line arguments */
	variables_map vm;
	try {
		// parse the command-line, take argc and argv, which define the command-line, and desc, which contain description of options
		// and store the options in vm
		store(parse_command_line(argc, argv, description1), vm);
		// should call notify() on the completed variable map
		notify(vm);
		
		// to check whether "help" option is in the command-line
		if (vm.count("help")) {
			std::cout << description1 << std::endl;
			exit(0);
		}

		bool doSrand = true;
		bool fixedItr = false;
		std::string dumpPath;
		if (vm.count("fixedItr"))
			fixedItr = true;
		if (vm.count("noSrand"))
			doSrand = false;
		if (vm.count("dumpPath"))
			dumpPath = vm["dumpPath"].as<std::string>();

		size_t k = vm["nmix"].as<size_t>();
		size_t d = vm["dim"].as<size_t>();
		size_t nItr = vm["maxItr"].as<size_t>();
		size_t nThres = vm["minData"].as<size_t>();

		Estimator<PoissonMixtureModel>::estimate(k, d, nItr, nThres,
				dumpPath, fixedItr, doSrand);
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
		exit(1);
	}

	return 0;
}
