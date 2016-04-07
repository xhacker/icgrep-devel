/*
 *  Copyright (c) 2015 International Characters.
 *  This software is licensed to the public under the Open Software License 3.0.
 *  icgrep is a trademark of International Characters.
 */

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/CodeGen/CommandFlags.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>

#include <re/re_cc.h>
#include <cc/cc_compiler.h>
#include <pablo/function.h>
#include <IDISA/idisa_builder.h>
#include <IDISA/idisa_target.h>
#include <kernels/u8u16_pipeline.h>

// Dynamic processor detection
#define ISPC_LLVM_VERSION ISPC_LLVM_3_6
#include <util/ispc.cpp>

#include <utf_encoding.h>

// mmap system
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
using namespace boost::iostreams;
using namespace boost::filesystem;

#include <fcntl.h>

static cl::list<std::string> inputFiles(cl::Positional, cl::desc("<input file ...>"), cl::OneOrMore);


static cl::OptionCategory cMachineCodeOptimization("Machine Code Optimizations", "These options control back-end compilier optimization levels.");

static cl::opt<char> OptLevel("O", cl::desc("Optimization level. [-O0, -O1, -O2, or -O3] (default = '-O0')"),
                              cl::cat(cMachineCodeOptimization), cl::Prefix, cl::ZeroOrMore, cl::init('0'));


//
//  Functions taken from toolchain.cpp and modified for casefold 
//  JIT_t_ExecutionEngine : remove object cache
//  icgrep_Linking:   unneeded?
//  all others: definitely unneeded
//

ExecutionEngine * JIT_to_ExecutionEngine (Module * m) {

    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();

    PassRegistry * Registry = PassRegistry::getPassRegistry();
    initializeCore(*Registry);
    initializeCodeGen(*Registry);
    initializeLowerIntrinsicsPass(*Registry);

    std::string errMessage;
    EngineBuilder builder(std::move(std::unique_ptr<Module>(m)));
    builder.setErrorStr(&errMessage);
    builder.setMCPU(sys::getHostCPUName());
    CodeGenOpt::Level optLevel = CodeGenOpt::Level::None;
    switch (OptLevel) {
        case '0': optLevel = CodeGenOpt::None; break;
        case '1': optLevel = CodeGenOpt::Less; break;
        case '2': optLevel = CodeGenOpt::Default; break;
        case '3': optLevel = CodeGenOpt::Aggressive; break;
        default: errs() << OptLevel << " is an invalid optimization level.\n";
    }
    builder.setOptLevel(optLevel);

    if ((strncmp(lGetSystemISA(), "avx2", 4) == 0)) {
            std::vector<std::string> attrs;
            attrs.push_back("avx2");
            builder.setMAttrs(attrs);
    }

    // builder.selectTarget();

    //builder.setOptLevel(mMaxWhileDepth ? CodeGenOpt::Level::Less : CodeGenOpt::Level::None);
    ExecutionEngine * engine = builder.create();
    if (engine == nullptr) {
        throw std::runtime_error("Could not create ExecutionEngine: " + errMessage);
    }
    return engine;
}


