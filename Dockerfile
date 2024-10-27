FROM ubuntu:latest

RUN apt-get update
RUN apt-get install -y build-essential -y cmake
RUN apt-get install -y ladspa-sdk ecasound sox libfftw3-dev
RUN apt-get install -y r-base

# Install R packages
RUN R -e "install.packages('tuneR', repos='http://cran.rstudio.com/')"
RUN R -e "install.packages('sound', repos='http://cran.rstudio.com/')"
RUN R -e "install.packages('fftw', repos='http://cran.rstudio.com/')"