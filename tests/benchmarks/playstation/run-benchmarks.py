import os
import sys
import subprocess
import shutil
from pathlib import Path

NUM_REPITIONS = 2
PCSX_REDUX_PATH = ''
PCSX_REDUX_PATH = 'C:/Users/malte/tools/pcsx-redux/pcsx-redux.exe'
BASE_ARGS = [ '-testmode', '-run' ]
# See https://pcsx-redux.consoledev.net/cli_flags/

BENCHMARKS = [ 
    'entity-system.iterate-no-holes',
    'entity-system.iterate-interleaved-holes',
    'entity-system.iterate-big-holes',
]

script_folder = Path(os.path.realpath(__file__)).parent
build_folder = script_folder.parent.parent.parent / 'build' / 'playstation'
benchmarks_folder = build_folder / 'bin' / 'benchmarks' / 'Release'
log_file_path = build_folder / 'benchmark.log'

benchmark_results_folder_path = build_folder / "benchmark_results"
if benchmark_results_folder_path.exists():
    shutil.rmtree(benchmark_results_folder_path)
benchmark_results_folder_path.mkdir()

for benchmark in BENCHMARKS:
    benchmark_path = benchmarks_folder / (benchmark + '.elf')
    if not Path.exists(benchmark_path): 
        print(f'Benchmark does not exist: "{benchmark_path}"', file=sys.stderr)
        sys.exit(1)

    print(f"Running benchmark: {benchmark_path}")

    headers = None
    values = []
    
    for i in range(NUM_REPITIONS):
        if log_file_path.exists():
            log_file_path.unlink()        

        args = [ PCSX_REDUX_PATH ] + BASE_ARGS.copy() + ['-logfile', log_file_path ] + [ '-exe', benchmark_path ]
        out, err,  = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()

        if not log_file_path.exists():
            print(f'  Error: Log file "{log_file_path}" is missing after running benchmark', file=sys.stderr)
            sys.exit(1)

        log_file = log_file_path.open("r")
        log_lines = log_file.readlines()

        if not len(log_lines) > 1:
            print(f'  Error: Log contains only {len(log_lines)} lines (expected at least 2)', file=sys.stderr)
            sys.exit(1)
        
        print(log_lines[-2].strip())
        this_headers = [x.strip() for x in log_lines[-2].split(',')]

        if headers is None:
            headers = this_headers
        elif not (set(headers) & set(this_headers)):
            print(f"  Error: headers mismatch between runs (was {this_headers} vs {headers})")
            sys.exit(1)
        
        print(log_lines[-1].strip())
        this_values_raw = [x.strip() for x in log_lines[-1].split(',')]
        if len(this_values_raw) != len(this_headers):
            print(f"  Error: Mismatch between num headers ({len(this_headers)}) and num values ({len(this_values)})", file=sys.stderr)
            sys.exit(1)

        values.append(this_values_raw)

        log_file.close()
    
    results_file_path = benchmark_results_folder_path / (benchmark + "_results.csv")
    results_file_path.touch(exist_ok=False)
    results_file = results_file_path.open("w")

    results_file.write(', '.join(headers) + "\n")
    
    for i in range(NUM_REPITIONS):
        results_file.write(', '.join(values[i]) + "\n")