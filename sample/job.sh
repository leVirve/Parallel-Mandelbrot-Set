#PBS -q batch
#PBS -N HYBRID
#PBS -r n
#PBS -l nodes=3:ppn=12
#PBS -l walltime=00:05:00

cd $PBS_O_WORKDIR
NUM_MPI_PROCESS_PER_NODE=1 # EDIT this line to set number of MPI process you want to use per node
export OMP_NUM_THREADS=$(($PBS_NUM_PPN / $NUM_MPI_PROCESS_PER_NODE)) # set max number of threads OpenMP can use per process
export MV2_ENABLE_AFFINITY=0 # prevent MPI from binding all threads to one core

if [ $(($NUM_MPI_PROCESS_PER_NODE * $OMP_NUM_THREADS)) -le $PBS_NUM_PPN ]; then
	time mpiexec -ppn $NUM_MPI_PROCESS_PER_NODE -np $(($NUM_MPI_PROCESS_PER_NODE * $PBS_NUM_NODES)) ./MS_Hybrid_dynamic $OMP_NUM_THREADS -2 2 -2 2 600 600 disable
else
	echo "$NUM_MPI_PROCESS_PER_NODE * $OMP_NUM_THREADS > $PBS_NUM_PPN; Please check"
fi
