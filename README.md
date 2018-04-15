# Compression-decompression-engine

Simulator for code compression and decompression engine is implemented in C++. 
A dictionary is populated which can have eight entries (index 3 bits) and the eight entries are selected based on frequency (the most frequent instruction should have index 000). The original code consists of 32-bit binaries.
If one entry (32-bit binary) can be compressed in more than one way, then the most beneficial one i.e., the one that provides the shortest compressed pattern is chosen. If two formats produce exactly the same compression, the one that appears earlier in the following listing (e.g., run-length encoding appears earlier than direct matching) is chosen.
Formats:
 - Run length encoding
 - Bit-mask based compression
 - 1-bit mismatch
 - 2-bit consecutive mismatch
 - 2-bit mismatch anywhere
 - Direct matching
 - Original binary
