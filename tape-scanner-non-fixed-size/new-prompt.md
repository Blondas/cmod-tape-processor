Changes to the original logic:
- return a vector structures:
  - header offset
  - collection name
  - file name
  - data_start offset (header offset + 128 bytes)
  - data_end offset (exclusive)

- the header is fixed size 128bytes
- the data size varies: it is all bytes between current and next header

The TapeScanner is designed to scan mainframe tape storage data looking for specific headers that match a given collection name. 
Here's the detailed breakdown:

Key Components:
1. The scanner uses memory-mapped file reading (via MmapFileReader)
2. It looks for fixed-size headers (128 bytes) in the data
3. It works with EBCDIC encoded data (common in mainframe systems)

Main Logic Flow:
1. Takes a memory-mapped file reader and a collection name as input
2. Return a vector metadata:
   - header offset
   - collection name
   - file name
   - data_start offset (header offset + 128 bytes)
   - data_end offset (inclusive)
3. The scanning process:
    - Pads the collection name to 44 characters with spaces
    - Converts the collection name from ASCII to EBCDIC for comparison
    - Iterates through the file and search for collection name. 
    - When you find it, it means here start next header
    - Checks if each chunk starts with the target collection name in EBCDIC

Example:
Assuming following tape structure:
1. collection_name found at offset 0
2. collection name found at offset 1000
3. collection name found at offset 1200
4. last offset on tape 1500

Return this structure (the json is only to illustrate):
```json
[
   {
      "header_offset": 0,
      "collection_name": "COLLECTION_NAME",
      "file_name": "FILENAME1",
      "data_start_offset": 128,
      "data_end_offset": 1000
   },
   {
   "header_offset": 1000,
   "collection_name": "COLLECTION_NAME",
   "file_name": "FILENAME1",
   "data_start_offset": 1128,
   "data_end_offset": 1200
   },
   {
   "header_offset": 1200,
   "collection_name": "COLLECTION_NAME",
   "file_name": "FILENAME2",
   "data_start_offset": 1328,
   "data_end_offset": 1500
   },
]
```