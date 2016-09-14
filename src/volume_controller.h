#ifndef VOLUME_CONTROLLER_H
#define VOLUME_CONTROLLER_H

#include <alsa/asoundlib.h>

#define VOLUME_LEN 7

/*
 * volume range 0~100
 */
void volume_controller(long *volume, void (*callback)(snd_mixer_elem_t*, long*));

void set_volume(snd_mixer_elem_t *elem, long *volume);

void get_volume(snd_mixer_elem_t *elem, long *volume);

#endif
