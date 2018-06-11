/*
 * PoissonMixtureModel.h
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

#ifndef SRC_CLASS_POISSONMIXTUREMODEL_H_
#define SRC_CLASS_POISSONMIXTUREMODEL_H_

#include "TopicModel.h"
#include <valarray>
#include <unordered_map>
#include "SegmentObservingVector.h"

class PoissonMixtureModel: public TopicModel<SegmentObservingVector<int>> {
private:
	/* private member variables */
	std::vector<std::valarray<double>> _distParams;

	/* private member functions */
	bool _isValid(int *dataPoint);
	void _addData(
			std::unordered_map<std::string, SegmentObservingVector<int>*>& hashtable,
			std::string id, int *dataPoint, bool forceadd);
	int _compareDistParams(std::valarray<double> dp1, std::valarray<double> dp2);

protected:
	/* protected member variables */
	const size_t _D = 1;

	/* protected member interface implementation */
	virtual bool _readDataFileLine(
			std::unordered_map<std::string, SegmentObservingVector<int>*>& hashtable,
			char *buf, bool forceadd);
	virtual bool _loadSegmentDataFromDump(FILE *fp);
	virtual bool _saveSegmentDataToDump(FILE *fp,
			SegmentObservingVector<int>* seg);
	virtual void _dumpTopicParamsAndSegments(void);
	virtual void _Mstep(void);
	virtual double _numberOfModelParameters(void);
	virtual double _pdf(size_t s, size_t n, size_t k);

public:
	/* constructor & destructor */
	PoissonMixtureModel(size_t k, size_t d, bool doSrand);
	PoissonMixtureModel(size_t k, size_t d);
	virtual ~PoissonMixtureModel() = default;

	/* public member interface implementation */
	virtual void validateDataset(void);
};

#endif /* SRC_CLASS_POISSONMIXTUREMODEL_H_ */
