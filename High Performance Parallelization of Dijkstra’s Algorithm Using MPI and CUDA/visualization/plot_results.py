import os
import re
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import logging

logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

def parse_log(filename):
    results = []
    with open(filename, 'r') as f:
        for line in f:
            m = re.match(r"Dijkstra \((\w+)\) completed in ([\d\.eE+-]+) seconds", line)
            if m:
                impl, time = m.group(1), float(m.group(2))
                results.append({'impl': impl, 'time': time})
    return results

def gather_results(log_dir):
    all_results = []
    for fname in os.listdir(log_dir):
        if fname.endswith('.log'):
            try:
                res = parse_log(os.path.join(log_dir, fname))
                all_results.extend(res)
            except Exception as e:
                logging.warning(f"Failed to parse {fname}: {e}")
    return pd.DataFrame(all_results)

def plot_performance(df, outdir):
    plt.figure(figsize=(8,6))
    for impl in df['impl'].unique():
        sub = df[df['impl'] == impl]
        plt.plot(sub['nodes'], sub['time'], marker='o', label=impl)
    plt.xlabel('Number of Nodes')
    plt.ylabel('Time (s)')
    plt.title('Performance (Linear Scale)')
    plt.legend()
    plt.savefig(os.path.join(outdir, 'performance_linear.png'))
    plt.yscale('log')
    plt.title('Performance (Log Scale)')
    plt.savefig(os.path.join(outdir, 'performance_log.png'))
    plt.close()

def plot_scaling(df, outdir):
    if 'procs' not in df.columns:
        return
    base = df[df['procs'] == 1]
    for nodes in base['nodes'].unique():
        base_time = base[base['nodes'] == nodes]['time'].values[0]
        scaling = df[df['nodes'] == nodes].copy()
        scaling['efficiency'] = base_time / (scaling['procs'] * scaling['time']) * 100
        plt.plot(scaling['procs'], scaling['efficiency'], marker='o', label=f'{nodes} nodes')
    plt.xlabel('Number of Processes')
    plt.ylabel('Scaling Efficiency (%)')
    plt.title('Strong Scaling Efficiency')
    plt.legend()
    plt.savefig(os.path.join(outdir, 'scaling_efficiency.png'))
    plt.close()

def main():
    log_dir = '../results'  # Directory where logs are stored
    outdir = '.'
    if not os.path.exists(log_dir):
        logging.error('Results directory not found!')
        return
    df = gather_results(log_dir)
    if df.empty:
        logging.error('No results found!')
        return
    # Assume nodes and procs columns are present in the logs for full analysis
    plot_performance(df, outdir)
    plot_scaling(df, outdir)
    logging.info('Plots saved in visualization directory.')

if __name__ == '__main__':
    main() 