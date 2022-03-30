## Semgrep

This folder contains yaml rules used by Semgrep to tag and find insecure functions / patterns in the code.

Can also be used to apply best practices / correctness.

Check [the docs](https://semgrep.dev/docs/writing-rules/overview/) for more details.


## Usage

Semgrep can be installed and used locally, without sending any information to their backend.

Install with:
```bash
pip install semgrep
```

Then run the rules with:
```bash
semgrep --metrics=off --config=<path_to_yml> path/to/src
```

Example:
```bash
semgrep --metrics=off --config=security/freezer_module.yaml freezer
```

## CI/CD

Semgrep is also run automatically in the CI/CD.