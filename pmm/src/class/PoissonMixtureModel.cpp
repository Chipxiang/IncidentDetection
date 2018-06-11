/*
 * PoissonMixtureModel.cpp
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

#include "PoissonMixtureModel.h"
#include "../lib/util.h"

#include <iostream>
#include <cstdio>
#include <cstring>
#include <random>
#include <gsl/gsl_randist.h>

PoissonMixtureModel::PoissonMixtureModel(size_t k, size_t d, bool doSrand) :
		TopicModel(k, d, sizeof(int), doSrand) {
	_distParams.resize(_K);
	for (size_t k = 0; k < _K; ++k) {
		_distParams[k].resize(_D);
	}
	if (doSrand) {
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_real_distribution<> dist(0.0, 100.0);
		for (size_t k = 0; k < _K; ++k) {
			for (size_t d = 0; d < _D; ++d) {
				_distParams[k][d] = dist(rng);
			}
		}
	} else {
		for (size_t k = 0; k < _K; k++) {
			for (size_t d = 0; d < _D; ++d) {
				_distParams[k][d] = (double) rand() / (double) RAND_MAX * 100.0;
			}
		}
	}
}

PoissonMixtureModel::PoissonMixtureModel(size_t k, size_t d) :
		PoissonMixtureModel(k, d, true) {
}

bool PoissonMixtureModel::_readDataFileLine(
		std::unordered_map<std::string, SegmentObservingVector<int>*>& hashtable,
		char *buf, bool forceadd) {
	char *p = buf;
	const char *delim = ",";
	char *token;

	token = strsep(&p, delim);
	if (token == NULL) {
		std::cerr << "readDataFile: cannot read id" << std::endl;
		exit(1);
	}
	std::string id(token);

	int dataPoint[_D];
	for (size_t d = 0; d < _D; d++) {
		token = strsep(&p, delim);
		if (token == NULL) {
			std::cerr << "readDataFile: cannot read value" << std::endl;
			exit(1);
		}
		dataPoint[d] = atoi(token);
	}

	bool ret = _isValid(dataPoint);
	if (ret) {
		_addData(hashtable, id, dataPoint, forceadd);
	}
	return ret;
}

bool PoissonMixtureModel::_loadSegmentDataFromDump(FILE *fp) {
	size_t nData, nId;
	if (fread(&nData, sizeof(size_t), 1, fp) != 1)
		die("fread");
	if (fread(&nId, sizeof(size_t), 1, fp) != 1)
		die("fread");
	bool ret = (nData >= _nThres);
	if (ret) {
		char id[nId + 1];
		memset(id, 0, sizeof(char) * (nId + 1));
		if (fread(id, sizeof(char), nId, fp) != nId)
			die("fread");

		SegmentObservingVector<int> *seg = new SegmentObservingVector<int>(
				std::string(id));
		seg->data.clear();
		for (size_t j = 0; j < nData; j++) {
			int data[_D];
			if (fread(&data, _valueSize, _D, fp) != _D)
				die("fread");
			std::valarray<int> v(data, _D);
			ObservedValue<std::valarray<int>> ov(v);
			seg->data.push_back(ov);
		}

		seg->initLatentParams(_K);
		_segments.push_back(seg);
	} else { /* skip */
		if (fseek(fp, (long) nId, SEEK_CUR) < 0)
			die("fseek");
		long skip = _valueSize * _D * nData;
		if (fseek(fp, skip, SEEK_CUR) < 0)
			die("fseek");
	}
	return ret;
}

bool PoissonMixtureModel::_saveSegmentDataToDump(FILE *fp,
		SegmentObservingVector<int>* seg) {
	size_t n = seg->data.size();
	if (fwrite(&n, sizeof(size_t), 1, fp) != 1)
		die("fwrite");
	size_t l = seg->getId().length();
	if (fwrite(&l, sizeof(size_t), 1, fp) != 1)
		die("fwrite");
	if (fwrite(seg->getId().c_str(), sizeof(char), l, fp) != l)
		die("fwrite");
	size_t nd = seg->data.size() * _D;
	int data[nd];
	for (size_t n = 0; n < seg->data.size(); ++n) {
		for (size_t d = 0; d < _D; ++d) {
			data[n * _D + d] = seg->data[n].value[d];
		}
	}
	if (fwrite(data, sizeof(int), nd, fp) != nd)
		die("fwrite");
	return true;
}

bool PoissonMixtureModel::_isValid(int *dataPoint) {
	for (size_t d = 0; d < _D; d++) {
		if (dataPoint[d] < 0)
			return false;
	}
	return true;
}

void PoissonMixtureModel::_addData(
		std::unordered_map<std::string, SegmentObservingVector<int>*>& hashtable,
		std::string id, int *dataPoint, bool forceadd) {
	SegmentObservingVector<int> *seg = _searchSegment(hashtable, id,
			forceadd);
	if (seg != NULL) {
		std::valarray<int> dp(dataPoint, _D);
		ObservedValue<std::valarray<int>> d(dp);
		seg->addData(d);
	}
}

