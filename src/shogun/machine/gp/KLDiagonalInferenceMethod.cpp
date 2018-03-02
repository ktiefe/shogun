 /*
 * Copyright (c) The Shogun Machine Learning Toolbox
 * Written (w) 2014 Wu Lin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the Shogun Development Team.
 *
 * Code adapted from
 * http://hannes.nickisch.org/code/approxXX.tar.gz
 * and Gaussian Process Machine Learning Toolbox
 * http://www.gaussianprocess.org/gpml/code/matlab/doc/
 * and the reference paper is
 * Challis, Edward, and David Barber.
 * "Concave Gaussian variational approximations for inference in large-scale Bayesian linear models."
 * International conference on Artificial Intelligence and Statistics. 2011.
 *
 * This code specifically adapted from function in approxKL.m and infKL.m
 */

#include <shogun/machine/gp/KLDiagonalInferenceMethod.h>

#include <shogun/mathematics/eigen3.h>
#include <shogun/mathematics/Math.h>
#include <shogun/mathematics/Statistics.h>
#include <shogun/machine/gp/VariationalGaussianLikelihood.h>

using namespace Eigen;

namespace shogun
{

CKLDiagonalInferenceMethod::CKLDiagonalInferenceMethod() : CKLLowerTriangularInference()
{
	init();
}

CKLDiagonalInferenceMethod::CKLDiagonalInferenceMethod(CKernel* kern,
		CFeatures* feat, CMeanFunction* m, CLabels* lab, CLikelihoodModel* mod)
		: CKLLowerTriangularInference(kern, feat, m, lab, mod)
{
	init();
}

void CKLDiagonalInferenceMethod::init()
{
	SG_ADD(&m_InvK, "invK",
		"The K^{-1} matrix",
		MS_NOT_AVAILABLE);
}

CKLDiagonalInferenceMethod* CKLDiagonalInferenceMethod::obtain_from_generic(
		CInference* inference)
{
	if (inference==NULL)
		return NULL;

	if (inference->get_inference_type()!=INF_KL_DIAGONAL)
		SG_SERROR("Provided inference is not of type CKLDiagonalInferenceMethod!\n")

	SG_REF(inference);
	return (CKLDiagonalInferenceMethod*)inference;
}

SGVector<float64_t> CKLDiagonalInferenceMethod::get_alpha()
{
	/** Note that m_alpha contains not only the alpha vector defined in the reference
	 * but also a vector corresponding to the lower triangular of C
	 *
	 * Note that alpha=K^{-1}(mu-mean), where mean is generated from mean function,
	 * K is generated from cov function
	 * and mu is not only the posterior mean but also the variational mean
	 */
	if (parameter_hash_changed())
		update();

	index_t len=m_mu.vlen;
	SGVector<float64_t> result(len);

	Map<VectorXd> eigen_result(result.vector, len);
	Map<VectorXd> eigen_alpha(m_alpha.vector, len);

	eigen_result=eigen_alpha;
	return result;
}

CKLDiagonalInferenceMethod::~CKLDiagonalInferenceMethod()
{
}

bool CKLDiagonalInferenceMethod::precompute()
{
	index_t len=m_mean_vec.vlen;
	Map<VectorXd> eigen_mean(m_mean_vec.vector, m_mean_vec.vlen);
	Map<MatrixXd> eigen_K(m_ktrtr.matrix, m_ktrtr.num_rows, m_ktrtr.num_cols);
	Map<VectorXd> eigen_alpha(m_alpha.vector, len);

	Map<VectorXd> eigen_mu(m_mu.vector, m_mu.vlen);
	//mu=K*alpha+m
	eigen_mu = eigen_K * std::exp(m_log_scale * 2.0) * eigen_alpha + eigen_mean;

	Map<VectorXd> eigen_log_v(m_alpha.vector+len, m_alpha.vlen-len);
	Map<VectorXd> eigen_s2(m_s2.vector, m_s2.vlen);
	//s2=sum(C.*C,2);
	eigen_s2=eigen_log_v.array().exp();

	CVariationalGaussianLikelihood * lik=get_variational_likelihood();
	bool status=lik->set_variational_distribution(m_mu, m_s2, m_labels);
	return status;
}

void CKLDiagonalInferenceMethod::get_gradient_of_nlml_wrt_parameters(SGVector<float64_t> gradient)
{
	REQUIRE(gradient.vlen==m_alpha.vlen,
		"The length of gradients (%d) should the same as the length of parameters (%d)\n",
		gradient.vlen, m_alpha.vlen);

	Map<MatrixXd> eigen_K(m_ktrtr.matrix, m_ktrtr.num_rows, m_ktrtr.num_cols);
	Map<MatrixXd> eigen_InvK(m_InvK.matrix, m_InvK.num_rows, m_InvK.num_cols);

	index_t len=m_mu.vlen;
	Map<VectorXd> eigen_alpha(m_alpha.vector, len);
	Map<VectorXd> eigen_s2(m_s2.vector, m_s2.vlen);

	CVariationalGaussianLikelihood * lik=get_variational_likelihood();
	//[a,df,dV] = a_related2(mu,s2,y,lik);
	TParameter* s2_param=lik->m_parameters->get_parameter("sigma2");
	SGVector<float64_t> dv=lik->get_variational_first_derivative(s2_param);
	Map<VectorXd> eigen_dv(dv.vector, dv.vlen);

	TParameter* mu_param=lik->m_parameters->get_parameter("mu");
	SGVector<float64_t> df=lik->get_variational_first_derivative(mu_param);
	Map<VectorXd> eigen_df(df.vector, df.vlen);

	Map<VectorXd> eigen_dnlz_alpha(gradient.vector, len);
	//dnlZ_alpha  = -K*(df-alpha);
	eigen_dnlz_alpha =
		eigen_K * std::exp(m_log_scale * 2.0) * (-eigen_df + eigen_alpha);

	Map<VectorXd> eigen_dnlz_log_v(gradient.vector+len, gradient.vlen-len);

	eigen_dnlz_log_v=(eigen_InvK.diagonal().array()-(1.0/eigen_s2.array()));
	eigen_dnlz_log_v=(0.5*eigen_dnlz_log_v.array())-eigen_dv.array();
	eigen_dnlz_log_v=eigen_dnlz_log_v.array()*eigen_s2.array();

}

float64_t CKLDiagonalInferenceMethod::get_negative_log_marginal_likelihood_helper()
{
	Map<VectorXd> eigen_alpha(m_alpha.vector, m_mu.vlen);
	Map<VectorXd> eigen_mu(m_mu.vector, m_mu.vlen);
	Map<MatrixXd> eigen_K(m_ktrtr.matrix, m_ktrtr.num_rows, m_ktrtr.num_cols);
	//get mean vector and create eigen representation of it
	Map<VectorXd> eigen_mean(m_mean_vec.vector, m_mean_vec.vlen);
	Map<VectorXd> eigen_log_v(m_alpha.vector+m_mu.vlen, m_alpha.vlen-m_mu.vlen);
	Map<VectorXd> eigen_s2(m_s2.vector, m_s2.vlen);
	Map<MatrixXd> eigen_InvK(m_InvK.matrix, m_InvK.num_rows, m_InvK.num_cols);

	CVariationalGaussianLikelihood * lik=get_variational_likelihood();
	float64_t a=SGVector<float64_t>::sum(lik->get_variational_expection());
	float64_t log_det=eigen_log_v.array().sum()-m_log_det_Kernel;
	float64_t trace=(eigen_s2.array()*eigen_InvK.diagonal().array()).sum();

	//nlZ = -a -logdet(V*inv(K))/2 -n/2 +(alpha'*K*alpha)/2 +trace(V*inv(K))/2;
	float64_t result=-a+0.5*(-eigen_K.rows()+eigen_alpha.dot(eigen_mu-eigen_mean)+trace-log_det);

	return result;
}

void CKLDiagonalInferenceMethod::update_alpha()
{
	Map<MatrixXd> eigen_K(m_ktrtr.matrix, m_ktrtr.num_rows, m_ktrtr.num_cols);
	m_InvK=SGMatrix<float64_t>(m_ktrtr.num_rows, m_ktrtr.num_cols);
	Map<MatrixXd> eigen_InvK(m_InvK.matrix, m_InvK.num_rows, m_InvK.num_cols);
	eigen_InvK=solve_inverse(MatrixXd::Identity(m_ktrtr.num_rows,m_ktrtr.num_cols));

	float64_t nlml_new=0;
	float64_t nlml_def=0;

	index_t len=m_labels->get_num_labels();
	index_t total_len=len*2;

	if (m_alpha.vlen == total_len)
	{
		nlml_new=get_negative_log_marginal_likelihood_helper();

		SGVector<float64_t> s2_tmp(m_s2.vlen);
		Map<VectorXd> eigen_s2(s2_tmp.vector, s2_tmp.vlen);
		eigen_s2.fill(1.0);
		CVariationalGaussianLikelihood * lik=get_variational_likelihood();
		lik->set_variational_distribution(m_mean_vec, s2_tmp, m_labels);
		float64_t a=SGVector<float64_t>::sum(lik->get_variational_expection());
		float64_t trace=eigen_InvK.diagonal().array().sum();
		nlml_def=-a+0.5*(-eigen_K.rows()+trace+m_log_det_Kernel);

		if (nlml_new<=nlml_def)
			lik->set_variational_distribution(m_mu, m_s2, m_labels);
	}

	if (m_alpha.vlen != total_len || nlml_def<nlml_new)
	{
		if(m_alpha.vlen != total_len)
			m_alpha = SGVector<float64_t>(total_len);
		m_alpha.zero();

		m_mu=SGVector<float64_t>(len);
		m_s2=SGVector<float64_t>(len);
	}

	nlml_new=optimization();
}

void CKLDiagonalInferenceMethod::update_Sigma()
{
	m_Sigma=SGMatrix<float64_t>(m_mu.vlen, m_mu.vlen);
	Map<MatrixXd> eigen_Sigma(m_Sigma.matrix, m_Sigma.num_rows, m_Sigma.num_cols);
	Map<VectorXd> eigen_s2(m_s2.vector, m_s2.vlen);
	eigen_Sigma=eigen_s2.asDiagonal();
}

void CKLDiagonalInferenceMethod::update_InvK_Sigma()
{
	m_InvK_Sigma=SGMatrix<float64_t>(m_ktrtr.num_rows, m_ktrtr.num_cols);
	Map<MatrixXd> eigen_InvK_Sigma(m_InvK_Sigma.matrix, m_InvK_Sigma.num_rows, m_InvK_Sigma.num_cols);
	Map<MatrixXd> eigen_Sigma(m_Sigma.matrix, m_Sigma.num_rows, m_Sigma.num_cols);
	eigen_InvK_Sigma=solve_inverse(eigen_Sigma);
}

} /* namespace shogun */