//
//
//
namespace pablo {

PabloFunction * u8u16_pablo(const Encoding encoding) {
    //  input: 8 basis bit streams
    //  output: 16 u8-indexed streams, + delmask stream + error stream
    PabloFunction * function = PabloFunction::Create("u8u16", 8, 18);
    cc::CC_Compiler ccc(*function, encoding);
    
    PabloBuilder pBuilder(ccc.getBuilder().getPabloBlock(), ccc.getBuilder());
    const std::vector<Var *> u8_bits = ccc.getBasisBits();
    // Outputs
    Assign * u16_hi[8];
    Assign * u16_lo[8];
    Assign * delmask;
    Assign * error_mask;
    
    // The logic for processing non-ASCII bytes is to be embedded within an if-hierarchy.
    PabloAST * nonASCII = ccc.compileCC(re::makeCC(0x80, 0xFF));
    
    // Builder for the if statement handling all non-ASCII logic
    PabloBuilder nAb = PabloBuilder::Create(pBuilder);
    // Bits 3 through 7 of a 2-byte prefix are data bits, needed to
    // produce the UTF-16 code unit data ..., 
    PabloAST * bit3a1 = nAb.createAdvance(u8_bits[3], 1);
    PabloAST * bit4a1 = nAb.createAdvance(u8_bits[4], 1);
    PabloAST * bit5a1 = nAb.createAdvance(u8_bits[5], 1);
    PabloAST * bit6a1 = nAb.createAdvance(u8_bits[6], 1);
    PabloAST * bit7a1 = nAb.createAdvance(u8_bits[7], 1);
    
    // Entry condition for 3 or 4 byte sequences: we have a prefix byte in the range 0xE0-0xFF.
    PabloAST * pfx34 = ccc.compileCC(re::makeCC(0xE0, 0xFF), nAb);
    // Builder for the if statement handling all logic for 3- and 4-byte sequences.
    PabloBuilder p34b = PabloBuilder::Create(nAb);
    // Bits 4 through 7 of a 3-byte prefix are data bits.  They must be moved
    // to the final position of the 3-byte sequence.
    PabloAST * bit2a1 = p34b.createAdvance(u8_bits[2], 1);
    PabloAST * bit4a2 = p34b.createAdvance(bit4a1, 1);
    PabloAST * bit5a2 = p34b.createAdvance(bit5a1, 1);
    PabloAST * bit6a2 = p34b.createAdvance(bit6a1, 1);
    PabloAST * bit7a2 = p34b.createAdvance(bit7a1, 1);
    //
    // Logic for 4-byte UTF-8 sequences
    //
    // Entry condition  or 4 byte sequences: we have a prefix byte in the range 0xF0-0xFF.
    PabloAST * pfx4 = ccc.compileCC(re::makeCC(0xF0, 0xFF), p34b);
    // Builder for the if statement handling all logic for 4-byte sequences only.
    PabloBuilder p4b = PabloBuilder::Create(p34b);
    // Illegal 4-byte sequences
    PabloAST * F0 = ccc.compileCC(re::makeCC(0xF0), p4b);
    PabloAST * F4 = ccc.compileCC(re::makeCC(0xF4), p4b);
    PabloAST * F0_err = p4b.createAnd(p4b.createAdvance(F0, 1), ccc.compileCC(re::makeCC(0x80, 0x8F), p4b));
    PabloAST * F4_err = p4b.createAnd(p4b.createAdvance(F4, 1), ccc.compileCC(re::makeCC(0x90, 0xBF), p4b));
    PabloAST * F5_FF = ccc.compileCC(re::makeCC(0xF5, 0xFF), p4b);
    Assign * FX_err = p4b.createAssign("FX_err", p4b.createOr(F5_FF, p4b.createOr(F0_err, F4_err)));
    //
    // 4-byte prefixes have a scope that extends over the next 3 bytes.
    Assign * u8scope42 = p4b.createAssign("u8scope42", p4b.createAdvance(pfx4, 1));
    Assign * u8scope43 = p4b.createAssign("u8scope43", p4b.createAdvance(u8scope42, 1));
    Assign * u8scope44 = p4b.createAssign("u8scope44", p4b.createAdvance(u8scope43, 1));
    //
    
    //  From the 4-byte sequence 11110abc 10defghi 10jklmno 10pqrstu,
    //  we must calculate the value abcde - 1 to produce the bit values
    //  for u16_hi6, hi7, lo0, lo1 at the scope43 position.
    Assign * s43_lo1 = p4b.createAssign("scope43_lo1", p4b.createAnd(u8scope43, p4b.createNot(bit3a1)));           // e - 1
    Assign * s43_lo0 = p4b.createAssign("scope43_lo0", p4b.createAnd(u8scope43, p4b.createXor(bit2a1, s43_lo1)));  // d - borrow
    PabloAST * brw1 = p4b.createAnd(s43_lo1, p4b.createNot(bit2a1));
    Assign * s43_hi7 = p4b.createAssign("scope43_hi7", p4b.createAnd(u8scope43, p4b.createXor(bit7a2, brw1)));     // c - borrow
    PabloAST * brw2 = p4b.createAnd(brw1, p4b.createNot(bit7a2));
    Assign * s43_hi6 = p4b.createAssign("scope43_hi6", p4b.createAnd(u8scope43, p4b.createXor(bit6a2, brw2)));     // b - borrow
    //
    Assign * s43_lo2 = p4b.createAssign("scope43_lo2", p4b.createAnd(u8scope43, bit4a1));
    Assign * s43_lo3 = p4b.createAssign("scope43_lo3", p4b.createAnd(u8scope43, bit5a1));
    Assign * s43_lo4 = p4b.createAssign("scope43_lo4", p4b.createAnd(u8scope43, bit6a1));
    Assign * s43_lo5 = p4b.createAssign("scope43_lo5", p4b.createAnd(u8scope43, bit7a1));
    Assign * s43_lo6 = p4b.createAssign("scope43_lo6", p4b.createAnd(u8scope43, u8_bits[2]));
    Assign * s43_lo7 = p4b.createAssign("scope43_lo7", p4b.createAnd(u8scope43, u8_bits[3]));
    //
    //
    p34b.createIf(pfx4,
                  {FX_err, u8scope42, u8scope43, u8scope44, s43_hi6, s43_hi7,
                   s43_lo0, s43_lo1, s43_lo2, s43_lo3, s43_lo4, s43_lo5, s43_lo6, s43_lo7},
                   p4b);
    //
    // Combined logic for 3 and 4 byte sequences
    //
    PabloAST * pfx3 = ccc.compileCC(re::makeCC(0xE0, 0xEF), p34b);
    Assign * u8scope32 = p34b.createAssign("u8scope32", p34b.createAdvance(pfx3, 1));
    Assign * u8scope33 = p34b.createAssign("u8scope33", p34b.createAdvance(u8scope32, 1));

    // Illegal 3-byte sequences
    PabloAST * E0 = ccc.compileCC(re::makeCC(0xE0), p34b);
    PabloAST * ED = ccc.compileCC(re::makeCC(0xED), p34b);
    PabloAST * E0_err = p34b.createAnd(p34b.createAdvance(E0, 1), ccc.compileCC(re::makeCC(0x80, 0x9F), p34b));
    PabloAST * ED_err = p34b.createAnd(p34b.createAdvance(ED, 1), ccc.compileCC(re::makeCC(0xA0, 0xBF), p34b));
    Assign * EX_FX_err = p34b.createAssign("EX_FX_err", p34b.createOr(p34b.createOr(E0_err, ED_err), FX_err));
    // Two surrogate UTF-16 units are computed at the 3rd and 4th positions of 4-byte sequences.
    PabloAST * surrogate = p34b.createOr(u8scope43, u8scope44);
    
    Assign * p34del = p34b.createAssign("p34del", p34b.createOr(u8scope32, u8scope42));


    // The high 5 bits of the UTF-16 code unit are only nonzero for 3 and 4-byte
    // UTF-8 sequences.
    u16_hi[0] = p34b.createAssign("u16_hi0", p34b.createOr(p34b.createAnd(u8scope33, bit4a2), surrogate));
    u16_hi[1] = p34b.createAssign("u16_hi1", p34b.createOr(p34b.createAnd(u8scope33, bit5a2), surrogate));
    u16_hi[2] = p34b.createAssign("u16_hi2", p34b.createAnd(u8scope33, bit6a2));
    u16_hi[3] = p34b.createAssign("u16_hi3", p34b.createOr(p34b.createAnd(u8scope33, bit7a2), surrogate));
    u16_hi[4] = p34b.createAssign("u16_hi4", p34b.createOr(p34b.createAnd(u8scope33, bit4a1), surrogate));
    
    //
    nAb.createIf(pfx34, 
                 {u8scope33, EX_FX_err, p34del, 
                  u16_hi[0], u16_hi[1], u16_hi[2], u16_hi[3], u16_hi[4], u8scope44, s43_hi6, s43_hi7,
                  s43_lo0, s43_lo1, s43_lo2, s43_lo3, s43_lo4, s43_lo5, s43_lo6, s43_lo7},
                 p34b);
    //
    // Combined logic for 2, 3 and 4 byte sequences
    //
    PabloAST * pfx2 = ccc.compileCC(re::makeCC(0xC0, 0xDF), nAb);
    PabloAST * u8scope22 = nAb.createAdvance(pfx2, 1);
    Assign * u8lastscope = nAb.createAssign("u8lastscope", nAb.createOr(u8scope22, nAb.createOr(u8scope33, u8scope44)));
    PabloAST * u8anyscope = nAb.createOr(u8lastscope, p34del);

    PabloAST * C0_C1_err = ccc.compileCC(re::makeCC(0xC0, 0xC1), nAb);
    PabloAST * scope_suffix_mismatch = nAb.createXor(u8anyscope, ccc.compileCC(re::makeCC(0x80, 0xBF), nAb));
    error_mask = nAb.createAssign("errormask", nAb.createOr(scope_suffix_mismatch, nAb.createOr(C0_C1_err, EX_FX_err)));
    delmask = nAb.createAssign("delmask", nAb.createOr(p34del, ccc.compileCC(re::makeCC(0xC0, 0xFF), nAb)));
    
    // The low 3 bits of the high byte of the UTF-16 code unit as well as the high bit of the
    // low byte are only nonzero for 2, 3 and 4 byte sequences.
    u16_hi[5] = nAb.createAssign("u16_hi5", nAb.createOr(nAb.createAnd(u8lastscope, bit3a1), u8scope44));
    u16_hi[6] = nAb.createAssign("u16_hi6", nAb.createOr(nAb.createAnd(u8lastscope, bit4a1), s43_hi6));
    u16_hi[7] = nAb.createAssign("u16_hi7", nAb.createOr(nAb.createAnd(u8lastscope, bit5a1), s43_hi7));
    u16_lo[0] = nAb.createAssign("u16_lo0", nAb.createOr(nAb.createAnd(u8lastscope, bit6a1), s43_lo0));
    Assign * p234_lo1 = nAb.createAssign("p234_lo1", nAb.createOr(nAb.createAnd(u8lastscope, bit7a1), s43_lo1));

    pBuilder.createIf(nonASCII, 
                      {error_mask, delmask, u8lastscope,
                       u16_hi[0], u16_hi[1], u16_hi[2], u16_hi[3], u16_hi[4], u16_hi[5], u16_hi[6], u16_hi[7],
                       u16_lo[0], p234_lo1, s43_lo2, s43_lo3, s43_lo4, s43_lo5, s43_lo6, s43_lo7},
                      nAb);
    //
    //
    PabloAST * ASCII = ccc.compileCC(re::makeCC(0x0, 0x7F));
    PabloAST * last_byte = pBuilder.createOr(ASCII, u8lastscope);
    u16_lo[1] = pBuilder.createAssign("u16_lo1", pBuilder.createOr(pBuilder.createAnd(ASCII, u8_bits[1]), p234_lo1));
    u16_lo[2] = pBuilder.createAssign("u16_lo2", pBuilder.createOr(pBuilder.createAnd(last_byte, u8_bits[2]), s43_lo2));
    u16_lo[3] = pBuilder.createAssign("u16_lo3", pBuilder.createOr(pBuilder.createAnd(last_byte, u8_bits[3]), s43_lo3));
    u16_lo[4] = pBuilder.createAssign("u16_lo4", pBuilder.createOr(pBuilder.createAnd(last_byte, u8_bits[4]), s43_lo4));
    u16_lo[5] = pBuilder.createAssign("u16_lo5", pBuilder.createOr(pBuilder.createAnd(last_byte, u8_bits[5]), s43_lo5));
    u16_lo[6] = pBuilder.createAssign("u16_lo6", pBuilder.createOr(pBuilder.createAnd(last_byte, u8_bits[6]), s43_lo6));
    u16_lo[7] = pBuilder.createAssign("u16_lo7", pBuilder.createOr(pBuilder.createAnd(last_byte, u8_bits[7]), s43_lo7));
    
    for (unsigned i = 0; i < 8; i++) {
        function->setResult(i, pBuilder.createAssign("u16_hi" + std::to_string(i), u16_hi[i]));
        function->setResult(i+8, pBuilder.createAssign("u16_lo" + std::to_string(i), u16_lo[i]));
    }
    function->setResult(16, pBuilder.createAssign("delbits", delmask));
    function->setResult(17, pBuilder.createAssign("errors", error_mask));

    return function;
}
}


