/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Heiko Strathmann, Sergey Lisitsyn
 */

/*%warnfilter(302) apply;
%warnfilter(302) apply_generic;*/

%newobject shogun::CPipelineBuilder::then(const std::string&);
%newobject shogun::CPipelineBuilder::then(const std::string&, CMachine*);
%newobject shogun::CPipelineBuilder::build();

%rename(Machine) CMachine;
%rename(PipelineBuilder) CPipelineBuilder;
%rename(Pipeline) CPipeline;

%newobject apply();
%newobject apply(CFeatures* data);
%newobject apply_binary();
%newobject apply_binary(CFeatures* data);
%newobject apply_regression();
%newobject apply_regression(CFeatures* data);
%newobject apply_multiclass();
%newobject apply_multiclass(CFeatures* data);
%newobject apply_structured();
%newobject apply_structured(CFeatures* data);
%newobject apply_latent();
%newobject apply_latent(CFeatures* data);

#if defined(SWIGPYTHON) || defined(SWIGOCTAVE) || defined(SWIGRUBY) || defined(SWIGLUA) || defined(SWIGR)

%define APPLY_MULTICLASS(CLASS)
    %extend CLASS
    {
        CMulticlassLabels* apply(CFeatures* data=NULL)
        {
            return $self->apply_multiclass(data);
        }
    }
%enddef

%define APPLY_BINARY(CLASS)
    %extend CLASS
    {
        CBinaryLabels* apply(CFeatures* data=NULL)
        {
            return $self->apply_binary(data);
        }
    }
%enddef

%define APPLY_REGRESSION(CLASS)
    %extend CLASS
    {
        CRegressionLabels* apply(CFeatures* data=NULL)
        {
            return $self->apply_regression(data);
        }
    }
%enddef

%define APPLY_STRUCTURED(CLASS)
    %extend CLASS
    {
        CStructuredLabels* apply(CFeatures* data=NULL)
        {
            return $self->apply_structured(data);
        }
    }
%enddef

%define APPLY_LATENT(CLASS)
    %extend CLASS
    {
        CLatentLabels* apply(CFeatures* data=NULL)
        {
            return $self->apply_latent(data);
        }
    }
%enddef

namespace shogun {
APPLY_MULTICLASS(CMulticlassMachine);
APPLY_MULTICLASS(CKernelMulticlassMachine);
APPLY_MULTICLASS(CLinearMulticlassMachine);
APPLY_MULTICLASS(CDistanceMachine);

APPLY_BINARY(CLinearMachine);
APPLY_BINARY(CKernelMachine);
#ifdef USE_GPL_SHOGUN
APPLY_BINARY(CWDSVMOcas);
#endif //USE_GPL_SHOGUN
APPLY_BINARY(CPluginEstimate);
#ifdef USE_GPL_SHOGUN
APPLY_BINARY(CGaussianProcessClassification);
#endif //USE_GPL_SHOGUN

APPLY_REGRESSION(CMKLRegression);
#ifdef HAVE_LAPACK
APPLY_REGRESSION(CLeastSquaresRegression);
APPLY_REGRESSION(CLeastAngleRegression);
#endif
#ifdef USE_GPL_SHOGUN
APPLY_REGRESSION(CGaussianProcessRegression);
#endif //USE_GPL_SHOGUN

APPLY_STRUCTURED(CStructuredOutputMachine);
APPLY_STRUCTURED(CLinearStructuredOutputMachine);
APPLY_STRUCTURED(CKernelStructuredOutputMachine);
#ifdef USE_MOSEK
APPLY_STRUCTURED(CPrimalMosekSOSVM);
#endif
#ifdef USE_GPL_SHOGUN
APPLY_STRUCTURED(CDualLibQPBMSOSVM);
APPLY_LATENT(CLatentSVM);
#endif //USE_GPL_SHOGUN
}

%rename(apply_generic) CMachine::apply(CFeatures* data=NULL);
%rename(apply_generic) CMulticlassMachine::apply(CFeatures* data=NULL);
%rename(apply_generic) CKernelMulticlassMachine::apply(CFeatures* data=NULL);
%rename(apply_generic) CLinearMulticlassMachine::apply(CFeatures* data=NULL);
%rename(apply_generic) CCDistanceMachineMachine::apply(CFeatures* data=NULL);
%rename(apply_generic) CLinearMachine::apply(CFeatures* data=NULL);
%rename(apply_generic) CKernelMachine::apply(CFeatures* data=NULL);
#ifdef USE_GPL_SHOGUN
%rename(apply_generic) CWDSVMOcas::apply(CFeatures* data=NULL);
#endif //USE_GPL_SHOGUN
%rename(apply_generic) CPluginEstimate::apply(CFeatures* data=NULL);
%rename(apply_generic) CMKLRegression::apply(CFeatures* data=NULL);
#ifdef HAVE_LAPACK
%rename(apply_generic) CLeastSquaresRegression::apply(CFeatures* data=NULL);
%rename(apply_generic) CLeastAngleRegression::apply(CFeatures* data=NULL);
#endif
%rename(apply_generic) CGaussianProcessRegression::apply(CFeatures* data=NULL);

%rename(apply_generic) CStructuredOutputMachine::apply(CFeatures* data=NULL);
%rename(apply_generic) CLinearStructuredOutputMachine::apply(CFeatures* data=NULL);
%rename(apply_generic) CKernelStructuredOutputMachine::apply(CFeatures* data=NULL);
#ifdef USE_MOSEK
%rename(apply_generic) CPrimalMosekSOSVM::apply(CFeatures* data=NULL);
#endif

#undef APPLY_MULTICLASS
#undef APPLY_BINARY
#undef APPLY_REGRESSION
#undef APPLY_STRUCTURED
#undef APPLY_LATENT

#endif

%include <shogun/machine/Machine.h>

/** Instantiate RandomMixin */
%template(RandomMixinMachine) shogun::RandomMixin<shogun::CMachine, std::mt19937_64>;

%include <shogun/machine/Pipeline.h>
