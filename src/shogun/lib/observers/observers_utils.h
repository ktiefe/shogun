/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Giovanni De Toni
 *
 */

#ifndef SHOGUN_OBSERVEDVALUE_H
#define SHOGUN_OBSERVEDVALUE_H

#include <chrono>
#include <utility>

#include <shogun/base/SGObject.h>
#include <shogun/base/some.h>

/**
 * Definitions of basic object with are needed by the Parameter
 * Observer architecture.
 */
namespace shogun
{
	/* Timepoint */
	typedef std::chrono::steady_clock::time_point time_point;

	/**
	 * Observed value with a timestamp
	 */
	typedef std::pair<Some<ObservedValue>, time_point> TimedObservedValue;

	/**
	 * Helper method to convert a time_point to milliseconds
	 * @param value time point we want to convert
	 * @return the time point converted to milliseconds
	 */
	SG_FORCED_INLINE auto convert_to_millis(const time_point& value)
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(
		           value.time_since_epoch())
		    .count();
	}
}

#endif // SHOGUN_OBSERVEDVALUE_H
