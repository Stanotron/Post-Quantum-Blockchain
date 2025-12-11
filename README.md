# Post-Quantum Signature Blockchain Benchmark

This project is a mini blockchain simulator for evaluating the performance of post-quantum digital signature schemes in a blockchain-like setting.

It focuses on lattice-based signatures:

-   ML-DSA (Dilithium) – via liboqs
    
-   Falcon – via liboqs
    
-   Hawk – via the authors’ optimized implementation
    

The blockchain itself is a small Ethereum-style account model:

-   2 accounts (Alice, Bob)
    
-   Many signed transactions from Alice → Bob
    
-   A block that stores all those transactions
    
-   Block hashes computed with SHA3-256
    

The goal is to measure realistic blockchain-shaped performance, not just isolated sign/verify operations.

* * *

## Main Features

Pluggable signature backend (AlgoFamily + variant):

-   ML-DSA-44 / 65 / 87
    
-   Falcon-512 / 1024
    
-   Hawk-512 / 1024
    

Per-algorithm measurements:

-   Wallet keygen: average time over many keypairs
    
-   Transaction signing: average time for many transactions inside one block
    
-   Block size (bytes) and average transaction size
    
-   Block validation: average time to verify all signatures in a block
    

Simple account-based model:

-   2 wallets (Alice, Bob) with PQ public keys
    
-   N transactions from Alice to Bob (e.g. 100 or 1000)
    

Hashing:

-   Block hashing with SHA3-256 (via liboqs)
    

* * *

## Repository Layout

Only the project’s own code is included; cryptographic libraries are external.

Top-level structure (simplified):

include/  
algo\_config.h – select algorithm family + variant  
crypto\_factory.h – factory that returns the right Crypto backend  
oqs\_mldsa\_crypto.h – wrapper for ML-DSA via liboqs  
oqs\_falcon\_crypto.h – wrapper for Falcon via liboqs  
hawk\_crypto.h – wrapper for Hawk AVX2 implementation  
timing.h – simple timing helpers  
block\_utils.h – integer packing + SHA3-256 wrapper  
transaction.h  
block.h  
blockchain.h  
wallet.h  
… (other small headers)

src/  
main\_crypto\_test.cpp – benchmark: pure keygen/sign/verify  
main\_blockchain.cpp – benchmark: full blockchain block  
algo\_config.cpp  
crypto\_factory.cpp  
oqs\_mldsa\_crypto.cpp  
oqs\_falcon\_crypto.cpp  
hawk\_crypto.cpp  
timing.cpp  
block\_utils.cpp  
transaction.cpp  
block.cpp  
blockchain.cpp  
wallet.cpp  
…

External code not included in this repo:

-   liboqs – for ML-DSA, Falcon, and SHA3-256
    
-   Hawk optimized implementation – for Hawk-512 / 1024 (AVX2 C code)
    

* * *

## Prerequisites

Platform used for development and testing:

-   Windows, with MSVC (Visual Studio 2022 Build Tools)
    

Required tools:

-   Visual Studio 2022 Build Tools or full Visual Studio 2022  
    (must have the “x64 Native Tools Command Prompt for VS 2022”)
    
-   CMake + a C compiler (for building liboqs if you don’t already have it)
    
-   Git (optional, for cloning)
    

External libraries (must be installed separately):

1.  liboqs (Open Quantum Safe)
        
2.  Hawk optimized AVX2 implementation

* * *

## Building (Windows / MSVC)

The project assumes something like:

-   PROJECT\_ROOT = path to this repository (e.g. D:\\pq-blockchain)
    
-   LIBOQS\_ROOT = path to liboqs (e.g. D:\\liboqs)
    
-   HAWK\_ROOT = path to Hawk AVX2 sources (e.g. D:\\oqs-hawk\\dev\\Optimized\_Implementation\\avx2)
    

You can set these as environment variables in the command prompt for convenience, or just inline the paths in the commands below.

Example environment variables:

set PROJECT\_ROOT=D:\\pq-blockchain  
set LIBOQS\_ROOT=D:\\liboqs  
set HAWK\_ROOT=D:\\oqs-hawk\\dev\\Optimized\_Implementation\\avx2

* * *

## Build script: blockchain benchmark

Create a file named “build\_blockchain.bat” in the project root:

@echo off  
setlocal

echo Building crypto\_blockchain.exe...

cl /EHsc /std:c++17 /O2 /arch:AVX2 ^  
src\\main\_blockchain.cpp ^  
src\\algo\_config.cpp ^  
src\\crypto\_factory.cpp ^  
src\\oqs\_mldsa\_crypto.cpp ^  
src\\oqs\_falcon\_crypto.cpp ^  
src\\hawk\_crypto.cpp ^  
src\\timing.cpp ^  
src\\block\_utils.cpp ^  
src\\transaction.cpp ^  
src\\block.cpp ^  
src\\blockchain.cpp ^  
src\\wallet.cpp ^  
"%HAWK\_ROOT%\*.c" ^  
/I"%PROJECT\_ROOT%\\include" ^  
/I"%LIBOQS\_ROOT%\\build\\include" ^  
/I"%HAWK\_ROOT%" ^  
/link /LIBPATH:"%LIBOQS\_ROOT%\\build\\lib\\Release" oqs.lib ^  
/out:crypto\_blockchain.exe

echo.  
echo Done. Output: crypto\_blockchain.exe  
echo.

