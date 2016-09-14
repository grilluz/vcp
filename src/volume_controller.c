/*
 * to compile needs -lasound -lm
 */

#include "volume_controller.h"

#include <alsa/asoundlib.h>
#include <math.h>

void volume_controller_open(struct mixer_controller *c) {
	if(c == NULL || c->handle == NULL || c->sid == NULL) {
		return;
	}

	snd_mixer_open(&(c->handle), 0);
	snd_mixer_attach(c->handle, c->card);
	snd_mixer_selem_register(c->handle, NULL, NULL);
	snd_mixer_load(c->handle);

	snd_mixer_selem_id_alloca(&(c->sid));
	snd_mixer_selem_id_set_index(c->sid, 0);
	snd_mixer_selem_id_set_name(c->sid, c->selem_name);
	c->elem = snd_mixer_find_selem(c->handle, c->sid);
}

void volume_controller_close(struct mixer_controller *c) {
	
	if(c == NULL || c->handle == NULL || c->sid == NULL) {
		return;
	}

	snd_mixer_selem_id_free(c->sid);	
	snd_mixer_detach(c->handle, c->card);
	snd_mixer_close(c->handle); //also free allocated resources
}

void set_volume(snd_mixer_elem_t *elem, const long volume) {
	long min, max;

	if(elem == NULL) {
		return;
	}

	snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

	if(volume < 0 || volume > 100) {
		return;
	}

	snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);
}

void get_volume(snd_mixer_elem_t *elem, long *volume) {
	long min, max, vol;

	if(elem == NULL || volume == NULL) {
		return;
	}

	snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
	snd_mixer_selem_get_playback_volume(elem, 0, &vol);

	*volume = lround((double)(vol * 100) / max);
}
