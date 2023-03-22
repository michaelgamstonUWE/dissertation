#include <stdint.h>

namespace tflite
{
    template <unsigned int tOpCount>
    class MicroMutableOpResolver;
    class ErrorReporter;
    class Model;
    class MicroInterpreter;
} // namespace tflite

struct TfLiteTensor;

class CNN 
{
private:
    tflite::MicroMutableOpResolver<12> *resolver;
    const tflite::Model *model;
    tflite::MicroInterpreter *interpreter;
    TfLiteTensor *input;
    TfLiteTensor *output;
    uint8_t *tensor_arena;

public:
    int8_t *getInputBuffer();
    CNN();
    int8_t predict();
    int8_t convert_float2int8(float x);
    float convert_int82float(int8_t x);
};
