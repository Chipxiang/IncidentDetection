/*
 * TopicModel.cpp
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

#include "TopicModel.h"

#include <iostream>
#include <cstdio>
#include <cmath>
#include <cfloat>
#include <algorithm>
#include <valarray>
#include <random>
#include <gsl/gsl_sf_log.h>
#include "../lib/util.h"

template<class T>
TopicModel<T>::TopicModel(size_t k, size_t d, size_t valueSize, bool doSrand) :
		_K(k), _D(d), _valueSize(valueSize), _doSrand(doSrand) {
	_nThres = 0;
	_segments.clear();
}

template<class T>
TopicModel<T>::~TopicModel() {
	_clearSegments();
}

template<class T>
void TopicModel<T>::_clearSegments(void) {
	if (!_segments.empty()) {
		for (auto itr = _segments.begin(); itr != _segments.end(); ++itr) {
			delete *itr;
		}
		_segments.clear();
	}
}

template<class T>
void TopicModel<T>::setThres(size_t nThres) {
	_nThres = nThres;
}

template<class T>
void TopicModel<T>::readDataFile(FILE *fp, bool forceadd) {
	std::unordered_map<std::string, T*> hashtable;
	size_t c = 0;
	char *buf = NULL;
	size_t n = 0;
	while (getline(&buf, &n, fp) >= 0) {
		if (_readDataFileLine(hashtable, buf, forceadd))
			++c;
	}
	free(buf);
	std::cerr << "added " << c << " records" << std::endl;
	_hash2list(hashtable);
}

template<class T>
void TopicModel<T>::_hash2list(std::unordered_map<std::string, T*>& hashtable) {
	_clearSegments();

	for (auto itr = hashtable.begin(); itr != hashtable.end(); ++itr) {
		T *seg = itr->second;
		if (seg->data.size() >= _nThres) {
			seg->initLatentParams(_K);
			_segments.push_back(seg);
		} else {
			delete seg;
		}
	}
}

/*
 * Load data from a binary dump file.
 * FILE *fp must be opened in binary mode.
 *
 * The binary file format is as follows:
 * Initial   sizeof(size_t) bytes: the dimension D
 * following sizeof(size_t) bytes: bytes per value T
 * following sizeof(size_t) bytes: number of segments S
 * repeat S times {
 *   initial   sizeof(size_t)   bytes: the number of data N
 *   following sizeof(size_t)   bytes: segment ID string length L
 *   following sizeof(char) * L bytes: segment ID string (does not contain trailing '\0')
 *   repeat N times {
 *      T * D bytes: observed data point vector
 *   }
 * }
 */
template<class T>
void TopicModel<T>::loadDataDump(FILE *fp) {
	_clearSegments();
	rewind(fp);

	/* read header */
	size_t d;
	if (fread(&d, sizeof(size_t), 1, fp) != 1)
		die("fread");
	if (d != _D) {
		std::cerr << "dimension mismatch:" << " assuming " << _D
				<< " but dump is " << d << "-D." << std::endl;
		exit(1);
	}
	size_t t;
	if (fread(&t, sizeof(size_t), 1, fp) != 1)
		die("fread");
	if (t != _valueSize) {
		std::cerr << "number type mismatch:" << " assuming " << _valueSize
				<< " bytes/value," << " but " << t << "." << std::endl;
		exit(1);
	}

	/* number of segments */
	size_t s;
	if (fread(&s, sizeof(size_t), 1, fp) != 1)
		die("fread");

	/* read segment data */
	size_t c = 0;
	for (size_t i = 0; i < s; i++) {
		if (_loadSegmentDataFromDump(fp))
			++c;
	}

	/* check */
	if (c != _segments.size()) {
		std::cerr << "number of segments mismatch:" << _segments.size()
				<< " vs " << c << std::endl;
		exit(1);
	}
}

template<class T>
void TopicModel<T>::saveDataDump(const char *path) {
	FILE *fp = fopen(path, "wb");
	if (fp == NULL)
		die("fopen");
	if (fwrite(&_D, sizeof(size_t), 1, fp) != 1)
		die("fwrite");
	if (fwrite(&_valueSize, sizeof(size_t), 1, fp) != 1)
		die("fwrite");
	size_t nSegs = _segments.size();
	if (fwrite(&nSegs, sizeof(size_t), 1, fp) != 1)
		die("fwrite");
	for (size_t s = 0; s < nSegs; s++) {
		if (!_saveSegmentDataToDump(fp, _segments[s])) {
			std::cerr << "failed dumping segment data." << std::endl;
			exit(1);
		}
	}
	if (fclose(fp) != 0)
		die("fclose");
}

template<class T>
T* TopicModel<T>::_searchSegment(std::unordered_map<std::string, T*>& hashtable,
		std::string id, bool forceadd) {
	T *res = NULL;
	auto itr = hashtable.find(id);
	if (itr != hashtable.end()) {
		/* found */
		res = itr->second;
	} else if (forceadd) {
		/* not found and force add */
		res = new T(id);
		hashtable[id] = res;
	}
	return res;
}

