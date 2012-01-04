#include "Bias.h"
#include "ActionRegister.h"

#include <cassert>

using namespace std;


namespace PLMD{

//+PLUMEDOC BIAS RESTRAINT
/**
Adds an harmonic and/or linear restraint on one or more variables

\par Syntax
\verbatim
RESTRAINT ARG=x1,x2,... KAPPA=k1,k2,... SLOPE=m1,m2,... AT=a1,a2,...
\endverbatim
KAPPA and SLOPE specifies an array of force constants, one for each variable,
and AT the center of the restraints. Thus, the resulting potential is
\f$
  \sum_i \frac{k_i}{2} (x_i-a_i)^2 + m_i*(x_i-a_i)
\f$.

\par Example
The following input is restraining the distance between atoms 3 and 5
and the distance between atoms 2 and 4, at different equilibrium
values, and it is printing the energy of the restraint
\verbatim
DISTANCE ATOMS=3,5 LABEL=d1
DISTANCE ATOMS=2,4 LABEL=d2
RESTRAINT ARG=d1,d2 AT=1.0,1.5 KAPPA=150.0,150.0 LABEL=restraint
PRINT ARG=restraint.bias
\endverbatim
(See also \ref DISTANCE and \ref PRINT).

*/
//+ENDPLUMEDOC

class BiasRestraint : public Bias{
  std::vector<double> at;
  std::vector<double> kappa;
  std::vector<double> slope;
public:
  BiasRestraint(const ActionOptions&);
  void calculate();
};

PLUMED_REGISTER_ACTION(BiasRestraint,"RESTRAINT")

BiasRestraint::BiasRestraint(const ActionOptions&ao):
PLUMED_BIAS_INIT(ao),
at(0),
kappa(getNumberOfArguments(),0.0)
slope(getNumberOfArguments(),0.0)
{
  parseVector("SLOPE",slope);
  assert(slope.size()==getNumberOfArguments());
  parseVector("KAPPA",kappa);
  assert(kappa.size()==getNumberOfArguments());
  parseVector("AT",at);
  assert(at.size()==getNumberOfArguments());
  checkRead();

  log.printf("  at");
  for(unsigned i=0;i<at.size();i++) log.printf(" %f",at[i]);
  log.printf("\n");
  log.printf("  with harmonic force constant");
  for(unsigned i=0;i<kappa.size();i++) log.printf(" %f",kappa[i]);
  log.printf("\n");
  log.printf("  and linear force constant");
  for(unsigned i=0;i<slope.size();i++) log.printf(" %f",slope[i]);
  log.printf("\n");

  addValue("bias");
  addValue("force2");
}


void BiasRestraint::calculate(){
  double ene=0.0;
  double totf2=0.0;
  for(unsigned i=0;i<getNumberOfArguments();++i){
    const double cv=difference(i,at[i],getArgument(i));
    const double k=kappa[i];
    const double m=slope[i];
    const double f=-(k*cv+m);
    ene+=0.5*k*cv*cv+m*cv;
    setOutputForces(i,f);
    totf2+=f*f;
  };
  Value* value;
  value=getValue("bias"); setValue(value,ene);
  value=getValue("force2");  setValue(value,totf2);
}

}