typedef void (*u8u16FunctionType)(char * byte_data, size_t filesize);

u8u16FunctionType u8u16CodeGen(void) {
                            
    Module * M = new Module("u8u16", getGlobalContext());
    
    IDISA::IDISA_Builder * idb = GetIDISA_Builder(M);

    kernel::PipelineBuilder pipelineBuilder(M, idb);

    Encoding encoding(Encoding::Type::UTF_8, 8);
    
    pablo::PabloFunction * function = pablo::u8u16_pablo(encoding);
    

    pipelineBuilder.CreateKernels(function);

    pipelineBuilder.ExecuteKernels();

    //std::cerr << "ExecuteKernels(); done\n";
    llvm::Function * main_IR = M->getFunction("Main");
    ExecutionEngine * mEngine = JIT_to_ExecutionEngine(M);
    
    mEngine->finalizeObject();
    //std::cerr << "finalizeObject(); done\n";

    delete idb;

    return reinterpret_cast<u8u16FunctionType>(mEngine->getPointerToFunction(main_IR));
}

void doCaseFold(u8u16FunctionType fn_ptr, const std::string & fileName) {
    std::string mFileName = fileName;
    size_t mFileSize;
    char * mFileBuffer;
    
    const path file(mFileName);
    if (exists(file)) {
        if (is_directory(file)) {
            return;
        }
    } else {
        std::cerr << "Error: cannot open " << mFileName << " for processing. Skipped.\n";
        return;
    }
    
    mFileSize = file_size(file);
    mapped_file mFile;
    if (mFileSize == 0) {
        mFileBuffer = nullptr;
    }
    else {
        try {
            mFile.open(mFileName, mapped_file::priv, mFileSize, 0);
        } catch (std::ios_base::failure e) {
            std::cerr << "Error: Boost mmap of " << mFileName << ": " << e.what() << std::endl;
            return;
        }
        mFileBuffer = mFile.data();
    }
    //std::cerr << "mFileSize =" << mFileSize << "\n";
    //std::cerr << "fn_ptr =" << std::hex << reinterpret_cast<intptr_t>(fn_ptr) << "\n";

    fn_ptr(mFileBuffer, mFileSize);

    mFile.close();
    
}