template<class T>
void TopicModel<T>::printDataStats(void) {
	size_t n = 0, _S = _segments.size();
	for (auto itr = _segments.begin(); itr != _segments.end(); ++itr) {
		n += (*itr)->data.size();
	}
	std::cerr << "total " << _S << " segments" << std::endl;
	std::cerr << "using " << n << " records" << std::endl;

	std::vector<size_t> vec(_S);

	unsigned long sum = 0;
	for (size_t s = 0; s < _S; s++) {
		vec[s] = _segments[s]->data.size();
		sum += vec[s];
	}
	std::sort(vec.begin(), vec.end());

	double mean = (double) sum / (double) _S;
	double var = 0.0;
	for (size_t s = 0; s < _S; s++) {
		double d = (double) (_segments[s]->data.size()) - mean;
		var += d * d;
	}
	var /= (double) _S;
	std::cerr << "n_data-mean = " << mean << ", n_data-variance = " << var
			<< std::endl;
	std::cerr << "n_data-min = " << vec[0] << ", n_data-max = " << vec[_S - 1]
			<< std::endl;
	std::cerr << "n_data-quantile: " << vec[_S / 4] << ", " << vec[_S / 2]
			<< ", " << vec[_S / 4 * 3] << std::endl;
}

template<class T>
void TopicModel<T>::dump(void) {
	std::cout << "# of Mixture : " << _K << std::endl;
	std::cout << "Dimension : " << _D << std::endl;

	_dumpTopicParamsAndSegments();
}

template<class T>
double TopicModel<T>::logLikelihood(void) {
	double res = 0.0;
	size_t s, n, k;

#ifdef _OPENMP
#pragma omp parallel for private(n, k) reduction(+:res)
#endif
	for (s = 0; s < _segments.size(); s++) {
		T *seg = _segments[s];
		for (n = 0; n < seg->data.size(); n++) {
			double tmp = 0.0;
			for (k = 0; k < _K; k++) {
				tmp += seg->theta[k] * _pdf(s, n, k);
			}
#ifdef DEBUG
			switch (fpclassify(tmp)) {
				case FP_INFINITE:
				case FP_NAN:
				case FP_SUBNORMAL:
				case FP_ZERO:
				fprintf(stderr, "tmp = %e\n", tmp);
				for (k = 0; k < almighty->K; k++) {
					fprintf(stderr, "theta[%u] = %e, po(x) = %e\n",
							k, seg->theta[k], pdf(almighty, s, k, seg->data[n]));
				}
				break;
				default: // FP_NORMAL
				break;
			}
#endif
			double r;
			if (tmp == 0.0) {
				tmp = DBL_MIN; /* avoiding zero... */
			}
			if (!std::isnormal(r = gsl_sf_log(tmp)))
				die("gsl_sf_log");
			res += r;
		}
	}
	return res;
}

template<class T>
void TopicModel<T>::AIC(void) {
	double params = _numberOfModelParameters();
	double likelihood = logLikelihood();

	fprintf(stderr, "AIC = %f\n", -2 * likelihood + 2 * params);
	fprintf(stderr, "1st term (likelihood) = %f\n", -2 * likelihood);
	fprintf(stderr, "2nd term (parameters) = %f\n", 2 * params);
}

template<class T>
void TopicModel<T>::EMAlgorithm(void) {
	_Estep();
	_Mstep();
}

template<class T>
void TopicModel<T>::_Estep(void) {
	size_t s, n, k;

	/* compute gamma[s][n][k] */
#ifdef _OPENMP
#pragma omp parallel for private(n, k) schedule(dynamic)
#endif
	for (s = 0; s < _segments.size(); s++) {
		T *seg = _segments[s];
		for (n = 0; n < seg->data.size(); n++) {
			std::valarray<double> g(_K);
			for (k = 0; k < _K; k++) {
				g[k] = _pdf(s, n, k);
			}
			seg->data[n].gamma = seg->theta * g;
			double gamma_denom = seg->data[n].gamma.sum();
#ifdef DEBUG
			if (fpclassify(gamma_denom) == FP_ZERO ||
					fpclassify(gamma_denom) == FP_SUBNORMAL) {
				fprintf(stderr, "gamma_denom=%e\n", gamma_denom);
				fprintf(stderr, "x[s][n] = %.20e\n", ((double *) seg->data[n]->x)[0]);
				for (k = 0; k < almighty->K; k++) {
					fprintf(stderr, "k=%u, theta=%e, g=%e, eta=%e\n",
							k, seg->theta[k], pdf(almighty, s, k, seg->data[n]), eta[k]);
					fprintf(stderr, "a=%e, b=%e, gamma(x|k)=%e\n",
							almighty->poissons[k]->lambda[0],
							almighty->poissons[k]->lambda[1],
							gsl_ran_gamma_pdf(((double *) seg->data[n]->x)[0],
									almighty->poissons[k]->lambda[0],
									almighty->poissons[k]->lambda[1]));
				}
			}
#endif
			/* compute gamma[s][n][k] */
			if (std::fpclassify(gamma_denom) == FP_ZERO) {
				/* assume uniform */
				seg->data[n].gamma = 1.0 / (double) _K;
			} else {
				seg->data[n].gamma /= gamma_denom;
			}
		} // end for [n]
	} // end for [s]
}

template<class T>
double TopicModel<T>::_finitePositiveValue(double x) {
	switch (std::fpclassify(x)) {
	case FP_NORMAL:
	case FP_SUBNORMAL:
		return x;
		break;
	case FP_ZERO:
		/* lambda must be positive; approx. by a small value */
		return DBL_MIN;
		break;
	case FP_INFINITE:
	case FP_NAN:
		std::cerr << "updated lambda = " << x << std::endl;
		std::cerr << "abort." << std::endl;
		exit(1);
		break;
	default:
		std::cerr << "Bad fortune. Abort." << std::endl;
		exit(2);
		break;
	}
}
