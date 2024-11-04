#include <stdlib.h>
#include <string.h>
#include <ladspa.h>

#define LINKWITZ_F0 0
#define LINKWITZ_Q0 1
#define LINKWITZ_FP 2
#define LINKWITZ_QP 3

static LADSPA_Descriptor *linkwitzDescriptor = NULL;

typedef struct {
    LADSPA_Data *f0;
    LADSPA_Data *q0;
    LADSPA_Data *fp;
    LADSPA_Data *qp;
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
    // Activation code here
}

static void cleanupLinkwitz(LADSPA_Handle instance) {
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
    }
}

static LADSPA_Handle instantiateLinkwitz(
    const LADSPA_Descriptor *descriptor,
    unsigned long s_rate) {
    Linkwitz *plugin_data = (Linkwitz *)malloc(sizeof(Linkwitz));
    return (LADSPA_Handle)plugin_data;
}

static void runLinkwitz(LADSPA_Handle instance, unsigned long sample_count) {
    // Signal processing code here
}

void _init(void);
void _init(void) {
    char **port_names;
    LADSPA_PortDescriptor *port_descriptors;
    LADSPA_PortRangeHint *port_range_hints;

    linkwitzDescriptor = (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));

    if (linkwitzDescriptor) {
        linkwitzDescriptor->UniqueID = 9003;
        linkwitzDescriptor->Label = "RTlinkwitzeq";
        linkwitzDescriptor->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
        linkwitzDescriptor->Name = "RT Linkwitz Equalizer";
        linkwitzDescriptor->Maker = "Stefan Janhunen <sjanhunen@gmail.com>";
        linkwitzDescriptor->Copyright = "GPL";
        linkwitzDescriptor->PortCount = 4;

        port_descriptors = (LADSPA_PortDescriptor *)calloc(4, sizeof(LADSPA_PortDescriptor));
        linkwitzDescriptor->PortDescriptors = (const LADSPA_PortDescriptor *)port_descriptors;

        port_range_hints = (LADSPA_PortRangeHint *)calloc(4, sizeof(LADSPA_PortRangeHint));
        linkwitzDescriptor->PortRangeHints = (const LADSPA_PortRangeHint *)port_range_hints;

        port_names = (char **)calloc(4, sizeof(char*));
        linkwitzDescriptor->PortNames = (const char **)port_names;

        // Parameters for f0
        port_descriptors[LINKWITZ_F0] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
        port_names[LINKWITZ_F0] = "f0 - Natural resonant frequency of the driver in a sealed box";
        port_range_hints[LINKWITZ_F0].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
        port_range_hints[LINKWITZ_F0].LowerBound = 0.0F;
        port_range_hints[LINKWITZ_F0].UpperBound = 20000.0F;

        // Parameters for Q0
        port_descriptors[LINKWITZ_Q0] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
        port_names[LINKWITZ_Q0] = "Q0 - Natural Q factor of the driver in a sealed box";
        port_range_hints[LINKWITZ_Q0].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
        port_range_hints[LINKWITZ_Q0].LowerBound = 0.1F;
        port_range_hints[LINKWITZ_Q0].UpperBound = 10.0F;

        // Parameters for fp
        port_descriptors[LINKWITZ_FP] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
        port_names[LINKWITZ_FP] = "fp - Equalized resonant frequency of the driver in a sealed box";
        port_range_hints[LINKWITZ_FP].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
        port_range_hints[LINKWITZ_FP].LowerBound = 0.0F;
        port_range_hints[LINKWITZ_FP].UpperBound = 20000.0F;

        // Parameters for Qp
        port_descriptors[LINKWITZ_QP] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
        port_names[LINKWITZ_QP] = "Qp - Equalized Q factor of the driver in a sealed box";
        port_range_hints[LINKWITZ_QP].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
        port_range_hints[LINKWITZ_QP].LowerBound = 0.1F;
        port_range_hints[LINKWITZ_QP].UpperBound = 10.0F;

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
