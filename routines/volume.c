#include <alsa/asoundlib.h>

#include "routine.h"
#include "util.h"

struct volume_t {
	double level;
	int mute;
};

static snd_mixer_selem_id_t *selem_id;
static snd_mixer_t *mixer;
static snd_mixer_elem_t *elem;
static long vol_min, vol_max, vol_now;

static int get_volume(const char *device, const char *mixer_name, const int mixer_index, struct volume_t *volume) {
	if (snd_mixer_open(&mixer, 0) < 0)
		return -1;
	if (snd_mixer_attach(mixer, device) < 0)
		goto close_mixer;
	if (snd_mixer_load(mixer) < 0)
		goto close_mixer;

	if (snd_mixer_selem_id_malloc(&selem_id) < 0)
		goto close_mixer;

	snd_mixer_selem_id_set_name(selem_id, mixer_name);
	snd_mixer_selem_id_set_index(selem_id, mixer_index);

	if (snd_mixer_selem_register(mixer, NULL, NULL) < 0)
		goto free_selem_id;
	if ((elem = snd_mixer_find_selem(mixer, selem_id)) == NULL)
		goto free_selem_id;
	if (snd_mixer_selem_get_playback_volume(elem, 0, &vol_now) < 0)
		goto free_selem_id;
	if (snd_mixer_selem_get_playback_volume_range(elem, &vol_min, &vol_max) < 0)
		goto free_selem_id;
	if (snd_mixer_selem_get_playback_switch(elem, 0, &volume->mute) < 0)
		goto free_selem_id;

	volume->level = 100 * (vol_now - vol_min) / (vol_max - vol_min);
	snd_mixer_selem_id_free(selem_id);
	snd_mixer_close(mixer);
	return 0;

free_selem_id:
	snd_mixer_selem_id_free(selem_id);
close_mixer:
	snd_mixer_close(mixer);
	return -2;
}


void volume_routine(cfg_t *config, struct text_t *text) {
	struct volume_t volume;

	if (get_volume(cfg_getstr(config, "device"), cfg_getstr(config, "mixer"), cfg_getint(config, "index"), &volume) < 0)
		die("could not get volume info\n");

	const char *format;

	CHOOSE_FMTCOL(
			volume.mute ? "format" : "format_muted",
			volume.mute ? "color_normal" : "color_degraded");

	FORMAT_WALK(format) {
		FORMAT_PRE_RESOLVE;
		FORMAT_RESOLVE("volume", 6, "%.0f", volume.level);
		FORMAT_POST_RESOLVE;
	}
}
