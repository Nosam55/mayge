#ifndef AUDIO_HPP_
#define AUDIO_HPP_
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <string>
#include <vector>

namespace may
{
    class base_oscillator
    {
    protected:
        typedef double (*wavefunc_t)(double, double);

        double _time;
        double _frequency;
        wavefunc_t _wavefunc;

    public:
        base_oscillator(double __frequency);
        double value() const;
        double next(double delta_time);
        double frequency() const;
        void frequency(double __frequency);

        base_oscillator clone(double __frequency) const;
    };

    class composcillator
    {
        std::vector<base_oscillator> _oscillators;

    public:
        composcillator();
        composcillator(const base_oscillator &first);

        composcillator operator+(const base_oscillator &osc) const;
        composcillator &operator+=(const base_oscillator &osc);

        double value() const;
        double next(double delta_time);
        void frequency(double __frequency);
    };

    class sine_osc : public base_oscillator
    {
    public:
        sine_osc(double __frequency);
    };

    class sawtooth_osc : public base_oscillator
    {
    public:
        sawtooth_osc(double __frequency);
    };

    class triangle_osc : public base_oscillator
    {
    public:
        triangle_osc(double __frequency);
    };

    class square_osc : public base_oscillator
    {
    public:
        square_osc(double __frequency);
    };

    class audio_device
    {
    private:
        std::string _name;
        SDL_AudioDeviceID _id;

        SDL_AudioSpec _spec;

    public:
        audio_device();
        audio_device(const std::string &__name);

        void play();
        void pause(bool __paused = true);

        bool open(SDL_AudioFormat __format, int __freq, int __channels = 1, int __framebuf_size = 2048, SDL_AudioCallback __cb = nullptr, void *userdata = nullptr);
        void close();

        inline const std::string &name() const { return _name; }
        inline SDL_AudioDeviceID id() const { return _id; }
        inline SDL_AudioFormat format() const { return _spec.format; }
        inline int freq() const { return _spec.freq; }
        inline int channels() const { return _spec.channels; }
        inline int framebuffer_size() const { return _spec.samples; }

        bool paused() const;
        int queued_size() const;
        void queue(void *data);
    };
}

#endif /* AUDIO_HPP_ */