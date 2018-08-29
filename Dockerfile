FROM debian:stretch

# The executable.
COPY build/ray /ray

# Data files.
COPY data /data

# Program to run.
CMD ["/ray"]

