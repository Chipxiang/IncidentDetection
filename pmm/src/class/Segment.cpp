/*
 * Segment.cpp
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

#include "Segment.h"

Segment::Segment(std::string id) :
		_id(id) {
	theta.resize(0);
}

Segment::~Segment() {
}

const std::string& Segment::getId(void) {
	return _id;
}

void Segment::initLatentParams(size_t k) {
	theta.resize(k);
	theta = 1.0 / (double) k;
}
