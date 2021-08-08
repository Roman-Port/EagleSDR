#include <volk/volk.h>
#include "RaptorRadioSource.h"
#include "RaptorRadioDemodulator.h"
#include "RaptorFirFilterComplex.h"
#include "RaptorFirFilterReal.h"
#include "RaptorArbitraryResampler.h"
#include "RaptorWavFileWriter.h"
#include "RaptorDecimatorComplex.h"
#include "RaptorSpectrumController.h"
#include "RaptorRadio.h"
#include "RaptorRtlSdrSource.h"
#include "RaptorConstants.h"
#include "RaptorWbFmDemodulator.h"
#include "RaptorFirFilterComplex.h"
#include "RaptorFirFilterReal.h"
#include "RaptorLowPassFilterBuilder.h"
#include "RaptorFileSource.h"

namespace RaptorRadio
{
	//Constant, not changed once initialized
	lv_32fc_t* bufferIq;
	float* bufferAudioL;
	float* bufferAudioR;
	RaptorRadioSource* sources[RAPTOR_SOURCE_COUNT];
	RaptorRadioDemodulator* demodulators[RAPTOR_DEMODULATOR_COUNT];

	//Shared
	float bandwidth;
	float vfoSampleRate;
	bool settingsStale;
	RaptorRadioDemodulator* pendingDemodulator;

	//Spectrums
	RaptorSpectrumController spectrumMain;
	RaptorSpectrumController spectrumMpx;

	//Worker thread only
	RaptorRadioSource* currentSource;
	RaptorRadioDemodulator* currentDemodulator;
	RaptorDecimatorComplex vfoDecimator;
	RaptorArbitraryResampler vfoResamplerL;
	RaptorArbitraryResampler vfoResamplerR;

	//Debug outputs
#ifdef RAPTOR_DEBUG_OUTPUT_AUDIO
	RaptorWavFileWriter* testAudio;
#endif
	
	bool initialize(char** errorMsg)
	{
		//Create buffers
		size_t alignment = volk_get_alignment();
		bufferIq = (lv_32fc_t*)volk_malloc(RAPTOR_BUFFER_SIZE * sizeof(lv_32fc_t), alignment);
		bufferAudioL = (float*)volk_malloc(RAPTOR_BUFFER_SIZE * sizeof(float), alignment);
		bufferAudioR = (float*)volk_malloc(RAPTOR_BUFFER_SIZE * sizeof(float), alignment);

		//Set default values
		bandwidth = 250000;
		pendingDemodulator = 0;
		vfoSampleRate = 0;
		settingsStale = true;

		//Initialize main spectrum
		spectrumMain.setup(8192, RAPTOR_SPECTRUM_WIDTH, 30, false);
		spectrumMain.range = 80;
		spectrumMain.offset = 30;

		//Initialize MPX spectrum
		spectrumMpx.setup(4096, RAPTOR_SPECTRUM_WIDTH, 30, true);
		spectrumMpx.range = 70;
		spectrumMpx.offset = 20;

		//Create sources
		sources[0] = new RaptorRtlSdrSource();
		sources[1] = new RaptorFileSource("/home/pi/test_files/fm_stereo_test_onechannel.wav");

		//Create demodulators
		demodulators[0] = new RaptorWbFmDemodulator();

		//Open testing outputs
#ifdef RAPTOR_DEBUG_OUTPUT_AUDIO
		testAudio = new RaptorWavFileWriter("test_out.wav", RAPTOR_OUTPUT_RATE);
#endif
		
		//Loop through sources and find the first one we can open
		currentSource = 0;
		for (int i = 0; i < RAPTOR_SOURCE_COUNT; i++) {
			if (sources[i]->open()) {
				currentSource = sources[i];
				break;
			}
		}
		if (currentSource == 0) {
			*errorMsg = "Failed to open sources.";
			return false;
		}

		return true;
	}

	void configure() {
		//Validate
		assert(pendingDemodulator != 0);
		assert(currentSource != 0);

		//Log
		raptor_debug_log("Beginning radio reconfiguration; Source rate: %i\n", currentSource->sample_rate);
		raptor_debug_log("BW: %f\n", bandwidth);
		raptor_debug_log("Demodulator: %s\n", pendingDemodulator->get_label());

		//Apply pending demodulator
		currentDemodulator = pendingDemodulator;

		//Configure main spectrum
		spectrumMain.set_sample_rate((int)currentSource->sample_rate);

		//Reconfigure VFO filter
		vfoDecimator.set_label("VFO Decimator");
		vfoSampleRate = vfoDecimator.configure(currentSource->sample_rate, bandwidth / 2);

		//Apply to the demodulator
		float audioSampleRate = currentDemodulator->configure(vfoSampleRate);
		raptor_debug_log("Demodulated audio rate: %f\n", audioSampleRate);

		//Update the audio resamplers
		vfoResamplerL.configure(audioSampleRate, RAPTOR_OUTPUT_RATE);
		vfoResamplerR.configure(audioSampleRate, RAPTOR_OUTPUT_RATE);

		//Update
		settingsStale = false;
	}

	void audio_output(int count) {
		//Write to debug file
#ifdef RAPTOR_DEBUG_OUTPUT_AUDIO
		testAudio->write(bufferAudioL, bufferAudioR, count);
#endif
	}

	void run() {
		int read;
		while (true) {
			//If the settings are stale, reconfigure
			if (settingsStale)
				configure();

			//Read samples from source
			read = currentSource->read(bufferIq);

			//Send to FFT
			spectrumMain.add_samples(bufferIq, read);

			//Decimate for VFO
			read = vfoDecimator.process(bufferIq, bufferIq, read);

			//Demodulate
			read = currentDemodulator->demodulate(bufferIq, read, bufferAudioL, bufferAudioR);

			//Input resampled audio
			vfoResamplerL.input(bufferAudioL, read);
			vfoResamplerR.input(bufferAudioR, read);

			//Output resampled audio
			do {
				//Output resampled audio
				read = vfoResamplerL.output(bufferAudioL, RAPTOR_BUFFER_SIZE);
				read = vfoResamplerR.output(bufferAudioR, RAPTOR_BUFFER_SIZE);

				//Write output
				audio_output(read);
			} while (read != 0);
		}
	}

	// PUBLIC

	void set_frequency(unsigned int freq) {
		currentSource->set_center_freq(freq);
	}

	void set_bandwidth(unsigned int newBandwidth) {
		bandwidth = newBandwidth;
		settingsStale = true;
	}

	void set_demodulator(int index) {
		assert(index >= 0 && index < RAPTOR_DEMODULATOR_COUNT);
		pendingDemodulator = demodulators[index];
		settingsStale = true;
	}

	char** get_demodulator_labels() {
		//Allocate
		char** labels = (char**)malloc(sizeof(char*) * RAPTOR_DEMODULATOR_COUNT);

		//Apply
		for (int i = 0; i < RAPTOR_DEMODULATOR_COUNT; i++)
			labels[i] = demodulators[i]->get_label();

		return labels;
	}

	RaptorSpectrumController* get_spectrum_main() {
		return &spectrumMain;
	}

	RaptorSpectrumController* get_spectrum_mpx() {
		return &spectrumMpx;
	}
	
}