#ifndef INSTANCE_H
#define INSTANCE_H

#include <llvm/IR/Instructions.h>
#include <kernels/kernel.h>
#include <util/slab_allocator.h>
#include <llvm/Support/raw_ostream.h>

namespace kernel {

class Instance {
    friend class KernelBuilder;
    using InputStreamMap = KernelBuilder::InputStreamMap;
    using Allocator = SlabAllocator<Instance>;
public:

    llvm::Value * CreateDoBlockCall();

    llvm::Value * getInternalState(const std::string & name) {
        return mDefinition->getInternalState(mKernelState, name);
    }

    void setInternalState(const std::string & name, llvm::Value * value) {
        mDefinition->setInternalState(mKernelState, name, value);
    }

    llvm::Value * getInternalState(const unsigned index) {
        return getInternalState(iBuilder->getInt32(index));
    }

    llvm::Value * getInternalState(llvm::Value * const index) {
        return mDefinition->getInternalState(mKernelState, index);
    }

    void setInternalState(const unsigned index, llvm::Value * value) {
        setInternalState(iBuilder->getInt32(index), value);
    }

    void setInternalState(llvm::Value * const index, llvm::Value * value) {
        mDefinition->setInternalState(mKernelState, index, value);
    }

    inline llvm::Value * getInputStreamSet(const unsigned streamOffset = 0) {
        return getStreamSet(mDefinition->getInputStreamType(), mInputStreamSet, streamOffset, mInputBufferSize);
    }

    llvm::Value * getInputStream(const unsigned index, const unsigned streamOffset = 0) {
        return getInputStream(iBuilder->getInt32(index), streamOffset);
    }

    llvm::Value * getInputStream(llvm::Value * const index, const unsigned streamOffset = 0) {
        return mDefinition->getInputStream(getInputStreamSet(streamOffset), index);
    }

    llvm::Type * getInputStreamType() const {
        return mDefinition->getInputStreamType();
    }

    llvm::Value * getInputScalar(const unsigned index) {
        return getInputScalar(iBuilder->getInt32(index));
    }

    llvm::Value * getInputScalar(llvm::Value * const index) {
        return mDefinition->getInputScalar(mInputScalarSet, index);
    }

    llvm::Type * getInputScalarType() const {
        return mDefinition->getInputScalarType();
    }

    inline llvm::Value * getOutputStreamSet(const unsigned streamOffset = 0) {
        return getStreamSet(mDefinition->getOutputStreamType(), mOutputStreamSet, streamOffset, mOutputBufferSize);
    }

    llvm::Value * getOutputStream(const unsigned index, const unsigned streamOffset = 0) {
        return getOutputStream(iBuilder->getInt32(index), streamOffset);
    }

    llvm::Value * getOutputStream(llvm::Value * const index, const unsigned streamOffset = 0) {
        return mDefinition->getOutputStream(getOutputStreamSet(streamOffset), index);
    }

    void clearOutputStreamSet();

    llvm::Value * getOutputScalar(const unsigned index) {
        return getOutputScalar(iBuilder->getInt32(index));
    }

    llvm::Value * getOutputScalar(llvm::Value * const index) {
        return mDefinition->getOutputScalar(mOutputScalarSet, index);
    }

    llvm::Value * getBlockNo() {
        return mDefinition->getBlockNo(mKernelState);
    }

    inline std::pair<llvm::Value *, unsigned> getResultSet() const {
        return std::make_pair(mOutputStreamSet, mOutputBufferSize);
    }

    void* operator new (std::size_t size) noexcept {
        return mAllocator.allocate(size);
    }

    void operator delete (void * ptr) {
        mAllocator.deallocate(static_cast<Allocator::value_type *>(ptr));
    }

protected:

    Instance(KernelBuilder * const definition, llvm::Value * const kernelState,
             llvm::Value * const inputScalarSet, llvm::Value * const inputStreamSet, const unsigned inputBufferSize,
             llvm::Value * const outputScalarSet, llvm::Value * const outputStreamSet, const unsigned outputBufferSize)
    : mDefinition(definition)
    , iBuilder(definition->iBuilder)
    , mKernelState(kernelState)
    , mInputScalarSet(inputScalarSet)
    , mInputStreamSet(inputStreamSet)
    , mInputBufferSize(inputBufferSize)
    , mOutputScalarSet(outputScalarSet)
    , mOutputStreamSet(outputStreamSet)
    , mOutputBufferSize(outputBufferSize) {

    }

    llvm::Value * getStreamSet(Type * const type, llvm::Value * const base, const unsigned index, const unsigned bufferSize);

private:
    KernelBuilder * const                           mDefinition;
    IDISA::IDISA_Builder * const                    iBuilder;
    llvm::Value * const                             mKernelState;
    llvm::Value * const                             mInputScalarSet;
    llvm::Value * const                             mInputStreamSet;
    const unsigned                                  mInputBufferSize;
    llvm::Value * const                             mOutputScalarSet;
    llvm::Value * const                             mOutputStreamSet;
    const unsigned                                  mOutputBufferSize;
    static Allocator                                mAllocator;
};

}

#endif // INSTANCE_H
