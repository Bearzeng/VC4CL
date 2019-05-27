/*
 * Author: doe300
 *
 * See the file "LICENSE" for the full license governing this code.
 */

#ifndef TESTKERNEL_H
#define TESTKERNEL_H

#include "src/vc4cl_config.h"

#include "cpptest.h"

class TestKernel : public Test::Suite
{
public:
    TestKernel();
    
    bool setup() override;

    void testCreateKernel();
    void testCreateKernelsInProgram();
    void testSetKernelArg();
    void testGetKernelInfo();
    void testGetKernelArgInfo();
    void testGetKernelWorkGroupInfo();
    void testRetainKernel();
    void testReleaseKernel();
    
    void prepareArgBuffer();
    void testEnqueueNDRangeKernel();
    void testEnqueueTask();
    void testEnqueueNativeKernel();
    void testKernelResult();

    void tear_down() override;
    
private:
    cl_context context;
    cl_program program;
    cl_command_queue queue;
    cl_mem in_buffer;
    cl_mem out_buffer;
    
    cl_kernel kernel;
};

#endif /* TESTKERNEL_H */

