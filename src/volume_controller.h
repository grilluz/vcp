#ifndef VOLUME_CONTROLLER_H
#define VOLUME_CONTROLLER_H

#include <alsa/asoundlib.h>

#define VOLUME_LEN 7

/*
 * volume range 0~100
 */

struct mixer_controller {
	snd_mixer_elem_t *elem;
	snd_mixer_t *handle;
	snd_mixer_selem_id_t *sid;
	char *card;
	char *selem_name;
};

void set_volume(snd_mixer_elem_t *elem, const long volume);

void get_volume(snd_mixer_elem_t *elem, long *volume);

void volume_controller_open(struct mixer_controller *controller);

void volume_controller_close(struct mixer_controller *controller);

#endif
