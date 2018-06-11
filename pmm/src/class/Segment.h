/*
 * Segment.h
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

#ifndef SRC_SEGMENT_H_
#define SRC_SEGMENT_H_

#include "ObservedValue.h"

#include <string>
#include <list>
#include <vector>
#include <valarray>

class Segment {
protected:
	/* protected member variables */
	const std::string _id;

public:
	/* constructor & destructor */
	Segment(std::string id);
	virtual ~Segment();

	/* public member variables */
	std::valarray<double> theta; /* mixing coefficient */

	/* public member functions */
	const std::string& getId(void);
	virtual void initLatentParams(size_t k);
};

#endif /* SRC_SEGMENT_H_ */
