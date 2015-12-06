job = """
#PBS -q batch
#PBS -N %(name)s
#PBS -r n
#PBS -l %(ppn)s
#PBS -l walltime=00:05:00

cd $PBS_O_WORKDIR
NUM_MPI_PROCESS_PER_NODE=1
export OMP_NUM_THREADS=$(($PBS_NUM_PPN / $NUM_MPI_PROCESS_PER_NODE))
export MV2_ENABLE_AFFINITY=0

if [ $(($NUM_MPI_PROCESS_PER_NODE * $OMP_NUM_THREADS)) -le $PBS_NUM_PPN ]; then
    time mpiexec -ppn $NUM_MPI_PROCESS_PER_NODE ./%(exec)s $OMP_NUM_THREADS -2 2 -2 2 600 600 disable
else
    echo "$NUM_MPI_PROCESS_PER_NODE * $OMP_NUM_THREADS > $PBS_NUM_PPN; Please check"
fi
"""

combinations = [
    (
        'nodes=%d:ppn=%d' % (s[0], s[1]),
        'ms_%s' % s[2],
        '%s-%d*%d' % (s[2], s[0], s[1])
    )
    for s in [
        (3, 12, 'mpi-dynamic'),
        (1, 12, 'mpi-dynamic'),
    ]
]

for c in combinations:
    print(job % {'ppn': c[0], 'exec': c[1], 'name': c[2]})
