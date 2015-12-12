#! /usr/bin/python
"""
Run this script with below command! (y)
$> nohup ./src/jobs.py > jobs_py.stdout 2> jobs_py.stderr < /dev/null &
"""
import re
import time
import subprocess

job_format = """
#PBS -q batch
#PBS -N %(name)s
#PBS -r n
#PBS -e errors/%(name)s
#PBS -o output/%(name)s
#PBS -l %(ppn)s
#PBS -l walltime=00:03:00

cd $PBS_O_WORKDIR
NUM_MPI_PROCESS_PER_NODE=%(mpi_process_per_node)d
export OMP_NUM_THREADS=$(($PBS_NUM_PPN / $NUM_MPI_PROCESS_PER_NODE))
export MV2_ENABLE_AFFINITY=0

time mpiexec -ppn $NUM_MPI_PROCESS_PER_NODE -np $(($NUM_MPI_PROCESS_PER_NODE * $PBS_NUM_NODES)) ./%(exec)s $OMP_NUM_THREADS -2 2 -2 2 %(w)d %(w)d disable
"""


class Testcase:
    ppn = (1, 12)
    mpi_process_per_node = 1
    width = 1000

    def __init__(self, exe, *initial_data, **kwargs):
        self.exe = exe
        for key, val in kwargs.items():
            setattr(self, key, val)
        self.name = '%s-%dx%d' % (self.exe, self.ppn[0], self.ppn[1])

    def content(self):
        return {
            'ppn': 'nodes=%d:ppn=%d' % (self.ppn[0], self.ppn[1]),
            'exec': self.exe,
            'mpi_process_per_node': self.mpi_process_per_node,
            'name': self.name,
            'w': self.width
        }


"""
------------------------------------------------------------------------------------------------------------------
Configurations
------------------------------------------------------------------------------------------------------------------
"""

exes = {
    'openmp': ['ms_openmp-static', 'ms_openmp-dynamic'],
    'hybrid': ['ms_hybrid-static', 'ms_hybrid-dynamic'],
    'mpi': ['ms_mpi-static', 'ms_mpi-dynamic'],
}

ranges = [40, 80, 160, 320, 640, 1280, 2560, 5120]

testcases = list()
testcases += [Testcase(exe='ms_openmp-static', ppn=(1, i), width=1000) for i in range(1, 13)]
testcases += [Testcase(exe=exe, width=5120) for exe in exes['openmp']]
testcases += [Testcase(exe='ms_mpi-static', mpi_process_per_node=12, width=5120) for exe in exes['mpi']]
testcases += [Testcase(exe='ms_hybrid-dynamic', ppn=(1, 12), mpi_process_per_node=1, width=1280)]
testcases += [Testcase(exe='ms_hybrid-dynamic', ppn=(2, 6) , mpi_process_per_node=2, width=1280)]
testcases += [Testcase(exe='ms_hybrid-dynamic', ppn=(3, 4) , mpi_process_per_node=3, width=1280)]
testcases += [Testcase(exe='ms_hybrid-dynamic', ppn=(4, 3) , mpi_process_per_node=4, width=1280)]

"""
Above are configurations -----------------------------------------------------------------------------------------
"""


def qsub(job_content):
    p = subprocess.Popen(
        ['qsub'],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    p.stdin.write(job_content)
    return p.communicate()


print('Total jobs: %d' % len(testcases))
for i, case in enumerate(testcases):
    while True:
        out, err = qsub(job_format % case.content())
        if re.match('\d+\.\w.', out):
            print("Testcase#%d %s" % (i, out))
            break
        time.sleep(10)
