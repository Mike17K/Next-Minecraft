#ifndef NOISEGENERATOR_H_INCLUDED
#define NOISEGENERATOR_H_INCLUDED

struct NoiseParameters {
    int octaves;
    int amplitude;
    int smoothness;
    int heightOffset;

    double roughness;
};

class NoiseGenerator {
public:
    NoiseGenerator(int seed);

    double getHeight(int x, int z, int chunkX, int chunkZ) const noexcept;

    void setParameters(const NoiseParameters& params) noexcept;

    NoiseParameters m_noiseParameters;
private:
    double getNoise(int n) const noexcept;
    double getNoise(double x, double z) const noexcept;

    double lerp(double a, double b, double z) const noexcept;

    double noise(double x, double z) const noexcept;


    int m_seed;
};

#endif // NOISEGENERATOR_H_INCLUDED

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
