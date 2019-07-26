/*
* BSD 3-Clause License
*
* Copyright (c) 2017, Shogun-Toolbox e.V. <shogun-team@shogun-toolbox.org>
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice, this
*   list of conditions and the following disclaimer.
*
* * Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*   and/or other materials provided with the distribution.
*
* * Neither the name of the copyright holder nor the names of its
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Written (W) 2017 Giovanni De Toni
*
*/

#ifndef SHOGUN_CROSSVALIDATIONSTORAGE_H
#define SHOGUN_CROSSVALIDATIONSTORAGE_H

#include <shogun/base/SGObject.h>
#include <shogun/evaluation/EvaluationResult.h>
#include <shogun/lib/SGVector.h>
#include <vector>

namespace shogun
{

	class CMachine;
	class CLabels;
	class CEvaluation;

	/**
	 * Store information about a single fold run.
	 */
	class CrossValidationFoldStorage : public CEvaluationResult
	{
	public:
		CrossValidationFoldStorage();
		virtual ~CrossValidationFoldStorage();

		virtual void print_result();

		/** post update test and true results
		 */
		virtual void post_update_results();

		/**
		 * Operator == needed for Any comparison
		 * @param rhs other CrossValidationFoldStorage
		 * @return true if the objects are the same, false otherwise.
		 */
		bool operator==(const CrossValidationFoldStorage& rhs) const;

		/**
		 * Class name (used for serialization)
		 * @return class name
		 */
		virtual const char* get_name() const
		{
			return "CrossValidationFoldStorage";
		};

	protected:
		/** Current run index is written here */
		index_t m_current_run_index;

		/** Current fold index is written here */
		index_t m_current_fold_index;

		/** Train indices */
		SGVector<index_t> m_train_indices;

		/** Test indices */
		SGVector<index_t> m_test_indices;

		/** Trained machine */
		CMachine* m_trained_machine;

		/** Test results */
		CLabels* m_test_result;

		/** Ground truth */
		CLabels* m_test_true_result;

		/** Evaluation result for this fold */
		float64_t m_evaluation_result;
	};

	/**
	 * This class store some information about CrossValidation runs.
	 */
	class CrossValidationStorage : public CEvaluationResult
	{
	public:
		/** Constructor */
		CrossValidationStorage();

		/** Destructor */
		virtual ~CrossValidationStorage();

		virtual void print_result();

		/**
		 * Class name (used for serialization)
		 * @return class name
		 */
		virtual const char* get_name() const
		{
			return "CrossValidationStorage";
		};

		/** Post init action. */
		virtual void post_init();

		/**
		 * Append a fold result to this storage
		 * @param result the result of a fold
		 */
		virtual void append_fold_result(CrossValidationFoldStorage* result);

		/**
		 * Operator == needed for Any comparison.
		 * @param rhs other CrossValidationStorage
		 * @return true if the objects are the same, false otherwise.
		 */
		bool operator==(const CrossValidationStorage& rhs) const;

	protected:
		/** number of runs is initialised here */
		index_t m_num_runs;

		/** number of folds is initialised here */
		index_t m_num_folds;

		/** Original labels */
		CLabels* m_original_labels;

		/** Vector with all the folds results */
		std::vector<CEvaluationResult*> m_folds_results;
	};
}

#endif // SHOGUN_CROSSVALIDATIONSTORAGE_H
