#include "CNN.h"
#include "cnn_model.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"

const int kArenaSize = 400000;

CNN::CNN()
{

    model = tflite::GetModel(quantised_cnn);
    // This pulls in the operators implementations we need
    resolver = new tflite::MicroMutableOpResolver<12>();
    resolver->AddFullyConnected();
    resolver->AddMul();
    resolver->AddAdd();
    resolver->AddLogistic();
    resolver->AddReshape();
    resolver->AddQuantize();
    resolver->AddDequantize();
    resolver->AddConv2D();
    resolver->AddAveragePool2D();
    resolver->AddMaxPool2D();

    tensor_arena = (uint8_t *)malloc(kArenaSize);
    //if (!tensor_arena)
    //{
    //    TF_LITE_REPORT_ERROR(error_reporter, "Could not allocate arena");
    //    return;
    //}

    // Build an interpreter to run the model with.
    interpreter = new tflite::MicroInterpreter(
        model, *resolver, tensor_arena, kArenaSize);

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
        MicroPrintf("AllocateTensors() failed");
        return;
    }

    size_t used_bytes = interpreter->arena_used_bytes();
    // TF_LITE_REPORT_ERROR(error_reporter, "Used bytes %d\n", used_bytes);
    
    // Obtain pointers to the model's input and output tensors.
    input = interpreter->input(0);
    output = interpreter->output(0);
}

int8_t *CNN::getInputBuffer()
{
    return input->data.int8;
}

int8_t CNN::predict()
{
    interpreter->Invoke();
    return output->data.int8[0];
}

int8_t CNN::convert_float2int8(float x)
{
   int8_t x_quantized = x / input->params.scale + input->params.zero_point;
   return x_quantized;
}

float CNN::convert_int82float(int8_t x)
{
    float y = (x - output->params.zero_point) * output->params.scale;
    return y;
}
