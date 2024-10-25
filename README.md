# rt-plugins: LADSPA plugins for Active Loudspeakers

This is a fork of the [original rt-plugins code](https://faculty.tru.ca/rtaylor/rt-plugins/index.html) developed by Richard Taylor.

Development Goals:
- Add support for container-based development and GitHub Actions
- Add support for additional useful plugins for active loudspeakers (e.g. Linkwitz Transform)
- Continue to make code and test quality improvements
- Improve documentation and examples

# Compiling

Install with

    cd build
    cmake ..
    make
    sudo make install

By default this installs plugins in `/usr/local/lib/ladspa`.  If
you want them to go somewhere else, configure cmake accordingly
by running `ccmake .`

You should set the environment variable `LADSPA_PATH` to contain the
path to the plugins, so they can be found.  Something like this:

    export LADSPA_PATH=/usr/local/lib/ladspa:/usr/lib/ladspa

You probably want to put this in your ~/.bashrc or some such.


# Platform-specific issues

The code sets flags in the floating point processor, so that denormal floating
point numbers don't cause a performance hit, should they occur.  This is
platform-specific, and requires compiling with `-msse2 -mfpmath=sse`.  So far as
I know, these instruction sets are only implemented on Intel and Athlon
processors.  Compiling for other architectures will probably require some
changes to the source code.

You can set `USE_SSE2` to `off` using `ccmake` if you do not want to use SSE2.
