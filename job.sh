
# NOTICE: Please do not remove the '#' before 'PBS'

# Select which queue (debug, batch) to run on
#PBS -q batch

# Name of your job
#PBS -N HYBRID

# Declaring job as not re-runnable
#PBS -r n

# Resource allocation (how many nodes? how many processes per node?)
#PBS -l nodes=3:ppn=12

# Max execution time of your job (hh:mm:ss)
# Your job may got killed if you exceed this limit
#PBS -l walltime=00:05:00

cd $PBS_O_WORKDIR
NUM_MPI_PROCESS_PER_NODE=1 # EDIT this line to set number of MPI process you want to use per node
export OMP_NUM_THREADS=$(($PBS_NUM_PPN / $NUM_MPI_PROCESS_PER_NODE)) # set max number of threads OpenMP can use per process
export MV2_ENABLE_AFFINITY=0 # prevent MPI from binding all threads to one core

if [ $(($NUM_MPI_PROCESS_PER_NODE * $OMP_NUM_THREADS)) -le $PBS_NUM_PPN ]; then
	time mpiexec -ppn $NUM_MPI_PROCESS_PER_NODE ./ms_hybrid-dynamic $OMP_NUM_THREADS -2 2 -2 2 600 600 disable
else
	echo "$NUM_MPI_PROCESS_PER_NODE * $OMP_NUM_THREADS > $PBS_NUM_PPN; Please check"
fi

