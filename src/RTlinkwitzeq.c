#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ladspa.h>
#include "biquad.h"
#include "linkwitz.h"

#define LINKWITZ_F0 0
#define LINKWITZ_Q0 1
#define LINKWITZ_FP 2
#define LINKWITZ_QP 3
#define LINKWITZ_INPUT 4
#define LINKWITZ_OUTPUT 5

static LADSPA_Descriptor *linkwitzDescriptor = NULL;

typedef struct {
    LADSPA_Data *f0;
    LADSPA_Data *q0;
    LADSPA_Data *fp;
    LADSPA_Data *qp;
    LADSPA_Data *input;
    LADSPA_Data *output;
    biquad *filter;
    float fs;
} Linkwitz;

const LADSPA_Descriptor *ladspa_descriptor(unsigned long index) {
    switch (index) {
    case 0:
        return linkwitzDescriptor;
    default:
        return NULL;
    }
}

static void activateLinkwitz(LADSPA_Handle instance) {
    Linkwitz *plugin_data = (Linkwitz *)instance;
    biquad *filter = plugin_data->filter;
    float fs = plugin_data->fs;
    biquad_init(filter);
    plugin_data->filter = filter;
    plugin_data->fs = fs;
}

static void cleanupLinkwitz(LADSPA_Handle instance) {
    Linkwitz *plugin_data = (Linkwitz *)instance;
    free(plugin_data->filter);
    free(instance);
}

static void connectPortLinkwitz(
    LADSPA_Handle instance,
    unsigned long port,
    LADSPA_Data *data) {
    Linkwitz *plugin = (Linkwitz *)instance;
    switch (port) {
    case LINKWITZ_F0:
        plugin->f0 = data;
        break;
    case LINKWITZ_Q0:
        plugin->q0 = data;
        break;
    case LINKWITZ_FP:
        plugin->fp = data;
        break;
    case LINKWITZ_QP:
        plugin->qp = data;
        break;
    case LINKWITZ_INPUT:
        plugin->input = data;
        break;
    case LINKWITZ_OUTPUT:
        plugin->output = data;
        break;
    }
}

static LADSPA_Handle instantiateLinkwitz(
    const LADSPA_Descriptor *descriptor,
    unsigned long s_rate) {
    Linkwitz *plugin_data = (Linkwitz *)malloc(sizeof(Linkwitz));
    biquad *filter = NULL;
    float fs;

    fs = (float)s_rate;
    filter = malloc(sizeof(biquad));
    biquad_init(filter);

    plugin_data->filter = filter;
    plugin_data->fs = fs;

    return (LADSPA_Handle)plugin_data;
}

#undef buffer_write
#define buffer_write(b, v) (b = v)

static void runLinkwitz(LADSPA_Handle instance, unsigned long sample_count) {
    Linkwitz *plugin_data = (Linkwitz *)instance;

    /* Natural resonant frequency (Hz) */
    const LADSPA_Data f0 = *(plugin_data->f0);

    /* Natural Q factor */
    const LADSPA_Data q0 = *(plugin_data->q0);

    /* Target resonant frequency (Hz) */
    const LADSPA_Data fp = *(plugin_data->fp);

    /* Target Q factor */
    const LADSPA_Data qp = *(plugin_data->qp);

    /* Input (array of floats of length sample_count) */
    const LADSPA_Data * const input = plugin_data->input;

    /* Output (array of floats of length sample_count) */
    LADSPA_Data * const output = plugin_data->output;
    biquad * filter = plugin_data->filter;
    float fs = plugin_data->fs;

    unsigned long pos;
    bq_t a[3], b[3];

    calculate_linkwitz_biquad(a, b, f0, q0, fp, qp, fs);

    /*
     * biquad_run() is written for the difference equation
     *   y[n] = b0 x[n] + b1 x[n-1] + b2 x[n-2]
     *          + a1 y[n-1] + a2 y[n-2]
     * which corresponds to the transfer function
     *   H(z) = (b0 + b1 z^-1 + b2 z^-2) / (1 - a1 z^-1 - a2 z^-2)
     * Note the minus-signs in the denominator.  The coefficients
     * returned by calculate_linkwitz_biquad() are in the more common
     * "DSP textbook" form 1 + a1 z^-1 + a2 z^-2.  Therefore we need
     * to negate a1 and a2 before feeding them to the runtime filter.
     */
    filter->a1 = -a[1];
    filter->a2 = -a[2];
    filter->b0 = b[0];
    filter->b1 = b[1];
    filter->b2 = b[2];

    for (pos = 0; pos < sample_count; pos++) {
        buffer_write(output[pos], (LADSPA_Data) biquad_run(filter, input[pos]));
    }
}

