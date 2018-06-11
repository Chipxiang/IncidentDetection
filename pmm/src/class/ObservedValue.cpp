/*
 * ObservedValue.cpp
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

#include "ObservedValue.h"

#include <valarray>

template<typename T>
ObservedValue<T>::ObservedValue(const ObservedValue<T>& ov) :
		value(ov.value) {
	gamma = ov.gamma;
}

template<typename T>
ObservedValue<T>::ObservedValue(const T& v) :
		value(v) {
	gamma.resize(0);
}

template<typename T>
void ObservedValue<T>::initLatentParams(size_t k) {
	gamma.resize(k);
	gamma = 1.0 / (double) k;
}
