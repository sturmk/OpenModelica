
#pragma once

#include "FactoryExport.h"

#include <Core/Solver/SolverDefaultImplementation.h>
#include <idas/idas.h>
#include <nvector/nvector_serial.h>
#include <sundials/sundials_direct.h>
#include <idas/idas_dense.h>


#ifdef RUNTIME_PROFILING
#include <Core/Utils/extension/measure_time.hpp>
#endif

#ifdef USE_BOOST_LOG
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/basic_logger.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/exception_handler_feature.hpp>
#include <boost/log/sources/features.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/utility/exception_handler.hpp>
#include <boost/log/sources/channel_logger.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/common.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;


enum idaseverity_level
{
  ida_info,
  ida_normal,
  ida_notification,
  ida_warning,
  ida_error,
  ida_critical
};

typedef src::severity_channel_logger_mt<
    idaseverity_level,     // the type of the severity level
    std::string         // the type of the channel name
> my_ida_logger_mt;


BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT(ida_lg, my_ida_logger_mt)
{
    // Specify the channel name on construction, similarly as with the channel_logger
    return my_ida_logger_mt(keywords::channel = "solver.IDA");
}
#endif


/*BOOST_LOG_INLINE_GLOBAL_LOGGER_CTOR_ARGS(
  ida_lg,
  src::channel_logger_mt< >,
  (keywords::severity = ida_normal)(keywords::channel = "solver.IDA"))
*/
/*****************************************************************************/
// IDA aus dem SUNDIALS-Package
// BDF-Verfahren für steife und nicht-steife ODEs
// Dokumentation siehe offizielle IDA Doku

/*****************************************************************************
Copyright (c) 2004, Bosch Rexroth AG, All rights reserved
*****************************************************************************/
class Ida
  : public ISolver,  public SolverDefaultImplementation
{
public:

  Ida(IMixedSystem* system, ISolverSettings* settings);

  virtual ~Ida();

  // geerbt von Object (in SolverDefaultImplementation)
  //---------------------------------------
  /// Spezielle Solvereinstellungen setzten (default oder user defined)
  virtual void initialize();


  // geerbt von ISolver
  //---------------------------------------
  /// Setzen der Startzeit für die numerische Lösung
  virtual void setStartTime(const double& time)
  {
    SolverDefaultImplementation::setStartTime(time);
  };

  /// Setzen der Endzeit für die numerische Lösung
  virtual void setEndTime(const double& time)
  {
    SolverDefaultImplementation::setEndTime(time);
  };

  /// Setzen der initialen Schrittweite (z.B. auch nach Nullstelle)
  virtual void setInitStepSize(const double& stepSize)
  {
    SolverDefaultImplementation::setInitStepSize(stepSize);
  };

  /// Berechung der numerischen Lösung innerhalb eines gegebenen Zeitintervalls
  virtual void solve(const SOLVERCALL command = UNDEF_CALL);

  /// Liefert den Status des Solvers nach Beendigung der Simulation
  virtual ISolver::SOLVERSTATUS getSolverStatus()
  {
    return (SolverDefaultImplementation::getSolverStatus());
  };

  //// Ausgabe von statistischen Informationen (wird vom SimManager nach Abschluß der Simulation aufgerufen)
  virtual void writeSimulationInfo();
   virtual void setTimeOut(unsigned int time_out);
    virtual void stop();

  virtual int reportErrorMessage(std::ostream& messageStream);
  virtual bool stateSelection();
private:

  // Solveraufruf
  void IDACore();

  /// Kapselung der Berechnung der rechten Seite
  int calcFunction(const double& time, const double* y, double* yd);

  // Callback für die rechte Seite
  static int CV_fCallback(double t, N_Vector y, N_Vector ydot, N_Vector, void *user_data);

  // Checks error flags of SUNDIALS
  int check_flag(void *flagvalue, const char *funcname, int opt);

  // Nulltellenfunktion
  void giveZeroVal(const double &t,const double *y,double *zeroValue);
  void writeIDAOutput(const double &time,const double &h,const int &stp);
  bool isInterrupted();
  // Callback der Nullstellenfunktion
  static int CV_ZerofCallback(double t, N_Vector y, N_Vector yp, double *zeroval, void *user_data);

  // Functions for Coloured Jacobian
  static int CV_JCallback(long int N, realtype t, N_Vector y, N_Vector fy, DlsMat Jac,void *user_data, N_Vector tmp1, N_Vector tmp2, N_Vector tmp3);
  int calcJacobian(double t, long int N, N_Vector fHelp, N_Vector errorWeight, N_Vector jthcol, double* y, N_Vector fy, DlsMat Jac);
  void initializeColoredJac();

  void setcycletime(double cycletime);


  ISolverSettings
    *_idasettings;              ///< Input      - Solver settings

  void
    *_idaMem,                  ///< Temp      - Memory for the solver
    *_data;                    ///< Temp      - User data. Contains pointer to ida

  long int
    _dimSys,                  ///< Input       - (total) Dimension of system (=number of ODE)
    _idid,                    ///< Input, Output  - Status Flag
    _locStps,                  ///< Output      - Number of Steps between two events
     _cv_rt;            ///< Temp    - ida return flag

  int
    _outStps,                  ///< Output      - Total number of output-steps
    *_zeroSign;


  double
    *_z,            ///< Output      - (Current) State vector
    *_zInit,          ///< Temp      - Initial state vector
    *_zWrite,                   ///< Temp      - Zustand den das System rausschreibt
    *_absTol,          ///         - Vektor für absolute Toleranzen
  *_delta,
  *_ysave;


  double
    _hOut;            ///< Temp      - Ouput step size for dense output

   unsigned int
    _event_n;
double
  _tLastEvent;

  double
    _tOut,            ///< Output      - Time for dense output
    _tZero,            ///< Temp      - Nullstelle
    _tLastWrite;        ///< Temp      - Letzter Ausgabezeitpunkt

  bool
    _bWritten,                  ///< Temp      - Is output already written
    _zeroFound;




  N_Vector
    _CV_y0,                  ///< Temp      - Initial values in the ida Format
    _CV_y,                  ///< Temp      - State in ida Format
  _CV_yp,    ///<Temp   - Stateders in ida Format
    _CV_yWrite,        ///< Temp      - Vector for dense out
    _CV_ypWrite,
    _CV_absTol;

  // Variables for Coloured Jacobians
  int  _sizeof_sparsePattern_colorCols;
  int* _sparsePattern_colorCols;

  int  _sizeof_sparsePattern_leadindex;
  int* _sparsePattern_leadindex;


  int  _sizeof_sparsePattern_index;
  int* _sparsePattern_index;


  int  _sparsePattern_maxColors;

  bool _ida_initialized;


   ISystemProperties* _properties;
   IContinuous* _continuous_system;
   IEvent* _event_system;
   IMixedSystem* _mixed_system;
   ITime* _time_system;

   #ifdef RUNTIME_PROFILING
   std::vector<MeasureTimeData> measureTimeFunctionsArray;
   MeasureTimeValues *measuredFunctionStartValues, *measuredFunctionEndValues, *solveFunctionStartValues, *solveFunctionEndValues;
   MeasureTimeValuesSolver *solverValues;
   #endif
};

