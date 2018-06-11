/*
 * csv2dump.c - convert CSV file to a binary dump file
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
#include <boost/program_options.hpp>
#include "class/PoissonMixtureModel.h"
#include "lib/util.h"

using namespace boost::program_options;

template<class T>
class CSV2Dump {
public:
	static void csv2dump(size_t d, FILE *fp, std::string outputPath) {
		T tm(1, d);
		tm.readDataFile(fp, true);
		if (fclose(fp) != 0)
			die("fclose");
		tm.saveDataDump(outputPath.c_str());
	}
};

int main(int argc, char **argv) {
	/* program options */
	options_description description("General options");
	options_description options("Program options");
	positional_options_description arguments;
	description.add_options()("help,h", "show help");
	options.add_options()
		("input,f", value<std::string>(), "input CSV file path")
		("dimension", value<size_t>(), "data dimension")
		("output", value<std::string>(), "output dump file path");
	arguments.add("dimension", 1);
	arguments.add("output", 1);

	description.add(options);

	/* parse parameters from command-line arguments */
	FILE *fp = stdin;
	size_t d;
	std::string outputPath;
	variables_map vm;
	try {
		store(
				command_line_parser(argc, argv).options(description).positional(
						arguments).run(), vm);
		notify(vm);

		if (vm.count("help")) {
			std::cout << "Usage: " << argv[0]
					<< " [options] <dimension> <outputPath>" << std::endl;
			std::cout << description << std::endl;
			exit(0);
		}

		if (vm.count("input")) {
			fp = fopen(vm["input"].as<std::string>().c_str(), "r");
			if (fp == NULL)
				die("fopen");
		}

		d = vm["dimension"].as<size_t>();
		outputPath = vm["output"].as<std::string>();

		CSV2Dump<PoissonMixtureModel>::csv2dump(d, fp, outputPath);
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
		exit(1);
	}

	return 0;
}
