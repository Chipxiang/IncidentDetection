/*
 * util.c
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

#include "util.h"
#include <stdio.h>
#include <stdlib.h>

void die(const char *str) {
	perror(str);
	exit(1);
}

#if defined(__GNUC__)
#  pragma GCC push_options
#  pragma GCC optimize ("O1")
#endif
double kahanSum(double *a, size_t n) {
	double sum = 0.0, delta = 0.0;
	size_t i;
	for (i = 0; i < n; i++) {
		double t1 = a[i] - delta;
		double t2 = sum + t1;
		delta = (t2 - sum) - t1;
		sum = t2;
	}
	return sum;
}
#if defined(__GNUC__)
#  pragma GCC pop_options
#endif
