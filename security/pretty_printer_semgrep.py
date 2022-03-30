from sys import argv
from colorama import Fore, Style
import json
from tabulate import tabulate

"""
Requirements:
`pip install colorama tabulate tabulate[widechars]`
"""

tabulate.WIDE_CHARS_MODE = True # fix alignment for emojis

def colorize(s: str, color) -> str:
    return color + s + Style.RESET_ALL

def colorize_severity(severity: str) -> str:
    color_map = {'ERROR': Fore.RED, 'WARNING': Fore.YELLOW, 'INFO': Fore.CYAN}
    return colorize(severity, color_map[severity])

def get_status_emoji(number_of_findings: int) -> str:
    if number_of_findings == 0:
        return "\U00002705"
    elif number_of_findings > 0 and number_of_findings < 3:
        return "\U000026A0"
    else:
        return "\U0000274C"

def pretty_print_overview(data: dict):
    print(f"Number of files scanned: {data['len_files_scanned']} \n")
    headers =['severity', 'number of findings', 'status']
    body = [[colorize_severity(k), v, get_status_emoji(v)] for k,v in sorted(data.items()) if k != "len_files_scanned"]
    print(tabulate(body, headers=headers, tablefmt="fancy_grid"))


def parse_semgrep_output(data: dict) -> dict:
    files_scanned = data['paths']
    len_files_scanned = len(files_scanned['scanned'])


    results = data['results']
    summary_results = {'ERROR': 0, 'INFO': 0, 'WARNING': 0}
    if results:
        for res in results:
            severity = res['extra']['severity']
            summary_results[severity] += 1

    return {
        'len_files_scanned': len_files_scanned,
        **summary_results
    }

def print_usage():
    print("This script can be used to pretty print a summary of the results given by semgrep")
    print("python3 pretty_printer_semgrep.py res.json")

def main():
    json_to_pp = None
    try:
        with open(argv[1], 'r') as f:
            json_txt = f.read()
            if json_txt.find("Semgrep Core — Fatal error") != -1:
                print("Fatal Error detected in the output, check your semgrep patterns.\n Exiting...\n")
                exit(-1)

        json_to_pp = json.loads(json_txt)
    except FileNotFoundError as e:
        print(e)
        print_usage()
        exit(-1)

    d = parse_semgrep_output(json_to_pp)
    pretty_print_overview(d)
    exit(0)

if __name__ == "__main__":
    main()