endlocal


* * *

## Build script: pure crypto benchmark

Create a file named “build\_crypto\_test.bat”:

@echo off  
setlocal

echo Building crypto\_test.exe...

cl /EHsc /std:c++17 /O2 /arch:AVX2 ^  
src\\main\_crypto\_test.cpp ^  
src\\algo\_config.cpp ^  
src\\crypto\_factory.cpp ^  
src\\oqs\_mldsa\_crypto.cpp ^  
src\\oqs\_falcon\_crypto.cpp ^  
src\\hawk\_crypto.cpp ^  
src\\timing.cpp ^  
"%HAWK\_ROOT%\*.c" ^  
/I"%PROJECT\_ROOT%\\include" ^  
/I"%LIBOQS\_ROOT%\\build\\include" ^  
/I"%HAWK\_ROOT%" ^  
/link /LIBPATH:"%LIBOQS\_ROOT%\\build\\lib\\Release" oqs.lib ^  
/out:crypto\_test.exe

echo.  
echo Done. Output: crypto\_test.exe  
echo.

endlocal

* * *

## Compiling

1.  Open “x64 Native Tools Command Prompt for VS 2022”.
    
2.  Change to the project directory:
    
    cd %PROJECT\_ROOT%
    
3.  Build the blockchain benchmark:
    
    build\_blockchain.bat
    
4.  (Optional) Build the pure crypto benchmark:
    
    build\_crypto\_test.bat
    

If everything is configured correctly, you should see:

-   crypto\_blockchain.exe
    
-   crypto\_test.exe
    

in the project root.

* * *

## Running the benchmarks

1.  Pure crypto benchmark
    

Run:

crypto\_test.exe

Example output (Falcon-512):

Algorithm: Falcon-512 (family=Falcon, variant=512)  
PK size: 897 bytes  
SK size: 1281 bytes  
Max signature size: 752 bytes  
Keygen iterations: 1000  
Avg keygen time: 5867.12 us  
Single sign/verify: OK  
Iterations: 10000  
Avg sign time: 199.19 us  
Avg verify time: 42.03 us

This runs many iterations of keygen / sign / verify and prints mean timings.



2.  Blockchain benchmark
    

Run:

crypto\_blockchain.exe

Example output (Falcon-512, 1000 tx per block):

\=== Blockchain benchmark ===  
Algorithm: Falcon-512 (family=Falcon, variant=512)

\[Keygen\] Generating 100 wallets to measure keygen performance...  
Wallet keygen count: 100  
Avg wallet keygen time: 6026.98 us

\[Wallets\] Alice PK size: 897 bytes  
\[Wallets\] Bob PK size: 897 bytes

\[Tx\] Creating and signing 1000 transactions (Alice -> Bob)...  
Transactions per block: 1000  
Avg tx sign time (in block context): 197.665 us

\[Block\] Block index: 1  
\[Block\] Tx count: 1000  
\[Block\] Prev hash: c61d8920e8cf69c388bc2ff68105714dd07012ff5772a76a189ddcbdbf5736b8  
\[Block\] Block hash: 78943117b57788c6911892540053f6748b583e268a17c866c9e5917d0bbdc237

\[Block\] Serialized block size: 2497144 bytes  
\[Block\] Avg tx size (block\_size / tx\_count): 2497.14 bytes

\[Validate\] Running single validateBlock(block1)...  
\[Validate\] Result: OK

\[Tamper\] Flipped 1 bit of first tx's signature.  
\[Tamper\] validateBlock(tampered) result: FAIL (as expected)

\[Benchmark\] Running 100 block validation iterations...  
\[Benchmark\] Avg block verify time: 56285.3 us  
\[Benchmark\] Avg verify time per tx: 56.2853 us

This shows:

-   SHA3-256–based prev\_hash and block\_hash
    
-   Block size and average transaction size
    
-   Tampering any signature breaks validation
    
-   Average per-transaction verification cost inside a block
    

* * *

## Switching algorithms

To choose which signature scheme to benchmark, edit:

src/algo\_config.cpp

The file looks like:

#include "algo\_config.h"

// CHANGE THIS TO SWITCH ALGORITHMS  
// return {AlgoFamily::ML\_DSA, "44"};  
// return {AlgoFamily::ML\_DSA, "65"};  
// return {AlgoFamily::ML\_DSA, "87"};  
// return {AlgoFamily::FALCON, "512"};  
// return {AlgoFamily::FALCON, "1024"};  
// return {AlgoFamily::HAWK, "512"};  
// return {AlgoFamily::HAWK, "1024"};

AlgoConfig getSelectedAlgorithm() {  
return {AlgoFamily::FALCON, "512"};  
}

Change the line inside getSelectedAlgorithm, rebuild, and run again.

* * *

## Notes and Limitations

-   This is a research / educational prototype, not production blockchain software.
    
-   There is no networking, mempool, or consensus; it’s just:
    
    -   a chain of blocks,
        
    -   signed transactions,
        
    -   SHA3-256 block hashes.
        
-   All cryptographic security comes from liboqs and the Hawk implementation; this project only wraps them.
    
-   The build instructions are currently tuned for Windows + MSVC. Porting to Linux/macOS would require:
    
    -   different compiler flags,
        
    -   linking against the corresponding liboqs build,
        
    -   integrating the Hawk sources with that toolchain.
