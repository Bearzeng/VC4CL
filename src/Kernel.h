/*
 * Author: doe300
 *
 * See the file "LICENSE" for the full license governing this code.
 */

#ifndef VC4CL_KERNEL
#define VC4CL_KERNEL

#include "Bitfield.h"
#include "Event.h"
#include "Program.h"

#include <bitset>
#include <vector>

namespace vc4cl
{
    struct DevicePointer;

    struct KernelArgument
    {
        struct ScalarValue : private Bitfield<uint32_t>
        {
            BITFIELD_ENTRY(Float, float, 0, Int)
            BITFIELD_ENTRY(Unsigned, uint32_t, 0, Int)
            BITFIELD_ENTRY(Signed, int32_t, 0, Int)
        };
        std::vector<ScalarValue> scalarValues;
        /*
         * This specifies the buffer-size to allocate, e.g. for __local pointers or direct struct parameters.
         *
         * NOTE: __local parameters are not passed a buffer, but the buffer-size to automatically allocate and
         * deallocate again after the kernel-execution.
         */
        unsigned sizeToAllocate;

        void addScalar(float f);
        void addScalar(uint32_t u);
        void addScalar(int32_t s);
        void addScalar(DevicePointer ptr);
        void setDirectData(const void* data, std::size_t numBytes);

        std::string to_string() const;

        inline bool isLocalParameter() const
        {
            return sizeToAllocate > 0 && scalarValues.empty();
        }

        /*
         * Passing non-trivial (e.g. struct) parameters directly to a kernel function generates pointers with
         * byval attribute set in LLVM. From the kernel side, they are treated as any other pointer parameter,
         * but on host side, they are set by directly passing the data, similar to direct vector parameters.
         *
         * We handle them by creating a buffer (similar to local memory), copying the data into this buffer and
         * passing the pointer to the kernel.
         */
        inline bool isByValueParameter() const
        {
            return sizeToAllocate > 0 && !scalarValues.empty();
        }
    };

    class Kernel final : public Object<_cl_kernel, CL_INVALID_KERNEL>
    {
    public:
        Kernel(Program* program, const KernelInfo& info);
        ~Kernel() override;

        CHECK_RETURN cl_int setArg(cl_uint arg_index, size_t arg_size, const void* arg_value);
        CHECK_RETURN cl_int getInfo(
            cl_kernel_info param_name, size_t param_value_size, void* param_value, size_t* param_value_size_ret);
        CHECK_RETURN cl_int getWorkGroupInfo(cl_kernel_work_group_info param_name, size_t param_value_size,
            void* param_value, size_t* param_value_size_ret);
        CHECK_RETURN cl_int getArgInfo(cl_uint arg_index, cl_kernel_arg_info param_name, size_t param_value_size,
            void* param_value, size_t* param_value_size_ret);
        CHECK_RETURN cl_int enqueueNDRange(CommandQueue* commandQueue, cl_uint work_dim,
            const size_t* global_work_offset, const size_t* global_work_size, const size_t* local_work_size,
            cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event);

        object_wrapper<Program> program;
        KernelInfo info;

        std::vector<KernelArgument> args;
        std::bitset<kernel_config::MAX_PARAMETER_COUNT> argsSetMask;
    };

    struct KernelExecution final : public EventAction
    {
        object_wrapper<Kernel> kernel;
        cl_uchar numDimensions;
        std::array<std::size_t, kernel_config::NUM_DIMENSIONS> globalOffsets;
        std::array<std::size_t, kernel_config::NUM_DIMENSIONS> globalSizes;
        std::array<std::size_t, kernel_config::NUM_DIMENSIONS> localSizes;

        explicit KernelExecution(Kernel* kernel);
        ~KernelExecution() override = default;

        cl_int operator()() override final;
    };

} /* namespace vc4cl */

#endif /* VC4CL_KERNEL */
