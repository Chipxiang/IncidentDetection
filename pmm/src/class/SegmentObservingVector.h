/*
 * SegmentObservingIntegerVector.h
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

#ifndef SRC_CLASS_SEGMENTOBSERVINGVECTOR_H_
#define SRC_CLASS_SEGMENTOBSERVINGVECTOR_H_

#include "Segment.h"
#include "ObservedValue.h"

template <typename T>
class SegmentObservingVector : public Segment {
public:
	/* public member variables */
	std::vector<ObservedValue<std::valarray<T>>> data; /* data sequence */

	/* constructor & destructor */
	SegmentObservingVector(std::string id);
	//virtual ~SegmentObservingVector() = default;

	/* public member functions */
	void addData(ObservedValue<std::valarray<T>>& data);
	virtual void initLatentParams(size_t k);
};

template class SegmentObservingVector<int>;
template class SegmentObservingVector<double>;

#endif /* SRC_CLASS_SEGMENTOBSERVINGVECTOR_H_ */
