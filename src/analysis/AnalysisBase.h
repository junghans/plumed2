/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   Copyright (c) 2012-2014 The plumed team
   (see the PEOPLE file at the root of the distribution for a list of names)

   See http://www.plumed-code.org for more information.

   This file is part of plumed, version 2.

   plumed is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   plumed is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with plumed.  If not, see <http://www.gnu.org/licenses/>.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
#ifndef __PLUMED_analysis_AnalysisBase_h
#define __PLUMED_analysis_AnalysisBase_h

#include "core/ActionPilot.h"
#include "core/ActionAtomistic.h"
#include "core/ActionWithArguments.h"
#include "vesselbase/ActionWithVessel.h"

namespace PLMD {

class ReferenceConfiguration;

namespace analysis {

/**
\ingroup INHERIT
This is the abstract base class to use for implementing new methods for analyzing the trajectory. You can find 
\ref AddingAnAnalysis "information" on how to use it to implement new analysis methods here.

*/

class AnalysisBase :
  public ActionPilot,
  public ActionAtomistic,
  public ActionWithArguments,
  public vesselbase::ActionWithVessel
  {
friend class ReselectLandmarks;
friend class ReadDissimilarityMatrix; 
friend class AnalysisWithDataCollection;
protected:
/// Just run the analysis a single time
  bool use_all_data;
/// The frequency with which we are performing analysis
  unsigned freq;
/// The Analysis action that we are reusing data from
  AnalysisBase* my_input_data;
public:
  static void registerKeywords( Keywords& keys );
  AnalysisBase(const ActionOptions&);
/// These are required because we inherit from both ActionAtomistic and ActionWithArguments
  void lockRequests();
  void unlockRequests();
/// Return the number of data points
  virtual unsigned getNumberOfDataPoints() const ;
/// Return the index of the data point in the base class
  virtual unsigned getDataPointIndexInBase( const unsigned& idata ) const ; 
/// Return the weight of the ith point
  virtual double getWeight( const unsigned& idata ) const ;
/// Get the name of the metric that is being used
  virtual std::string getMetricName() const ;
/// Are we using memory in this calculation this affects the weights of points
  virtual bool usingMemory() const ;
/// Return the normalisation constant for the calculation
  virtual double getNormalization() const ;
/// Ensures that dissimilarities were set somewhere 
  virtual bool dissimilaritiesWereSet() const ;
/// Get the squared dissimilarity between two reference configurations
  virtual double getDissimilarity( const unsigned& i, const unsigned& j );
/// Overwrite getArguments so we get arguments from underlying class
  virtual const std::vector<Value*>    & getArguments() const ;
/// Get the ith data point
  virtual void getDataPoint( const unsigned& idata, std::vector<double>& point, double& weight ) const ;
/// Get a reference configuration (in dimensionality reduction this returns the projection)
  virtual ReferenceConfiguration* getReferenceConfiguration( const unsigned& idata, const bool& calcdist  );
/// This actually performs the analysis
  virtual void performAnalysis()=0;
/// These overwrite things from inherited classes (this is a bit of a fudge)
  bool isPeriodic(){ plumed_error(); return false; }
  unsigned getNumberOfDerivatives(){ plumed_error(); return 0; }
  void calculateNumericalDerivatives( ActionWithValue* a=NULL ){ plumed_error(); }
/// Calculate and apply do nothing all analysis is done during update step
  void calculate(){}
  void apply(){}
/// This will call the analysis to be performed
  virtual void update();
/// This calls the analysis to be performed in the final step of the calculation 
/// i.e. when use_all_data is true
  virtual void runFinalJobs();
/// We would like a cleaner way of doing this if possible
  void confirmStride( const unsigned& istride, const unsigned& all );
};

inline
void AnalysisBase::lockRequests(){
  ActionAtomistic::lockRequests();
  ActionWithArguments::lockRequests();
}

inline
void AnalysisBase::unlockRequests(){
  ActionAtomistic::unlockRequests();
  ActionWithArguments::unlockRequests();
}

inline
unsigned AnalysisBase::getNumberOfDataPoints() const {
  return my_input_data->getNumberOfDataPoints();
}

inline
unsigned AnalysisBase::getDataPointIndexInBase( const unsigned& idata ) const {
  return my_input_data->getDataPointIndexInBase( idata );
}

inline
std::string AnalysisBase::getMetricName() const {
  return my_input_data->getMetricName();
}

inline
double AnalysisBase::getWeight( const unsigned& idata ) const {
  return my_input_data->getWeight( idata );
}

inline
bool AnalysisBase::usingMemory() const {
  return my_input_data->usingMemory();
}

inline
double AnalysisBase::getNormalization() const {
  return my_input_data->getNormalization();
}

inline
bool AnalysisBase::dissimilaritiesWereSet() const {
  return my_input_data->dissimilaritiesWereSet();
}

inline
double AnalysisBase::getDissimilarity( const unsigned& i, const unsigned& j ){
  return my_input_data->getDissimilarity( i, j );
}

inline
const std::vector<Value*> & AnalysisBase::getArguments() const {
  return my_input_data->getArguments();
}

inline
void AnalysisBase::confirmStride( const unsigned& istride, const unsigned& all ){
  freq=istride; use_all_data=all;
}

inline
void AnalysisBase::getDataPoint( const unsigned& idata, std::vector<double>& point, double& weight ) const {
  my_input_data->getDataPoint( idata, point, weight );
}

inline
ReferenceConfiguration* AnalysisBase::getReferenceConfiguration( const unsigned& idata, const bool& calcdist ){
  return my_input_data->getReferenceConfiguration( idata, calcdist );
}

}
}

#endif
