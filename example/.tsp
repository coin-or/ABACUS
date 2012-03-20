#
# This is an example for a problem specific configuration file of
# ABACUS. The concept of the ABACUS configuration files is that 
# easyly settings from the general configuration file can be redefined
# and new problem specific parameters can be added without
# implementing additional functions for file reading.
#

#
# Since following TSPLIB all distances in our problem instances are
# integer, also the objective function value of any feasible solution
# is an integer value. This knowledge can accelerated the solution
# because non integer lower bounds can be rounded up to the next
# integer value. This improved rounding can be turned on by setting
# the parameter ObjInteger to true.
#

ObjInteger                      true

#
# After redefining general ABACUS parameters we add a further
# parameter for the traveling salesman problem. At the end of the
# optimization the best known tour is output if the value of
# ShowBestTour is true.
#
ShowBestTour                    true

#
# USE THE APROXIMATE SOLVER
#
# If ABACUS is compiled with support for the Volume Algorithm
# setting this parameter to true enables solving the LP with the 
# Volume Algorithm whenever the call to TSPSUB::solveApproxNow()
# returns returns true and soving approximately is feasible. 
#
SolveApprox                    false 

#
# THE DEFAULT LP-SOLVER
#
# valid settings: Cbc
#                 Clp
#                 CPLEX
#                 DyLP
#                 FortMP
#                 GLPK
#                 MOSEK
#                 OSL
#                 SoPlex
#                 SYMPHONY
#                 XPRESS_MP 
#
DefaultLpSolver	               CLP

#
# OUTPUT LEVEL OF THE LP SOLVER
#
#   valid settings: 0 - none
#                   1 - minimal
#                   2 - normal low
#                   3 - normal high
#                   4 - verbose
#
SolverOutputLevel               0

#
# NUMBER OF LPS CONSIDERED IN TAILING OFF ANALYSIS
#
#  valid settings: positive integer    => tailing off control
#                  nonpositive integer => no tailing off control 
#
TailOffNLps		        -1
TailOffNLpsVol                   3

# 
# MINIMAL IMPROVEMENT IN TAILING OFF ANALYSIS IN PERCENT
#
#   valid settings: any nonnegative floating point number
#
TailOffPercent		        0.0001
TailOffPercentVol               2.0

#
# PARAMETERS FOR CPLEX
#

#
# PRIMAL PRICING ALGORITHM
#   valid settings: CPX_PPRIIND_AUTO
#                   CPX_PPRIIND_PARTIAL
#                   CPX_PPRIIND_DEVEX
#                   CPX_PPRIIND_STEEP
#                   CPX_PPRIIND_STEEPQSTART
#                   CPX_PPRIIND_FULL
#                   
CplexPrimalPricing              CPX_PPRIIND_AUTO

#
# DUAL PRICING ALGORITHM
#   valid settings: CPX_DPRIIND_AUTO
#                   CPX_DPRIIND_STEEP
#                   CPX_DPRIIND_FULLSTEEP
#                   CPX_DPRIIND_STEEPQSTART
#                   CPX_DPRIIND_FULL
#
CplexDualPricing                CPX_DPRIIND_AUTO

#
# PARAMETERS FOR CLP
#

# PRIMAL COLUMN PIVOT ALGORITHM
#   valid settings: columnsteepest
#                   columndantzig
#
ClpPrimalColumnPivotAlgorithm   columnsteepest

#
# DUAL ROW PIVOT ALGORITHM
#   valid settings: rowsteepest
#                   rowdantzig
#
ClpDualRowPivotAlgorithm        rowsteepest

#
# Parameters for the Volume Algorithm
#
Vol_ascentFirstCheck            1000000
Vol_ascentCheckInterval         100
Vol_printFlag                   0
Vol_printInterval               5000
Vol_greenTestInterval           1
Vol_yellowTestInterval          2
Vol_redTestInterval             10
Vol_alphaInt                    200
Vol_maxSubGradientIterations    2000
Vol_lambdaInit                  0.1
Vol_alphaInit                   0.01
Vol_alphaFactor                 0.5
Vol_alphaMin                    0.000005
Vol_primalAbsPrecision          0.02
Vol_gapAbsPrecision             1.0
Vol_gapRelPrecision             0.01
Vol_granularity                 0.0
Vol_minimumRelAscent            0.0001
