# Tape structure:
1. tape is divided in segments
2. each segment consist of header and data
3. this is the segment header structure in c++ - make sure you take to consideration comments:
```
typedef struct {
   char collection_name[44]; // right space padding
   char file_name[44];   // right space padding

   union {              # INT
      uint32_t filesize;  // big endian, represents total file size
      uint8_t filesize_b[4];  
   };
   uint32_t _unknown;  // zero padding  # INT
   union {
      uint32_t segsize;  // big endian, represents this segment size
      uint8_t segsize_b[4];
   };
   uint8_t oam_tag[28];  // *OAM zero padded
} header_t;
```
* the whole header is padded to 128 bytes and encoded in EBCIDIC
4. The tape stores files
5. Files consist of  1-to-n segments
6. On 1 tape there is only 1 collection
7. Collection consists of 1-to-n files
8. Tape data is binary