void _init(void);
void _init(void) {
    char **port_names;
    LADSPA_PortDescriptor *port_descriptors;
    LADSPA_PortRangeHint *port_range_hints;

#define D_(s) (s)

    linkwitzDescriptor =
     (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));

    if (linkwitzDescriptor) {
        linkwitzDescriptor->UniqueID = 9003;
        linkwitzDescriptor->Label = "RTlinkwitzeq";
        linkwitzDescriptor->Properties =
         LADSPA_PROPERTY_HARD_RT_CAPABLE;
        linkwitzDescriptor->Name =
         D_("RT Linkwitz Transform Equalizer");
        linkwitzDescriptor->Maker =
         "Stefan Janhunen <sjanhunen@gmail.com>";
        linkwitzDescriptor->Copyright =
         "GPL";
        linkwitzDescriptor->PortCount = 6;

        port_descriptors = (LADSPA_PortDescriptor *)calloc(6,
         sizeof(LADSPA_PortDescriptor));
        linkwitzDescriptor->PortDescriptors =
         (const LADSPA_PortDescriptor *)port_descriptors;

        port_range_hints = (LADSPA_PortRangeHint *)calloc(6,
         sizeof(LADSPA_PortRangeHint));
        linkwitzDescriptor->PortRangeHints =
         (const LADSPA_PortRangeHint *)port_range_hints;

        port_names = (char **)calloc(6, sizeof(char*));
        linkwitzDescriptor->PortNames =
         (const char **)port_names;

        /* Parameters for f0 */
        port_descriptors[LINKWITZ_F0] =
         LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
        port_names[LINKWITZ_F0] =
         D_("f0 - Natural resonant frequency (Hz)");
        port_range_hints[LINKWITZ_F0].HintDescriptor =
         LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_440;
        port_range_hints[LINKWITZ_F0].LowerBound = 1.0f;
        port_range_hints[LINKWITZ_F0].UpperBound = 20000.0f;

        /* Parameters for Q0 */
        port_descriptors[LINKWITZ_Q0] =
         LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
        port_names[LINKWITZ_Q0] =
         D_("Q0 - Natural Q factor");
        port_range_hints[LINKWITZ_Q0].HintDescriptor =
         LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_1;
        port_range_hints[LINKWITZ_Q0].LowerBound = 0.1f;
        port_range_hints[LINKWITZ_Q0].UpperBound = 10.0f;

        /* Parameters for fp */
        port_descriptors[LINKWITZ_FP] =
         LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
        port_names[LINKWITZ_FP] =
         D_("fp - Target resonant frequency (Hz)");
        port_range_hints[LINKWITZ_FP].HintDescriptor =
         LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_440;
        port_range_hints[LINKWITZ_FP].LowerBound = 1.0f;
        port_range_hints[LINKWITZ_FP].UpperBound = 20000.0f;

        /* Parameters for Qp */
        port_descriptors[LINKWITZ_QP] =
         LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
        port_names[LINKWITZ_QP] =
         D_("Qp - Target Q factor");
        port_range_hints[LINKWITZ_QP].HintDescriptor =
         LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_1;
        port_range_hints[LINKWITZ_QP].LowerBound = 0.1f;
        port_range_hints[LINKWITZ_QP].UpperBound = 10.0f;

        /* Parameters for Input */
        port_descriptors[LINKWITZ_INPUT] =
         LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
        port_names[LINKWITZ_INPUT] =
         D_("Input");
        port_range_hints[LINKWITZ_INPUT].HintDescriptor =
         LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
        port_range_hints[LINKWITZ_INPUT].LowerBound = -1.0f;
        port_range_hints[LINKWITZ_INPUT].UpperBound = +1.0f;

        /* Parameters for Output */
        port_descriptors[LINKWITZ_OUTPUT] =
         LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
        port_names[LINKWITZ_OUTPUT] =
         D_("Output");
        port_range_hints[LINKWITZ_OUTPUT].HintDescriptor =
         LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
        port_range_hints[LINKWITZ_OUTPUT].LowerBound = -1.0f;
        port_range_hints[LINKWITZ_OUTPUT].UpperBound = +1.0f;

        linkwitzDescriptor->activate = activateLinkwitz;
        linkwitzDescriptor->cleanup = cleanupLinkwitz;
        linkwitzDescriptor->connect_port = connectPortLinkwitz;
        linkwitzDescriptor->deactivate = NULL;
        linkwitzDescriptor->instantiate = instantiateLinkwitz;
        linkwitzDescriptor->run = runLinkwitz;
        linkwitzDescriptor->run_adding = NULL;
        linkwitzDescriptor->set_run_adding_gain = NULL;
    }
}

void _fini(void);
void _fini(void) {
    if (linkwitzDescriptor) {
        free((LADSPA_PortDescriptor *)linkwitzDescriptor->PortDescriptors);
        free((char **)linkwitzDescriptor->PortNames);
        free((LADSPA_PortRangeHint *)linkwitzDescriptor->PortRangeHints);
        free(linkwitzDescriptor);
    }
}
