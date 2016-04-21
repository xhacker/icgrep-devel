/*
 *  Copyright (c) 2016 International Characters.
 *  This software is licensed to the public under the Open Software License 3.0.
 */

#include <llvm/Support/CommandLine.h>

#include <IDISA/idisa_avx_builder.h>
#include <IDISA/idisa_sse_builder.h>
#include <IDISA/idisa_i64_builder.h>

// Dynamic processor detection
#define ISPC_LLVM_VERSION ISPC_LLVM_3_8
#include <util/ispc.cpp>

static cl::OptionCategory dCodeGenOptions("Code Generation Options", "These options control code generation.");

static cl::opt<int> BlockSize("BlockSize", cl::init(0), cl::desc("specify a block size (defaults to widest SIMD register width in bits)."), cl::cat(dCodeGenOptions));
static cl::opt<bool> ForceAVX512("ForceAVX512", cl::init(0), cl::desc("force icgrep to use the AVX512 builder."), cl::cat(dCodeGenOptions));


IDISA::IDISA_Builder * GetIDISA_Builder(Module * mod) {
    bool hasAVX2 = (strncmp(lGetSystemISA(), "avx2", 4) == 0);
    bool hasAVX512 = (strncmp(lGetSystemISA(), "avx512", 6) == 0) || ForceAVX512;
    
    unsigned theBlockSize = BlockSize;  // from command line
    
    if (theBlockSize == 0) {  // No BlockSize override: use processor SIMD width
        theBlockSize = hasAVX512 ? 512 : (hasAVX2 ? 256 : 128);
    }
    Type * bitBlockType = VectorType::get(IntegerType::get(getGlobalContext(), 64), theBlockSize/64);
    
    int blockSize = bitBlockType->isIntegerTy() ? cast<IntegerType>(bitBlockType)->getIntegerBitWidth() : cast<VectorType>(bitBlockType)->getBitWidth();
    if (blockSize >= 256) {
        if (hasAVX512) {
            return new IDISA::IDISA_AVX512_Builder(mod, bitBlockType);
        }
        else if (hasAVX2) {
            return new IDISA::IDISA_AVX2_Builder(mod, bitBlockType);
        }
        else {
            return new IDISA::IDISA_SSE2_Builder(mod, bitBlockType);
        }
    }
    else if (blockSize == 64) {
        return new IDISA::IDISA_I64_Builder(mod, bitBlockType);
    }
    return new IDISA::IDISA_SSE2_Builder(mod, bitBlockType);
}

