# HyperLogLog

### What's the purpose?

This repository contains a simple implementation of the HyperLogLog algorithm, which is good for reading code, but not for production.

### How to use?

Run `./estimator` with wanted precision.
```
./estimator 14
```

Estimator reads strings from `STDIN` separated by '\n', and when it reads `EOF` it prints the estimated number of distict strings.

Here is an example:
```console
user@laptop:~$ seq 0 100000 | ./estimator 10
103751
```

### How to build?

```console
make
```

### System requirements

gcc, make