int main(int argc, char *argv[]) {
    StringMap<cl::Option*> Map;
    cl::getRegisteredOptions(Map);
    Map["time-passes"]->setHiddenFlag(cl::Hidden);
    Map["disable-spill-fusing"]->setHiddenFlag(cl::Hidden);
    Map["enable-misched"]->setHiddenFlag(cl::Hidden);
    Map["enable-tbaa"]->setHiddenFlag(cl::Hidden);
    Map["exhaustive-register-search"]->setHiddenFlag(cl::Hidden);
    Map["join-liveintervals"]->setHiddenFlag(cl::Hidden);
    Map["limit-float-precision"]->setHiddenFlag(cl::Hidden);
    Map["mc-x86-disable-arith-relaxation"]->setHiddenFlag(cl::Hidden);
    Map["limit-float-precision"]->setHiddenFlag(cl::Hidden);
    Map["print-after-all"]->setHiddenFlag(cl::Hidden);
    Map["print-before-all"]->setHiddenFlag(cl::Hidden);
    Map["print-machineinstrs"]->setHiddenFlag(cl::Hidden);
    Map["regalloc"]->setHiddenFlag(cl::Hidden);
    Map["rng-seed"]->setHiddenFlag(cl::Hidden);
    Map["stackmap-version"]->setHiddenFlag(cl::Hidden);
    Map["x86-asm-syntax"]->setHiddenFlag(cl::Hidden);
    Map["verify-debug-info"]->setHiddenFlag(cl::Hidden);
    Map["verify-dom-info"]->setHiddenFlag(cl::Hidden);
    Map["verify-loop-info"]->setHiddenFlag(cl::Hidden);
    Map["verify-regalloc"]->setHiddenFlag(cl::Hidden);
    Map["verify-scev"]->setHiddenFlag(cl::Hidden);
    Map["x86-recip-refinement-steps"]->setHiddenFlag(cl::Hidden);
    Map["rewrite-map-file"]->setHiddenFlag(cl::Hidden);

    cl::ParseCommandLineOptions(argc, argv);

    u8u16FunctionType fn_ptr = u8u16CodeGen();

    for (unsigned i = 0; i != inputFiles.size(); ++i) {
        std::cerr << inputFiles[i] << " beginning\n";
        doCaseFold(fn_ptr, inputFiles[i]);
    }

    return 0;
}

                       
