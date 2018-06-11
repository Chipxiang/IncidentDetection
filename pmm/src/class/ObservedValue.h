/*
 * ObservedValue.h
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

#ifndef SRC_OBSERVEDVALUE_H_
#define SRC_OBSERVEDVALUE_H_

#include <valarray>

template <typename T>
class ObservedValue {
public:
	const T value;			// x
	std::valarray<double> gamma;	// p(z | x; params)

	ObservedValue(const ObservedValue<T>& ov);
	ObservedValue(const T& v);
	virtual ~ObservedValue() = default;

	void initLatentParams(size_t k);
};

template class ObservedValue<double>;
template class ObservedValue<std::valarray<int>>;
template class ObservedValue<std::valarray<double>>;

#endif /* SRC_OBSERVEDVALUE_H_ */
