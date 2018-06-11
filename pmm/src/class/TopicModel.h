/*
 * TopicModel.h
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

#ifndef SRC_TOPICMODEL_H_
#define SRC_TOPICMODEL_H_

#include <string>
#include <list>
#include <vector>
#include <valarray>
#include <unordered_map>

#include "SegmentObservingVector.h"

template<class SegmentT>
class TopicModel {
protected:
	/* protected member variables */
	const size_t _K; /* # of components */
	const size_t _D; /* data dimension */
	const size_t _valueSize;
	const bool _doSrand;

	size_t _nThres; /* minimum data size a segment must contain */
	std::vector<SegmentT*> _segments; /* list of segments */

	/* protected interfaces */
	virtual bool _readDataFileLine(
			std::unordered_map<std::string, SegmentT*>& hashtable, char *buf,
			bool forceadd) = 0;
	virtual bool _loadSegmentDataFromDump(FILE *fp) = 0;
	virtual bool _saveSegmentDataToDump(FILE *fp, SegmentT* seg) = 0;
	virtual void _dumpTopicParamsAndSegments(void) = 0;
	virtual void _Mstep(void) = 0;
	virtual double _numberOfModelParameters(void) = 0;
	virtual double _pdf(size_t s, size_t n, size_t k) = 0;

	/* protected member functions */
	void _clearSegments(void);
	void _hash2list(std::unordered_map<std::string, SegmentT*>& hashtable);
	SegmentT* _searchSegment(
			std::unordered_map<std::string, SegmentT*>& hashtable,
			std::string id, bool forceadd);
	void _Estep(void);
	double _finitePositiveValue(double x);

public:
	/* constructor & destructor */
	TopicModel(size_t k, size_t d, size_t valueSize, bool doSrand);
	TopicModel(size_t k, size_t d);
	virtual ~TopicModel();

	/* getter & setter */
	void setThres(size_t nThres);

	/* public interface */
	virtual void validateDataset(void) = 0;

	/* public member functions */
	void readDataFile(FILE *fp, bool forceadd);
	void loadDataDump(FILE *fp);
	void saveDataDump(const char *path);

	void printDataStats(void);
	void dump(void);

	double logLikelihood(void);
	void AIC(void);
	void EMAlgorithm(void);
};

template class TopicModel<SegmentObservingVector<int>>;
template class TopicModel<SegmentObservingVector<double>>;

#endif /* SRC_TOPICMODEL_H_ */