void PoissonMixtureModel::validateDataset(void) {
}

int PoissonMixtureModel::_compareDistParams(std::valarray<double> dp1,
		std::valarray<double> dp2) {
	for (size_t d = 0; d < _D; ++d) {
		if (dp1[d] < dp2[d]) {
			return -1;
		} else if (dp1[d] > dp2[d]) {
			return 1;
		}
	}
	return 0;
}

void PoissonMixtureModel::_dumpTopicParamsAndSegments(void) {
	/* sort by parameter */
	size_t order[_K];
	order[0] = 0;
	for (size_t k = 1; k < _K; k++) {
		bool flag = false;
		for (size_t l = 0; l < k; l++) {
			if (_compareDistParams(_distParams[k], _distParams[order[l]]) < 0) {
				for (size_t m = k; m > l; m--) {
					order[m] = order[m - 1];
				}
				order[l] = k;
				flag = true;
				break;
			}
		}
		if (!flag) {
			order[k] = k;
		}
	}

	printf("lambda\n");
	for (size_t k = 0; k < _K; ++k) {
		printf("%e", _distParams[order[k]][0]);
		for (size_t d = 1; d < _D; ++d) {
			printf(",%e", _distParams[order[k]][d]);
		}
		printf("\n");
	}
	printf("\n");

	/* mixing coefficient for each segment */
	/* Ns is the data size */
	printf("id,Ns,theta1,theta2,...\n");
	for (size_t s = 0; s < _segments.size(); s++) {
		SegmentObservingVector<int> *seg = _segments[s];
		printf("%s,%lu", seg->getId().c_str(), seg->data.size());
		for (size_t k = 0; k < _K; k++) {
			printf(",%e", seg->theta[order[k]]);
		}
		printf("\n");
	}
}

void PoissonMixtureModel::_Mstep(void) {
	size_t s, n, k, d;

	/* buffers */
	std::vector<std::valarray<double>> gam_l(_K,
			std::valarray<double>(0.0, _segments.size()));
	std::vector<std::vector<std::valarray<double>>>gam_x(_K,
			std::vector<std::valarray<double>>(_D,
					std::valarray<double>(0.0, _segments.size())));

#ifdef _OPENMP
#pragma omp parallel for private(n, k, d) schedule(dynamic)
#endif
	for (s = 0; s < _segments.size(); ++s) {
		SegmentObservingVector<int> *seg = _segments[s];
		for (k = 0; k < _K; ++k) {
			double gamma_total = 0.0;
			std::valarray<double> gamma_x_total(0.0, _D);
			for (n = 0; n < seg->data.size(); ++n) {
				gamma_total += seg->data[n].gamma[k];
				for (d = 0; d < _D; ++d) {
					gamma_x_total[d] += seg->data[n].gamma[k]
							* (double) seg->data[n].value[d];
				}
			}
			seg->theta[k] = gamma_total / (double) seg->data.size();
			for (d = 0; d < _D; ++d) {
				gam_x[k][d][s] = gamma_x_total[d];
			}
			gam_l[k][s] = gamma_total;
		}
	}

	for (k = 0; k < _K; k++) {
		double denom = gam_l[k].sum();
		for (d = 0; d < _D; ++d) {
			double num = _finitePositiveValue(gam_x[k][d].sum());
			_distParams[k] = num / denom;
		}
	}
}

double PoissonMixtureModel::_numberOfModelParameters(void) {
	return (_segments.size() + _D) * _K;
}

double PoissonMixtureModel::_pdf(size_t s, size_t n, size_t k) {
	SegmentObservingVector<int> *seg = _segments[s];
	std::valarray<double> p(_D);
	for (size_t d = 0; d < _D; ++d) {
		p[d] = gsl_ran_poisson_pdf(seg->data[n].value[d], _distParams[k][d]);
	}
	double ret = std::exp(std::log(p).sum());

#ifdef DEBUG
	switch (fpclassify(ret)) {
		case FP_NAN:
		case FP_INFINITE:
		fprintf(stderr, "a = %e (= %e * %e)\n", a, ret, tmp);
		switch (almighty->disttype) {
			case Poisson:
			fprintf(stderr, "x[d] = %u, lambda = %e\n",
					((unsigned int *) x->x)[d], poisson->lambda[d]);
			break;
			case Gamma:
			fprintf(stderr, "x[d] = %f, a = %e, b = %e\n",
					((double *) x->x)[d],
					poisson->lambda[2*d], poisson->lambda[2*d+1]);
			break;
			case SemiGamma:
			fprintf(stderr, "x[d] = %f, a = %e, 1/b = %e\n",
					((double *) x->x)[d],
					seg->k_of_semigamma, poisson->lambda[d]);
			break;
			default:
			break;
		}
		die("gsl_ran_poisson_pdf");
		break;
		case FP_ZERO:
		case FP_SUBNORMAL:
		case FP_NORMAL:
		break;
		default: // none
		break;
	}
#endif

	return ret;
}
