#include "audio.hpp"
#include "common.hpp"

namespace may
{
    audio_device::audio_device() : audio_device("")
    {
    }

    audio_device::audio_device(const std::string &__name)
    {
        _name = __name;
        _spec.callback = nullptr;
        _spec.channels = 0;
        _spec.format = AUDIO_U8;
        _spec.freq = 0;
        _spec.samples = 0;
    }

    void audio_device::play()
    {
        SDL_PauseAudioDevice(_id, 0);
    }

    void audio_device::pause(bool __paused)
    {
        SDL_PauseAudioDevice(_id, __paused);
    }

    bool audio_device::open(SDL_AudioFormat __format, int __freq, int __channels, int __framebuf_size, SDL_AudioCallback __cb, void *userdata)
    {
        SDL_AudioSpec requested;
        requested.format = __format;
        requested.freq = __freq;
        requested.channels = __channels;
        requested.samples = __framebuf_size;
        requested.callback = __cb;
        requested.userdata = userdata;

        const char *name = nullptr;
        if (!_name.empty())
        {
            name = _name.c_str();
        }

        SDL_AudioSpec actual;
        _id = SDL_OpenAudioDevice(name, 0, &requested, &actual, SDL_AUDIO_ALLOW_SAMPLES_CHANGE);
        if (!_id)
        {
            fprintf(stderr, "unable to open audio device '%s': %s\n", _name.c_str(), SDL_GetError());
            return false;
        }

        _spec = actual;

        SDL_GetNumAudioDevices(0); // required for SDL_GetAudioDeviceName to work

        name = SDL_GetAudioDeviceName(_id, 0);
        if (!name)
        {
            fprintf(stderr, "unable to get device %d name: %s\n", _id, SDL_GetError());
        }
        else
        {
            _name = std::string(name);
        }

        return true;
    }

    void audio_device::close()
    {
        SDL_CloseAudioDevice(_id);
    }

    bool audio_device::paused() const
    {
        SDL_AudioStatus status = SDL_GetAudioDeviceStatus(_id);
        return status != SDL_AUDIO_PLAYING;
    }

    int audio_device::queued_size() const
    {
        return SDL_GetQueuedAudioSize(_id);
    }

    void audio_device::queue(void *data)
    {
    }

    double wave_noop(double time, double freq)
    {
        return 0.0;
    }

    double wave_sine(double time, double freq)
    {
        return SDL_sin(time * freq * 2 * M_PI);
    }

    double wave_sawtooth(double time, double freq)
    {
        return SDL_fmod(time * freq, 1.0) * 2.0 - 1.0;
    }

    double wave_square(double time, double freq)
    {
        return wave_sine(time, freq) > 0.0 ? 1.0 : -1.0;
    }

    double wave_triangle(double time, double freq)
    {
        return SDL_fabs(-wave_sawtooth(time, freq)) * 2.0 - 1.0;
    }

    base_oscillator::base_oscillator(double __frequency)
    {
        this->_time = 0.0f;
        this->_frequency = __frequency;
        this->_wavefunc = wave_noop;
    }

    double base_oscillator::value() const
    {
        return may::clamp(-1.0, _wavefunc(_time, _frequency), 1.0);
    }

    double base_oscillator::next(double delta_time)
    {
        _time += delta_time;
        return value();
    }

    double base_oscillator::frequency() const
    {
        return _frequency;
    }

    void base_oscillator::frequency(double __frequency)
    {
        if (__frequency == 0.0)
            __frequency = 1e-50;

        _time -= _time * (__frequency - _frequency) / __frequency; // THIS PHASE SHIFT REMOVES THE POPPING WHEN CHANGING FREQUENCIES
        _frequency = __frequency;
    }

    base_oscillator base_oscillator::clone(double __frequency) const
    {
        if (__frequency == 0.0)
            __frequency = 1e-50;

        base_oscillator ret(*this);
        ret._frequency = __frequency;
        return ret;
    }

    sine_osc::sine_osc(double __frequency) : base_oscillator(__frequency)
    {
        _wavefunc = wave_sine;
    }

    sawtooth_osc::sawtooth_osc(double __frequency) : base_oscillator(__frequency)
    {
        _wavefunc = wave_sawtooth;
    }

    triangle_osc::triangle_osc(double __frequency) : base_oscillator(__frequency)
    {
        _wavefunc = wave_triangle;
    }

    square_osc::square_osc(double __frequency) : base_oscillator(__frequency)
    {
        _wavefunc = wave_square;
    }

    composcillator::composcillator()
    {
    }

    composcillator::composcillator(const base_oscillator &first)
    {
        _oscillators.push_back(first);
    }

    composcillator composcillator::operator+(const base_oscillator &osc) const
    {
        composcillator ret;
        ret._oscillators = _oscillators;
        ret._oscillators.push_back(osc);
        return ret;
    }

    composcillator &composcillator::operator+=(const base_oscillator &osc)
    {
        _oscillators.push_back(osc);
        return *this;
    }

    double composcillator::value() const
    {
        double val = 0;

        for (auto &o : _oscillators)
        {
            val += o.value();
        }

        return val / _oscillators.size();
    }

    double composcillator::next(double delta_time)
    {
        for (auto &o : _oscillators)
        {
            o.next(delta_time);
        }

        return value();
    }

    void composcillator::frequency(double __frequency)
    {
        for (auto &o : _oscillators)
        {
            o.frequency(__frequency);
        }
    }
}